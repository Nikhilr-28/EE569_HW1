#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace rawio {

bool readRaw8(const std::string& path, int width, int height, std::vector<uint8_t>& out);
bool writeRaw8(const std::string& path, const std::vector<uint8_t>& data);

}
