#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace rawio {

// Reads an 8-bit RGB interleaved raw file: [R,G,B,R,G,B,...]
bool read_rgb_raw_u8(const std::string& path, int width, int height, std::vector<uint8_t>& out);

// Writes an 8-bit RGB interleaved raw file
bool write_rgb_raw_u8(const std::string& path, int width, int height, const std::vector<uint8_t>& data);

} // namespace rawio
