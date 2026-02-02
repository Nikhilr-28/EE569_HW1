#pragma once
#include <vector>
#include <cstdint>

namespace metrics {

double mse8(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);
double psnrFromMSE(double mse, double maxVal = 255.0);

}
