/*
Name: Nikhil Ravichandran
ID: 8515860149
Email: nikhilr@usc.edu
Date: February 1, 2026
*/
#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace rawio {

// Reads a RAW file with exactly (width * height * channels) bytes.
// Returns data in row-major order, interleaved by channels if channels > 1.
bool read_raw_u8(const std::string& path,
                 int width, int height, int channels,
                 std::vector<uint8_t>& out);

// Writes RAW bytes exactly as provided.
bool write_raw_u8(const std::string& path,
                  const std::vector<uint8_t>& data);

} // namespace rawio
