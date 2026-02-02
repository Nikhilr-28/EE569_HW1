#include "RawIO.h"
#include <fstream>
#include <iostream>

namespace rawio {

static size_t expected_bytes(int width, int height) {
    return static_cast<size_t>(width) * static_cast<size_t>(height) * 3ULL;
}

bool read_rgb_raw_u8(const std::string& path, int width, int height, std::vector<uint8_t>& out) {
    out.clear();

    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: cannot open input file: " << path << "\n";
        return false;
    }

    const size_t nbytes = expected_bytes(width, height);
    out.resize(nbytes);

    ifs.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(nbytes));
    if (!ifs) {
        std::cerr << "Error: failed to read expected bytes. Expected " << nbytes << " bytes.\n";
        return false;
    }
    return true;
}

bool write_rgb_raw_u8(const std::string& path, int width, int height, const std::vector<uint8_t>& data) {
    const size_t nbytes = expected_bytes(width, height);
    if (data.size() != nbytes) {
        std::cerr << "Error: write size mismatch. Got " << data.size() << " expected " << nbytes << "\n";
        return false;
    }

    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error: cannot open output file: " << path << "\n";
        return false;
    }

    ofs.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(nbytes));
    if (!ofs) {
        std::cerr << "Error: failed to write output bytes.\n";
        return false;
    }
    return true;
}

} // namespace rawio
