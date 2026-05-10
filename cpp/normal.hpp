#pragma once
#include <unordered_map>
#include <cstring>
#include <cmath>
#include "constants.hpp"
#include "shading.hpp"

static const int BIT_DEPTH = 12;
static const int QUANT     = 1 << BIT_DEPTH; // 4096

// normal buffer layout: [nx: N*N][ny: N*N][nz: N*N][nw: N*N]
// nw = 1 if pixel resolved, 0 otherwise

void resolve_normal(const double* dem, double* normal,
                    double t1, double p1,
                    double t2, double p2,
                    double t3, double p3) {
    static double sh1[IMG_N * IMG_N], sh2[IMG_N * IMG_N], sh3[IMG_N * IMG_N];
    static double rf1[REFL_N * REFL_N], rf2[REFL_N * REFL_N], rf3[REFL_N * REFL_N];

    compute_shading(dem, sh1, t1, p1);
    compute_shading(dem, sh2, t2, p2);
    compute_shading(dem, sh3, t3, p3);
    compute_reflectance(rf1, t1, p1);
    compute_reflectance(rf2, t2, p2);
    compute_reflectance(rf3, t3, p3);

    // Build lookup: quantized (r1,r2,r3) -> surface normal
    struct NormalEntry { double nx, ny, nz; };
    std::unordered_map<uint64_t, NormalEntry> lookup;
    lookup.reserve(REFL_N * REFL_N);

    for (int i = 0; i < REFL_N; ++i) {
        for (int j = 0; j < REFL_N; ++j) {
            double r1 = rf1[i * REFL_N + j];
            double r2 = rf2[i * REFL_N + j];
            double r3 = rf3[i * REFL_N + j];
            if (r1 <= 0 || r2 <= 0 || r3 <= 0) continue;
            vec3 n(0, 0, 0);
            if (!index_to_normal(i, j, n)) continue;
            int c1 = (int)(r1 * (QUANT - 1));
            int c2 = (int)(r2 * (QUANT - 1));
            int c3 = (int)(r3 * (QUANT - 1));
            uint64_t key = ((uint64_t)c1 * QUANT + c2) * QUANT + c3;
            lookup[key] = { n.x, n.y, n.z };
        }
    }

    // Resolve each unresolved pixel
    double* nx   = normal + 0 * IMG_N * IMG_N;
    double* ny   = normal + 1 * IMG_N * IMG_N;
    double* nz   = normal + 2 * IMG_N * IMG_N;
    double* nw = normal + 3 * IMG_N * IMG_N;

    for (int i = 0; i < IMG_N; ++i) {
        for (int j = 0; j < IMG_N; ++j) {
            int idx = i * IMG_N + j;
            if (nw[idx] != 0) continue;
            int c1 = (int)(sh1[idx] * (QUANT - 1));
            int c2 = (int)(sh2[idx] * (QUANT - 1));
            int c3 = (int)(sh3[idx] * (QUANT - 1));
            uint64_t key = ((uint64_t)c1 * QUANT + c2) * QUANT + c3;
            auto it = lookup.find(key);
            if (it != lookup.end()) {
                nx[idx]   = it->second.nx;
                ny[idx]   = it->second.ny;
                nz[idx]   = it->second.nz;
                nw[idx] = 1.0;
            }
        }
    }
}

void compute_gradients(const double* normal, double* gx, double* gy) {
    const double* nx   = normal + 0 * IMG_N * IMG_N;
    const double* ny   = normal + 1 * IMG_N * IMG_N;
    const double* nz   = normal + 2 * IMG_N * IMG_N;
    const double* nw = normal + 3 * IMG_N * IMG_N;
    for (int i = 0; i < IMG_N * IMG_N; ++i) {
        gx[i] = (nw[i] == 0 || nz[i] == 0) ? 0.0 : -(nx[i] / nz[i]);
        gy[i] = (nw[i] == 0 || nz[i] == 0) ? 0.0 :   ny[i] / nz[i];
    }
}

void reset_normal(double* normal) {
    std::memset(normal, 0, 4 * IMG_N * IMG_N * sizeof(double));
}
