#include "RawIO.h"
#include "Awb.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

static void print_stats(const std::string& title, const awb::Stats& s) {
    std::cout << "\n" << title << "\n";
    std::cout << "Mean R: " << s.meanR << "\n";
    std::cout << "Mean G: " << s.meanG << "\n";
    std::cout << "Mean B: " << s.meanB << "\n";
    std::cout << "Target mean (gray): " << s.targetMean << "\n";
    std::cout << "Gains alphaR, alphaG, alphaB: " << s.gainR << ", " << s.gainG << ", " << s.gainB << "\n";
    std::cout << "Clipping low/high (only for AWB output): " << s.clippedLow << " / " << s.clippedHigh << "\n";
}

int main(int argc, char** argv) {
    // Default dimensions. Change here if your assignment specifies different size.
    int width = 768;
    int height = 512;

    // Expected usage:
    // awb.exe <input_raw> <output_dir> [width height]
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_raw> <output_dir> [width height]\n";
        std::cerr << "Example: " << argv[0] << " A:/DIP/HW_1/EE569_HW1/images/sea.raw A:/DIP/HW_1/EE569_HW1/images/3 768 512\n";
        return 1;
    }

    const std::string inPath = argv[1];
    const std::string outDir = argv[2];
    if (argc >= 5) {
        width = std::stoi(argv[3]);
        height = std::stoi(argv[4]);
    }

    const std::string outAwbPath = outDir + "/sea_awb.raw";
    const std::string histBeforePath = outDir + "/hist_before.csv";
    const std::string histAfterPath  = outDir + "/hist_after.csv";
    const std::string summaryPath    = outDir + "/summary.txt";

    std::vector<uint8_t> img;
    if (!rawio::read_rgb_raw_u8(inPath, width, height, img)) {
        return 1;
    }

    // BEFORE stats + hist
    awb::Stats before = awb::compute_stats(img);
    awb::Hist256 hR0, hG0, hB0;
    awb::compute_histograms(img, hR0, hG0, hB0);
    if (!awb::write_hist_csv(histBeforePath, hR0, hG0, hB0)) return 1;

    // AWB
    awb::Stats awbStats;
    std::vector<uint8_t> imgAwb = awb::grey_world_awb(img, awbStats);
    if (!rawio::write_rgb_raw_u8(outAwbPath, width, height, imgAwb)) {
        return 1;
    }

    // AFTER stats + hist
    awb::Stats after = awb::compute_stats(imgAwb);
    awb::Hist256 hR1, hG1, hB1;
    awb::compute_histograms(imgAwb, hR1, hG1, hB1);
    if (!awb::write_hist_csv(histAfterPath, hR1, hG1, hB1)) return 1;

    // Print
    print_stats("Before AWB", before);
    print_stats("AWB applied (gains/clipping)", awbStats);
    print_stats("After AWB (means recomputed)", after);

    // Summary txt for your report writeup convenience
    std::ofstream s(summaryPath);
    if (s) {
        s << "Problem 3: Grey World AWB\n\n";
        s << "Input: " << inPath << "\n";
        s << "Size: " << width << " x " << height << "\n\n";
        s << "Before AWB means: R=" << before.meanR << ", G=" << before.meanG << ", B=" << before.meanB << "\n";
        s << "Target mean: " << before.targetMean << "\n";
        s << "Gains: alphaR=" << awbStats.gainR << ", alphaG=" << awbStats.gainG << ", alphaB=" << awbStats.gainB << "\n";
        s << "Clipping low/high: " << awbStats.clippedLow << " / " << awbStats.clippedHigh << "\n\n";
        s << "After AWB means: R=" << after.meanR << ", G=" << after.meanG << ", B=" << after.meanB << "\n\n";
        s << "Histogram CSVs:\n";
        s << "  " << histBeforePath << "\n";
        s << "  " << histAfterPath << "\n";
        s << "Output image:\n";
        s << "  " << outAwbPath << "\n";
    }

    std::cout << "\nWrote:\n";
    std::cout << "  " << outAwbPath << "\n";
    std::cout << "  " << histBeforePath << "\n";
    std::cout << "  " << histAfterPath << "\n";
    std::cout << "  " << summaryPath << "\n";
    return 0;
}
