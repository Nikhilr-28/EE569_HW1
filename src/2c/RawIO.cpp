#include "RawIO.h"
#include <fstream>

namespace rawio {

bool readRaw8(const std::string& path, int width, int height, std::vector<uint8_t>& out) {
    const size_t n = static_cast<size_t>(width) * static_cast<size_t>(height);
    out.assign(n, 0);

    std::ifstream in(path, std::ios::binary);
    if (!in) return false;

    in.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(n));
    return in.good() || in.eof();
}

bool writeRaw8(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    return out.good();
}

}
