#include "RawIO.h"
#include "Metrics.h"

#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdint>
#include <cstring>

static cv::Mat vecToMatGrayCopy(const std::vector<uint8_t>& v, int W, int H) {
    // Make a safe copy into cv::Mat
    cv::Mat m(H, W, CV_8UC1);
    std::memcpy(m.data, v.data(), static_cast<size_t>(W) * static_cast<size_t>(H));
    return m;
}

static std::vector<uint8_t> matToVecGrayCopy(const cv::Mat& m) {
    std::vector<uint8_t> v(static_cast<size_t>(m.total()));
    std::memcpy(v.data(), m.data, v.size());
    return v;
}

int main() {
    const int W = 768;
    const int H = 512;

    const std::string baseImgDir = "A:/DIP/HW_1/EE569_HW1/images";
    const std::string origPath   = baseImgDir + "/flower_gray.raw";
    const std::string noisyPath  = baseImgDir + "/flower_gray_noisy.raw";
    const std::string outDir     = baseImgDir + "/2c";

    std::filesystem::create_directories(outDir);

    std::vector<uint8_t> origVec, noisyVec;
    if (!rawio::readRaw8(origPath, W, H, origVec)) {
        std::cerr << "Failed to read original: " << origPath << "\n";
        return 1;
    }
    if (!rawio::readRaw8(noisyPath, W, H, noisyVec)) {
        std::cerr << "Failed to read noisy: " << noisyPath << "\n";
        return 1;
    }

    // Baseline metrics
    const double mseBase  = metrics::mse8(origVec, noisyVec);
    const double psnrBase = metrics::psnrFromMSE(mseBase);

    std::cout << "Baseline (Noisy vs Original)\n";
    std::cout << "MSE  = " << mseBase << "\n";
    std::cout << "PSNR = " << psnrBase << " dB\n\n";

    // Convert noisy to cv::Mat
    const cv::Mat noisy = vecToMatGrayCopy(noisyVec, W, H);

    // Fixed windows (based on your earlier sweep)
    const int templateWindowSize = 7; // N' (patch size)
    const int searchWindowSize   = 31; // big search window

    // Focused h sweep
    const std::vector<float> hVals = {20.f, 25.f, 30.f, 35.f};

    // Track best by PSNR
    double bestPSNR = -1.0;
    double bestMSE  = 0.0;
    float bestH     = 0.f;
    std::string bestOutPath;

    for (float h : hVals) {
        cv::Mat den;
        cv::fastNlMeansDenoising(
            noisy, den,
            h,
            templateWindowSize,
            searchWindowSize
        );

        auto denVec = matToVecGrayCopy(den);

        const double mse  = metrics::mse8(origVec, denVec);
        const double psnr = metrics::psnrFromMSE(mse);

        std::string tag =
            "flower_nlm_h" + std::to_string((int)h) +
            "_t" + std::to_string(templateWindowSize) +
            "_s" + std::to_string(searchWindowSize);

        std::string outPath = outDir + "/" + tag + ".raw";
        rawio::writeRaw8(outPath, denVec);

        std::cout << "[NLM] h=" << h
                  << " template=" << templateWindowSize
                  << " search=" << searchWindowSize << "\n";
        std::cout << "MSE  = " << mse << "\n";
        std::cout << "PSNR = " << psnr << " dB\n";
        std::cout << "Output -> " << outPath << "\n\n";

        if (psnr > bestPSNR) {
            bestPSNR = psnr;
            bestMSE  = mse;
            bestH    = h;
            bestOutPath = outPath;
        }
    }

    std::cout << "Best NLM (by PSNR)\n";
    std::cout << "h=" << bestH
              << " template=" << templateWindowSize
              << " search=" << searchWindowSize << "\n";
    std::cout << "MSE  = " << bestMSE << "\n";
    std::cout << "PSNR = " << bestPSNR << " dB\n";
    std::cout << "Output -> " << bestOutPath << "\n";

    return 0;
}
