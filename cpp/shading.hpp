#pragma once
#include "constants.hpp"
#include "utils.hpp"

static const double EXAGGERATION = 20.0;

inline vec3 light(double theta, double phi) {
    return unit(
        std::cos(radians(phi)) * std::cos(radians(theta)),
        std::cos(radians(phi)) * std::sin(radians(theta)),
        std::sin(radians(phi))
    );
}

// Returns false if (i,j) is outside the unit sphere in reflectance map space
bool index_to_normal(int i, int j, vec3& n) {
    double x = -2.0 + j * (4.0 / REFL_N);
    double y =  2.0 + i * (-4.0 / REFL_N);
    double r2 = x*x + y*y;
    if (r2 >= 4.0) return false;
    double z = (4.0 - r2) / (r2 + 4.0);
    n = unit(x * z, y * z, z);
    return true;
}

void compute_shading(const double* dem, double* out, double theta, double phi) {
    vec3 l = light(theta, phi);
    for (int i = 0; i < IMG_N; ++i) {
        for (int j = 0; j < IMG_N; ++j) {
            vec3 n = unit(
                EXAGGERATION * (sample(dem, IMG_N, i, j-1) - sample(dem, IMG_N, i, j+1)) / 2.0,
                EXAGGERATION * (sample(dem, IMG_N, i+1, j) - sample(dem, IMG_N, i-1, j)) / 2.0,
                1.0
            );
            out[i * IMG_N + j] = std::max(0.0, dot(l, n));
        }
    }
}

void compute_reflectance(double* out, double theta, double phi) {
    vec3 l = light(theta, phi);
    for (int i = 0; i < REFL_N; ++i) {
        for (int j = 0; j < REFL_N; ++j) {
            vec3 n(0, 0, 0);
            if (!index_to_normal(i, j, n))
                out[i * REFL_N + j] = -1.0;
            else
                out[i * REFL_N + j] = dot(l, n);
        }
    }
}