#include <emscripten/emscripten.h>
#include "constants.hpp"
#include "shading.hpp"
#include "normal.hpp"
#include "reconstruct.hpp"

static double dem         [IMG_N  * IMG_N ];
static double shading     [IMG_N  * IMG_N ];
static double reflectance [REFL_N * REFL_N];
static double normal      [4 * IMG_N * IMG_N];
static double gx          [IMG_N  * IMG_N ];
static double gy          [IMG_N  * IMG_N ];
static double integral    [IMG_N  * IMG_N ];
static double reconstructed[IMG_N * IMG_N ];
static double amplitudes  [IMG_N  * IMG_N ];

extern "C" {

EMSCRIPTEN_KEEPALIVE double* get_dem_ptr()          { return dem;          }
EMSCRIPTEN_KEEPALIVE double* get_shading_ptr()      { return shading;      }
EMSCRIPTEN_KEEPALIVE double* get_reflectance_ptr()  { return reflectance;  }
EMSCRIPTEN_KEEPALIVE double* get_normal_ptr()       { return normal;       }
EMSCRIPTEN_KEEPALIVE double* get_gx_ptr()           { return gx;           }
EMSCRIPTEN_KEEPALIVE double* get_gy_ptr()           { return gy;           }
EMSCRIPTEN_KEEPALIVE double* get_integral_ptr()     { return integral;     }
EMSCRIPTEN_KEEPALIVE double* get_reconstructed_ptr(){ return reconstructed; }
EMSCRIPTEN_KEEPALIVE double* get_amplitudes_ptr()   { return amplitudes;   }

EMSCRIPTEN_KEEPALIVE void do_compute_shading(double theta, double phi) {
    compute_shading(dem, shading, theta, phi);
}

EMSCRIPTEN_KEEPALIVE void do_compute_reflectance(double theta, double phi) {
    compute_reflectance(reflectance, theta, phi);
}

EMSCRIPTEN_KEEPALIVE void do_resolve_normal(double t1, double p1,
                                             double t2, double p2,
                                             double t3, double p3) {
    resolve_normal(dem, normal, t1, p1, t2, p2, t3, p3);
}

EMSCRIPTEN_KEEPALIVE void do_reset_normal() {
    reset_normal(normal);
}

EMSCRIPTEN_KEEPALIVE void do_compute_gradients() {
    compute_gradients(normal, gx, gy);
}

EMSCRIPTEN_KEEPALIVE void do_integrate() {
    integrate(gx, gy, integral);
}

EMSCRIPTEN_KEEPALIVE void do_frankot_chellappa() {
    frankot_chellappa(gx, gy, reconstructed, amplitudes);
}

} // extern "C"