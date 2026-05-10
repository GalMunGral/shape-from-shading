import { control, button, loadDemData, displayGray, displayNormal } from "./io";
import { N, R } from "./constants";

const clamp = (v: number, lo: number, hi: number) => Math.max(lo, Math.min(hi, v));

interface SolverModule {
  _get_dem_ptr():           number;
  _get_shading_ptr():       number;
  _get_reflectance_ptr():   number;
  _get_normal_ptr():        number;
  _get_integral_ptr():      number;
  _get_reconstructed_ptr(): number;
  _get_amplitudes_ptr():    number;
  _do_compute_shading(theta: number, phi: number): void;
  _do_compute_reflectance(theta: number, phi: number): void;
  _do_resolve_normal(t1: number, p1: number, t2: number, p2: number, t3: number, p3: number): void;
  _do_reset_normal(): void;
  _do_compute_gradients(): void;
  _do_integrate(): void;
  _do_frankot_chellappa(): void;
  HEAPF64: Float64Array;
}

declare const createSolverModule: () => Promise<SolverModule>;

async function main() {
  const M = await createSolverModule();

  const f = (ptr: number, len: number) => M.HEAPF64.subarray(ptr / 8, ptr / 8 + len);
  const demBuf          = f(M._get_dem_ptr(),           N * N);
  const shadingBuf      = f(M._get_shading_ptr(),       N * N);
  const reflectanceBuf  = f(M._get_reflectance_ptr(),   R * R);
  const normalBuf       = f(M._get_normal_ptr(),     4 * N * N);
  const integralBuf     = f(M._get_integral_ptr(),       N * N);
  const reconstructedBuf= f(M._get_reconstructed_ptr(), N * N);
  const amplitudesBuf   = f(M._get_amplitudes_ptr(),    N * N);

  const dem = await loadDemData();
  demBuf.set(dem);
  displayGray("dem", demBuf, N);

  const thetas: number[] = [];
  const phis:   number[] = [];

  function reset() {
    thetas.length = 0;
    phis.length   = 0;
    M._do_reset_normal();
  }

  const theta = control("theta", { initialValue: 45, onInput: captureImage, onChange: updateAll });
  const phi   = control("phi",   { initialValue: 45, onInput: captureImage, onChange: updateAll });

  function captureImage() {
    const t = theta.get(), p = phi.get();
    for (let i = 0; i < thetas.length; ++i) {
      if (thetas[i] === t && phis[i] === p) return;
    }
    M._do_compute_shading(t, p);
    M._do_compute_reflectance(t, p);
    displayGray("shading",     shadingBuf,     N);
    displayGray("reflectance", reflectanceBuf, R);
    if (thetas.length === 3) { thetas.shift(); phis.shift(); }
    thetas.push(t); phis.push(p);
  }

  function updateGradients() {
    if (thetas.length < 3) return;
    M._do_resolve_normal(thetas[0], phis[0], thetas[1], phis[1], thetas[2], phis[2]);
    displayNormal("normal", normalBuf, N);
    M._do_compute_gradients();
    M._do_integrate();
    displayGray("integral", integralBuf, N);
  }

  function fitModel() {
    M._do_frankot_chellappa();
    displayGray("reconstructed", reconstructedBuf, N);
    displayGray("amplitudes",    amplitudesBuf,    N);
  }

  function updateAll() {
    captureImage();
    updateGradients();
    fitModel();
  }

  let rafHandle = -1;
  button("reconstruct", () => {
    cancelAnimationFrame(rafHandle);
    reset();
    let iter     = 360 * 10;
    let interval = 1;
    rafHandle = requestAnimationFrame(function step() {
      if (!iter--) return;
      if (iter % 360 === 0) phi.set(clamp(phi.get() - 1, 10, 80));
      theta.set((theta.get() + 1) % 360);
      captureImage();
      updateGradients();
      if (iter % interval === 0) {
        fitModel();
        interval = clamp(interval * 4, 1, 32);
      }
      rafHandle = requestAnimationFrame(step);
    });
  });

}

main();
