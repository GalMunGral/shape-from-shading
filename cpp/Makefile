CXX      = em++
CXXFLAGS = -O2 -std=c++17
LDFLAGS  = \
	-s WASM=1 \
	-s EXPORTED_FUNCTIONS='["_get_dem_ptr","_get_shading_ptr","_get_reflectance_ptr","_get_normal_ptr","_get_gx_ptr","_get_gy_ptr","_get_integral_ptr","_get_reconstructed_ptr","_get_amplitudes_ptr","_do_compute_shading","_do_compute_reflectance","_do_resolve_normal","_do_reset_normal","_do_compute_gradients","_do_integrate","_do_frankot_chellappa"]' \
	-s EXPORTED_RUNTIME_METHODS='["HEAPF64"]' \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s MODULARIZE=1 \
	-s EXPORT_NAME=createSolverModule

OUT = ../public/solver.js

all: $(OUT)

$(OUT): main.cpp constants.hpp utils.hpp shading.hpp normal.hpp fft.hpp reconstruct.hpp
	mkdir -p ../public
	$(CXX) $(CXXFLAGS) main.cpp -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT) ../public/solver.wasm