#pragma once
#include <vector>
#include <cstdint>

namespace he {

// Method A: Transfer-function-based histogram equalization on 8-bit image.
std::vector<uint8_t> equalize_A(const std::vector<uint8_t>& img_u8);

// Method B: Bucket-filling histogram equalization on 8-bit image.
std::vector<uint8_t> equalize_B(const std::vector<uint8_t>& img_u8);

} // namespace he
