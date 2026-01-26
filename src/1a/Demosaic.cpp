#include "Demosaic.h"
#include <algorithm>
#include <iostream>

static inline int clamp_int(int v, int lo, int hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

static inline uint8_t get_cfa(const std::vector<uint8_t>& cfa,
                              int x, int y, int w, int h)
{
    // Border handling: replicate (clamp)
    x = clamp_int(x, 0, w - 1);
    y = clamp_int(y, 0, h - 1);
    return cfa[y * w + x];
}

static inline uint8_t avg2(uint8_t a, uint8_t b) {
    return static_cast<uint8_t>((static_cast<int>(a) + static_cast<int>(b)) / 2);
}

static inline uint8_t avg4(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return static_cast<uint8_t>((static_cast<int>(a) + static_cast<int>(b) +
                                 static_cast<int>(c) + static_cast<int>(d)) / 4);
}

// Pattern (1-indexed in the PDF) looks like:
// Row1: G R G R ...
// Row2: B G B G ...
// This is GRBG if top-left is G.
static inline bool is_green(int x, int y) {
    // 0-indexed: (even, even) = G; (odd, odd) = G
    return ((x % 2) == (y % 2));
}
static inline bool is_red(int x, int y) {
    // 0-indexed: row even, col odd => R
    return (y % 2 == 0) && (x % 2 == 1);
}
static inline bool is_blue(int x, int y) {
    // 0-indexed: row odd, col even => B
    return (y % 2 == 1) && (x % 2 == 0);
}

std::vector<uint8_t> demosaic_bilinear_grbg(
    const std::vector<uint8_t>& cfa,
    int width,
    int height
) {
    if ((int)cfa.size() != width * height) {
        std::cerr << "ERROR: CFA size mismatch.\n";
        return {};
    }

    std::vector<uint8_t> rgb(width * height * 3, 0);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            uint8_t R = 0, G = 0, B = 0;
            const uint8_t center = get_cfa(cfa, x, y, width, height);

            if (is_red(x, y)) {
                // R known
                R = center;
                // G from N,S,E,W (greens)
                G = avg4(
                    get_cfa(cfa, x, y - 1, width, height),
                    get_cfa(cfa, x, y + 1, width, height),
                    get_cfa(cfa, x - 1, y, width, height),
                    get_cfa(cfa, x + 1, y, width, height)
                );
                // B from diagonals
                B = avg4(
                    get_cfa(cfa, x - 1, y - 1, width, height),
                    get_cfa(cfa, x + 1, y - 1, width, height),
                    get_cfa(cfa, x - 1, y + 1, width, height),
                    get_cfa(cfa, x + 1, y + 1, width, height)
                );
            }
            else if (is_blue(x, y)) {
                // B known
                B = center;
                // G from N,S,E,W (greens)
                G = avg4(
                    get_cfa(cfa, x, y - 1, width, height),
                    get_cfa(cfa, x, y + 1, width, height),
                    get_cfa(cfa, x - 1, y, width, height),
                    get_cfa(cfa, x + 1, y, width, height)
                );
                // R from diagonals
                R = avg4(
                    get_cfa(cfa, x - 1, y - 1, width, height),
                    get_cfa(cfa, x + 1, y - 1, width, height),
                    get_cfa(cfa, x - 1, y + 1, width, height),
                    get_cfa(cfa, x + 1, y + 1, width, height)
                );
            }
            else if (is_green(x, y)) {
                // G known
                G = center;

                // Two kinds of green:
                // If on a "red row" (y even): neighbors left/right are red, up/down are blue
                // If on a "blue row" (y odd): neighbors up/down are red, left/right are blue
                if (y % 2 == 0) {
                    // green on red row: R from left/right, B from up/down
                    R = avg2(
                        get_cfa(cfa, x - 1, y, width, height),
                        get_cfa(cfa, x + 1, y, width, height)
                    );
                    B = avg2(
                        get_cfa(cfa, x, y - 1, width, height),
                        get_cfa(cfa, x, y + 1, width, height)
                    );
                } else {
                    // green on blue row: R from up/down, B from left/right
                    R = avg2(
                        get_cfa(cfa, x, y - 1, width, height),
                        get_cfa(cfa, x, y + 1, width, height)
                    );
                    B = avg2(
                        get_cfa(cfa, x - 1, y, width, height),
                        get_cfa(cfa, x + 1, y, width, height)
                    );
                }
            } else {
                // Should never happen
                R = G = B = center;
            }

            const int idx = (y * width + x) * 3;
            rgb[idx + 0] = R;
            rgb[idx + 1] = G;
            rgb[idx + 2] = B;
        }
    }

    return rgb;
}
