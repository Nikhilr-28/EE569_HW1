#include "RawIO.h"
#include "Metrics.h"

#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdint>
#include <cstring>
#include <algorithm>

static cv::Mat rgbVecToMatCopy(const std::vector<uint8_t>& rgb, int W, int H) {
    // OpenCV uses BGR by default, but we can store as RGB and convert later.
    cv::Mat m(H, W, CV_8UC3);
    std::memcpy(m.data, rgb.data(), static_cast<size_t>(W) * static_cast<size_t>(H) * 3ull);
    return m;
}

static std::vector<uint8_t> matToRgbVecCopy(const cv::Mat& m) {
    std::vector<uint8_t> out(static_cast<size_t>(m.total()) * 3ull);
    std::memcpy(out.data(), m.data, out.size());
    return out;
}

static void splitChannels(
    const std::vector<uint8_t>& rgb, std::vector<uint8_t>& R,
    std::vector<uint8_t>& G, std::vector<uint8_t>& B
) {
    const size_t nPix = rgb.size() / 3ull;
    R.resize(nPix); G.resize(nPix); B.resize(nPix);
    for (size_t i = 0; i < nPix; ++i) {
        R[i] = rgb[3*i + 0];
        G[i] = rgb[3*i + 1];
        B[i] = rgb[3*i + 2];
    }
}

static void printPSNR_RGB(const std::string& tag,
                          const std::vector<uint8_t>& origRGB,
                          const std::vector<uint8_t>& testRGB) {
    std::vector<uint8_t> oR,oG,oB, tR,tG,tB;
    splitChannels(origRGB, oR,oG,oB);
    splitChannels(testRGB, tR,tG,tB);

    const double mseR = metrics::mse8(oR, tR);
    const double mseG = metrics::mse8(oG, tG);
    const double mseB = metrics::mse8(oB, tB);

    const double psnrR = metrics::psnrFromMSE(mseR);
    const double psnrG = metrics::psnrFromMSE(mseG);
    const double psnrB = metrics::psnrFromMSE(mseB);
    const double psnrAvg = (psnrR + psnrG + psnrB) / 3.0;

    std::cout << tag << "\n";
    std::cout << "PSNR_R = " << psnrR << " dB\n";
    std::cout << "PSNR_G = " << psnrG << " dB\n";
    std::cout << "PSNR_B = " << psnrB << " dB\n";
    std::cout << "PSNR_avg = " << psnrAvg << " dB\n\n";
}

static void impulseStats(const std::vector<uint8_t>& rgb, const std::string& name) {
    // crude evidence: fraction of channel values near 0 or near 255
    const int lowThr = 2;
    const int highThr = 253;

    size_t low = 0, high = 0;
    for (uint8_t v : rgb) {
        if (v <= lowThr) low++;
        if (v >= highThr) high++;
    }
    const double total = static_cast<double>(rgb.size());
    std::cout << "[Impulse evidence] " << name << "\n";
    std::cout << "  <= " << lowThr << ": " << (100.0 * low / total) << " %\n";
    std::cout << "  >= " << highThr << ": " << (100.0 * high / total) << " %\n\n";
}

int main() {
    const int W = 768;
    const int H = 512;

    const std::string baseImgDir = "A:/DIP/HW_1/EE569_HW1/images";
    const std::string origPath   = baseImgDir + "/flower.raw";
    const std::string noisyPath  = baseImgDir + "/flower_noisy.raw";
    const std::string outDir     = baseImgDir + "/2d";
    std::filesystem::create_directories(outDir);

    std::vector<uint8_t> origRGB, noisyRGB;
    if (!rawio::readRawRGB(origPath, W, H, origRGB)) {
        std::cerr << "Failed to read original: " << origPath << "\n";
        return 1;
    }
    if (!rawio::readRawRGB(noisyPath, W, H, noisyRGB)) {
        std::cerr << "Failed to read noisy: " << noisyPath << "\n";
        return 1;
    }

    // Noise-type evidence
    impulseStats(noisyRGB, "flower_noisy.raw");

    // Baseline PSNR
    printPSNR_RGB("Baseline (Noisy vs Original)", origRGB, noisyRGB);

    // Convert to OpenCV Mat (RGB layout in memory)
    cv::Mat noisy = rgbVecToMatCopy(noisyRGB, W, H);

    // OpenCV expects BGR for many functions; convert RGB->BGR
    cv::Mat noisyBGR;
    cv::cvtColor(noisy, noisyBGR, cv::COLOR_RGB2BGR);

    // Step 1: Median filter to remove impulse noise
    cv::Mat medBGR;
    cv::medianBlur(noisyBGR, medBGR, 3);

    // Convert back to RGB for writing + metrics
    cv::Mat medRGB;
    cv::cvtColor(medBGR, medRGB, cv::COLOR_BGR2RGB);
    auto medVec = matToRgbVecCopy(medRGB);

    const std::string outMed = outDir + "/flower_med3.raw";
    rawio::writeRawRGB(outMed, medVec);
    printPSNR_RGB("[Median 3x3 output -> " + outMed + "]", origRGB, medVec);

    // Step 2: NLM color denoising (OpenCV)
    // h: luminance strength, hColor: chroma strength
    // templateWindowSize: patch size, searchWindowSize: search area
    const float h = 10.f;
    const float hColor = 10.f;
    const int templateWindowSize = 7;
    const int searchWindowSize = 21;

    cv::Mat nlmBGR;
    cv::fastNlMeansDenoisingColored(
        medBGR, nlmBGR,
        h, hColor,
        templateWindowSize,
        searchWindowSize
    );

    cv::Mat nlmRGB;
    cv::cvtColor(nlmBGR, nlmRGB, cv::COLOR_BGR2RGB);
    auto nlmVec = matToRgbVecCopy(nlmRGB);

    const std::string outFinal = outDir + "/flower_med3_nlm.raw";
    rawio::writeRawRGB(outFinal, nlmVec);
    printPSNR_RGB("[Median 3x3 + NLM-color output -> " + outFinal + "]", origRGB, nlmVec);

    std::cout << "Done.\n";
    return 0;
}
