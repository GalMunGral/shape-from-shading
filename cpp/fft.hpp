#pragma once
#include <complex>
#include <vector>
#include <cmath>

using cd = std::complex<double>;

static std::vector<cd> fft1d(std::vector<cd> f) {
    int n = f.size();
    if (n == 1) return f;
    std::vector<cd> e(n / 2), o(n / 2);
    for (int i = 0; i < n / 2; ++i) { e[i] = f[2 * i]; o[i] = f[2 * i + 1]; }
    auto E = fft1d(e), O = fft1d(o);
    std::vector<cd> F(n);
    for (int i = 0; i < n / 2; ++i) {
        cd w = std::polar(1.0, -2.0 * M_PI * i / n);
        F[i]       = E[i] + w * O[i];
        F[i + n/2] = E[i] - w * O[i];
    }
    return F;
}

static std::vector<cd> ifft1d(std::vector<cd> f) {
    int n = f.size();
    if (n == 1) return f;
    std::vector<cd> e(n / 2), o(n / 2);
    for (int i = 0; i < n / 2; ++i) { e[i] = f[2 * i]; o[i] = f[2 * i + 1]; }
    auto E = ifft1d(e), O = ifft1d(o);
    std::vector<cd> F(n);
    for (int i = 0; i < n / 2; ++i) {
        cd w = std::polar(1.0, 2.0 * M_PI * i / n);
        F[i]       = 0.5 * (E[i] + w * O[i]);
        F[i + n/2] = 0.5 * (E[i] - w * O[i]);
    }
    return F;
}

// 2D FFT on N×N row-major array
std::vector<cd> dft2d(const double* f, int N) {
    std::vector<cd> F(N * N);
    for (int i = 0; i < N; ++i) {
        std::vector<cd> row(N);
        for (int j = 0; j < N; ++j) row[j] = f[i * N + j];
        auto R = fft1d(row);
        for (int j = 0; j < N; ++j) F[i * N + j] = R[j];
    }
    for (int j = 0; j < N; ++j) {
        std::vector<cd> col(N);
        for (int i = 0; i < N; ++i) col[i] = F[i * N + j];
        auto C = fft1d(col);
        for (int i = 0; i < N; ++i) F[i * N + j] = C[i];
    }
    return F;
}

std::vector<cd> idft2d(const std::vector<cd>& F, int N) {
    std::vector<cd> f(N * N);
    for (int i = 0; i < N; ++i) {
        std::vector<cd> row(N);
        for (int j = 0; j < N; ++j) row[j] = F[i * N + j];
        auto R = ifft1d(row);
        for (int j = 0; j < N; ++j) f[i * N + j] = R[j];
    }
    for (int j = 0; j < N; ++j) {
        std::vector<cd> col(N);
        for (int i = 0; i < N; ++i) col[i] = f[i * N + j];
        auto C = ifft1d(col);
        for (int i = 0; i < N; ++i) f[i * N + j] = C[i];
    }
    return f;
}
