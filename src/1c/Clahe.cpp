/*
Name: Nikhil Ravichandran
ID: 8515860149
Email: nikhilr@usc.edu
Date: February 1, 2026
*/
#include "Clahe.h"

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <cstring>


namespace clahe {

std::vector<uint8_t> apply(const std::vector<uint8_t>& img_u8,
                           int width, int height,
                           int tile_x, int tile_y,
                           double clip_limit)
{
    if ((int)img_u8.size() != width * height) return {};

    cv::Mat in(height, width, CV_8UC1);
    std::memcpy(in.data, img_u8.data(), (size_t)width * height);

    cv::Mat out;

    cv::Ptr<cv::CLAHE> c = cv::createCLAHE();
    c->setClipLimit(clip_limit);
    c->setTilesGridSize(cv::Size(tile_x, tile_y));
    c->apply(in, out);

    std::vector<uint8_t> result((size_t)width * height);
    std::memcpy(result.data(), out.data, result.size());
    return result;
}

} // namespace clahe
