#include "RawIO.h"
#include <fstream>

namespace rawio {

static bool readBytes(const std::string& path, std::vector<uint8_t>& buf, size_t expected) {
    buf.assign(expected, 0);

    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return false;

    ifs.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(expected));
    const size_t got = static_cast<size_t>(ifs.gcount());
    return got == expected;
}

static bool writeBytes(const std::string& path, const std::vector<uint8_t>& buf) {
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) return false;

    ofs.write(reinterpret_cast<const char*>(buf.data()),
              static_cast<std::streamsize>(buf.size()));
    return static_cast<bool>(ofs);
}

// -------------------- Grayscale --------------------

bool readRaw8(const std::string& path, int w, int h, std::vector<uint8_t>& out) {
    if (w <= 0 || h <= 0) return false;
    const size_t expected = static_cast<size_t>(w) * static_cast<size_t>(h);
    return readBytes(path, out, expected);
}

bool writeRaw8(const std::string& path, const std::vector<uint8_t>& data) {
    return writeBytes(path, data);
}

// -------------------- RGB --------------------

bool readRawRGB(const std::string& path, int w, int h, std::vector<uint8_t>& outRGB) {
    if (w <= 0 || h <= 0) return false;
    const size_t expected = static_cast<size_t>(w) * static_cast<size_t>(h) * 3ull;
    return readBytes(path, outRGB, expected);
}

bool writeRawRGB(const std::string& path, const std::vector<uint8_t>& rgb) {
    return writeBytes(path, rgb);
}

} // namespace rawio
