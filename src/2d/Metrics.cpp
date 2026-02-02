#include "Metrics.h"
#include <cmath>
#include <stdexcept>

namespace metrics {

double mse8(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
    if (a.size() != b.size()) throw std::runtime_error("mse8: size mismatch");
    const size_t n = a.size();
    if (n == 0) return 0.0;

    double acc = 0.0;
    for (size_t i = 0; i < n; ++i) {
        const double d = static_cast<double>(a[i]) - static_cast<double>(b[i]);
        acc += d * d;
    }
    return acc / static_cast<double>(n);
}

double psnrFromMSE(double mse, double maxVal) {
    if (mse <= 0.0) return INFINITY;
    const double ratio = (maxVal * maxVal) / mse;
    return 10.0 * std::log10(ratio);
}

}
