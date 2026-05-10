#pragma once
#include <cmath>
#include <limits>
#include <algorithm>
#include "constants.hpp"
#include "fft.hpp"

static void remap(double* img, int N) {
    double lo =  std::numeric_limits<double>::infinity();
    double hi = -std::numeric_limits<double>::infinity();
    for (int i = 0; i < N * N; ++i) {
        lo = std::min(lo, img[i]);
        hi = std::max(hi, img[i]);
    }
    double range = hi - lo;
    if (range == 0.0) { std::fill(img, img + N * N, 0.0); return; }
    for (int i = 0; i < N * N; ++i) img[i] = (img[i] - lo) / range;
}

void integrate(const double* gx, const double* gy, double* out) {
    const double h = 1.0;
    out[0] = 0.0;
    for (int j = 1; j < IMG_N; ++j)
        out[j] = out[j-1] + gx[j-1] * h;
    for (int i = 1; i < IMG_N; ++i) {
        out[i * IMG_N] = out[(i-1) * IMG_N] - gx[(i-1) * IMG_N] * h;
        for (int j = 1; j < IMG_N; ++j) {
            int idx = i * IMG_N + j;
            double v1 = out[(i-1)*IMG_N+(j-1)] + gx[(i-1)*IMG_N+(j-1)] * h - gy[(i-1)*IMG_N+(j-1)] * h;
            double v2 = out[(i-1)*IMG_N+j]     - gy[(i-1)*IMG_N+j] * h;
            double v3 = out[i*IMG_N+(j-1)]     + gx[i*IMG_N+(j-1)] * h;
            out[idx] = (v1 + v2 + v3) / 3.0;
        }
    }
    remap(out, IMG_N);
}

void frankot_chellappa(const double* gx, const double* gy,
                       double* reconstructed, double* amplitudes) {
    auto Fx = dft2d(gx, IMG_N);
    auto Fy = dft2d(gy, IMG_N);

    std::vector<cd> Z(IMG_N * IMG_N);
    for (int i = 0; i < IMG_N; ++i) {
        for (int j = 0; j < IMG_N; ++j) {
            if (i == 0 && j == 0) continue;
            double d  = i*i + j*j;
            double re = ( Fx[i*IMG_N+j].imag() * j + Fy[i*IMG_N+j].imag() * i) / d;
            double im = -(Fx[i*IMG_N+j].real() * j + Fy[i*IMG_N+j].real() * i) / d;
            Z[i*IMG_N+j] = cd(re, im);
        }
    }

    auto z = idft2d(Z, IMG_N);
    for (int i = 0; i < IMG_N * IMG_N; ++i) reconstructed[i] = z[i].real();
    remap(reconstructed, IMG_N);

    for (int i = 0; i < IMG_N * IMG_N; ++i) {
        amplitudes[i] = std::log(std::abs(Z[i]) + 1.0);
    }
    remap(amplitudes, IMG_N);
}
