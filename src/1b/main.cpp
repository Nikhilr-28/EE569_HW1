/*
Name: Nikhil Ravichandran
ID: 8515860149
Email: nikhilr@usc.edu
Date: February 1, 2026
*/
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdint>
#include <cmath>
#include <algorithm>

#include "RawIO.h"

// Write (x,y) pairs to CSV: header + rows
static bool write_csv_pairs(const std::string& path,
                            const std::string& header1,
                            const std::string& header2,
                            const std::vector<long long>& y)
{
    std::ofstream ofs(path);
    if (!ofs) return false;
    ofs << header1 << "," << header2 << "\n";
    for (int i = 0; i < (int)y.size(); i++) {
        ofs << i << "," << y[i] << "\n";
    }
    return true;
}

static bool write_csv_transfer(const std::string& path,
                               const std::vector<int>& map)
{
    std::ofstream ofs(path);
    if (!ofs) return false;
    ofs << "input,output\n";
    for (int i = 0; i < 256; i++) {
        ofs << i << "," << map[i] << "\n";
    }
    return true;
}

static bool write_csv_cdf_before_after(const std::string& path,
                                       const std::vector<double>& cdf_before,
                                       const std::vector<double>& cdf_after)
{
    std::ofstream ofs(path);
    if (!ofs) return false;
    ofs << "intensity,cdf_before,cdf_after\n";
    for (int i = 0; i < 256; i++) {
        ofs << i << "," << cdf_before[i] << "," << cdf_after[i] << "\n";
    }
    return true;
}

// Compute histogram for 8-bit grayscale image
static std::vector<long long> histogram_u8(const std::vector<uint8_t>& img)
{
    std::vector<long long> hist(256, 0);
    for (uint8_t v : img) hist[v]++;
    return hist;
}

// Compute normalized CDF from histogram (values in [0,1])
static std::vector<double> cdf_from_hist(const std::vector<long long>& hist, long long total)
{
    std::vector<double> cdf(256, 0.0);
    long long running = 0;
    for (int i = 0; i < 256; i++) {
        running += hist[i];
        cdf[i] = (total > 0) ? (double)running / (double)total : 0.0;
    }
    return cdf;
}

// -------------------------
// Method A: Transfer-function HE
// map[i] = round(255 * CDF(i))
// -------------------------
static std::vector<int> he_method_A_mapping(const std::vector<long long>& hist, long long total)
{
    std::vector<int> map(256, 0);
    long long running = 0;
    for (int i = 0; i < 256; i++) {
        running += hist[i];
        double cdf = (total > 0) ? (double)running / (double)total : 0.0;
        int out = (int)std::lround(255.0 * cdf);
        out = std::clamp(out, 0, 255);
        map[i] = out;
    }
    return map;
}

static std::vector<uint8_t> apply_mapping_u8(const std::vector<uint8_t>& img,
                                             const std::vector<int>& map)
{
    std::vector<uint8_t> out(img.size(), 0);
    for (size_t i = 0; i < img.size(); i++) {
        out[i] = (uint8_t)map[img[i]];
    }
    return out;
}

// -------------------------
// Method B: Bucket-filling HE
//
// Idea: each pixel gets assigned an output bin based on its global rank.
// For intensity i, pixels occupy ranks [start, end).
// For each pixel, output = floor(256 * rank / N), clamped to 0..255.
// We implement rank by scanning image and counting occurrences per intensity.
// -------------------------
static std::vector<uint8_t> he_method_B_bucket_fill(const std::vector<uint8_t>& img,
                                                    const std::vector<long long>& hist)
{
    const long long N = (long long)img.size();
    if (N <= 0) return {};

    // prefix counts: start rank for each intensity i
    std::vector<long long> prefix(256, 0);
    long long running = 0;
    for (int i = 0; i < 256; i++) {
        prefix[i] = running;
        running += hist[i];
    }

    // occurrence counter per intensity while scanning pixels
    std::vector<long long> seen(256, 0);

    std::vector<uint8_t> out(img.size(), 0);
    for (size_t p = 0; p < img.size(); p++) {
        const int v = img[p];
        const long long k = seen[v]++;                 // 0..hist[v]-1
        const long long rank = prefix[v] + k;          // 0..N-1

        // bucket index: floor(256 * rank / N)
        long long outv = (256LL * rank) / N;           // 0..255
        if (outv < 0) outv = 0;
        if (outv > 255) outv = 255;
        out[p] = (uint8_t)outv;
    }
    return out;
}

int main()
{
    // airplane.raw is 1024x1024, 8-bit grayscale
    const int width = 1024;
    const int height = 1024;

    const std::string in_path  = "A:\\DIP\\HW_1\\EE569_HW1\\images\\airplane.raw";
    const std::string outA_raw = "A:\\DIP\\HW_1\\EE569_HW1\\images\\1b\\airplane_A.raw";
    const std::string outB_raw = "A:\\DIP\\HW_1\\EE569_HW1\\images\\1b\\airplane_B.raw";

    const std::string hist_csv    = "A:\\DIP\\HW_1\\EE569_HW1\\images\\1b\\hist_original.csv";
    const std::string transfer_csv= "A:\\DIP\\HW_1\\EE569_HW1\\images\\1b\\transfer_A.csv";
    const std::string cdf_csv     = "A:\\DIP\\HW_1\\EE569_HW1\\images\\1b\\cdf_B.csv";

    std::vector<uint8_t> img;
    if (!rawio::read_raw_u8(in_path, width, height, 1, img)) {
        return 1;
    }

    const long long N = (long long)img.size();

    // (1) Histogram of original
    const auto hist0 = histogram_u8(img);
    if (!write_csv_pairs(hist_csv, "intensity", "count", hist0)) {
        std::cerr << "ERROR: failed to write " << hist_csv << "\n";
        return 1;
    }
    std::cout << "Wrote: " << hist_csv << "\n";

    // (2) Method A
    const auto mapA = he_method_A_mapping(hist0, N);
    const auto imgA = apply_mapping_u8(img, mapA);
    if (!rawio::write_raw_u8(outA_raw, imgA)) return 1;
    if (!write_csv_transfer(transfer_csv, mapA)) {
        std::cerr << "ERROR: failed to write " << transfer_csv << "\n";
        return 1;
    }
    std::cout << "Wrote: " << outA_raw << "\n";
    std::cout << "Wrote: " << transfer_csv << "\n";

    // (3) Method B
    const auto imgB = he_method_B_bucket_fill(img, hist0);
    if (imgB.empty()) {
        std::cerr << "ERROR: Method B failed.\n";
        return 1;
    }
    if (!rawio::write_raw_u8(outB_raw, imgB)) return 1;
    std::cout << "Wrote: " << outB_raw << "\n";

    // CDF before and after Method B (for the plot they ask)
    const auto cdf_before = cdf_from_hist(hist0, N);
    const auto histB = histogram_u8(imgB);
    const auto cdf_after = cdf_from_hist(histB, N);

    if (!write_csv_cdf_before_after(cdf_csv, cdf_before, cdf_after)) {
        std::cerr << "ERROR: failed to write " << cdf_csv << "\n";
        return 1;
    }
    std::cout << "Wrote: " << cdf_csv << "\n";

    std::cout << "\nDone.\n";
    std::cout << "Open airplane_A.raw and airplane_B.raw in ImageJ as RAW 1024x1024, 8-bit.\n";
    return 0;
}
