/*
Name: Nikhil Ravichandran
ID: 8515860149
Email: nikhilr@usc.edu
Date: February 1, 2026
*/
#include "RawIO.h"
#include <fstream>
#include <iostream>

namespace rawio {

bool read_raw_u8(const std::string& path,
                 int width, int height, int channels,
                 std::vector<uint8_t>& out)
{
    const long long expected = 1LL * width * height * channels;
    out.assign((size_t)expected, 0);

    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) {
        std::cerr << "ERROR: Cannot open input file: " << path << "\n";
        return false;
    }

    ifs.read(reinterpret_cast<char*>(out.data()), expected);
    const std::streamsize got = ifs.gcount();

    if (got != expected) {
        std::cerr << "ERROR: Read " << got << " bytes, expected " << expected
                  << " bytes. Check width/height/channels.\n";
        return false;
    }
    return true;
}

bool write_raw_u8(const std::string& path,
                  const std::vector<uint8_t>& data)
{
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) {
        std::cerr << "ERROR: Cannot open output file: " << path << "\n";
        return false;
    }
    ofs.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
    if (!ofs) {
        std::cerr << "ERROR: Failed while writing: " << path << "\n";
        return false;
    }
    return true;
}

} // namespace rawio
