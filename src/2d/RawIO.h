#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace rawio {

// 8-bit grayscale RAW (W*H bytes)
bool readRaw8(const std::string& path, int w, int h, std::vector<uint8_t>& out);
bool writeRaw8(const std::string& path, const std::vector<uint8_t>& data);

// 24-bit RGB RAW (W*H*3 bytes), interleaved RGBRGB...
bool readRawRGB(const std::string& path, int w, int h, std::vector<uint8_t>& outRGB);
bool writeRawRGB(const std::string& path, const std::vector<uint8_t>& rgb);

} // namespace rawio
