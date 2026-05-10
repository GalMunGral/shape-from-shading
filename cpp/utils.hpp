#pragma once
#include <cmath>
#include <algorithm>

struct vec3 {
    double x, y, z;

    vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    vec3 operator+(const vec3& o) const { return vec3(x+o.x, y+o.y, z+o.z); }
    vec3 operator-(const vec3& o) const { return vec3(x-o.x, y-o.y, z-o.z); }
    vec3 operator*(double s)      const { return vec3(x*s,   y*s,   z*s);   }
    vec3 operator/(double s)      const { return vec3(x/s,   y/s,   z/s);   }
};

inline double dot(vec3 a, vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline vec3 unit(double x, double y, double z) {
    vec3 v(x, y, z);
    return v / std::sqrt(dot(v, v));
}

inline double clamp(double v, double lo, double hi) {
    return std::max(lo, std::min(hi, v));
}

inline double radians(double deg) {
    return deg * M_PI / 180.0;
}

inline double sample(const double* img, int N, int i, int j) {
    i = (int)clamp((double)i, 0.0, (double)(N - 1));
    j = (int)clamp((double)j, 0.0, (double)(N - 1));
    return img[i * N + j];
}
