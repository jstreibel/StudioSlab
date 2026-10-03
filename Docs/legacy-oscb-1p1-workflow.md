# Legacy 1+1 perturbed-oscillon OSCB workflow

This is the shortest supported path for running the legacy perturbed oscillon, storing the recorded field history, reopening it in `osc-viewer`, and loading it in Python.

The binary OSCB records both phase-space channels at every retained output instant:

- `phi(t, x)`
- `dphidt(t, x)`

“Full history” here means every snapshot selected by the history output cadence. It does not include the internal Runge-Kutta sub-stages.

## Build

A CPU-only build is sufficient:

```bash
cmake -S . -B cmake-build-debug-no-gpu -DSTUDIOSLAB_CUDA_SUPPORT=OFF
cmake --build cmake-build-debug-no-gpu --target Fields-RtoR osc-viewer testsuite -j
```

Executables are written to `Build/bin`.

## Run a perturbed 1+1 simulation

Run from the directory where the OSCB file should be created:

```bash
mkdir -p /tmp/studioslab-perturbed-run
cd /tmp/studioslab-perturbed-run

/home/joao/Developer/StudioSlab/Build/bin/Fields-RtoR \
    --sim=1 \
    -N 8192 \
    -L 100 \
    -t 100 \
    --outn 2048 \
    -l 1 \
    -a 0.9 \
    --r_dt 0.1
```

Relevant options:

- `--sim=1`: perturbed oscillon in 1+1 dimensions.
- `-N`, `-L`, `-t`: lattice sites, spatial length, and final time.
- `--r_dt`: integration step factor, with `dt = r_dt * L / N`.
- `--outn`: stored spatial resolution and the basis of the legacy output cadence.
- `-l`: initial-condition scale.
- `-a`: perturbation; `a=1` is unperturbed.

The program prints the output name. Unless `--no_history_to_file` is supplied, it creates a binary `.oscb` file in the current working directory.

Choose `N`, `t`, and `outn` with storage in mind. For fp32 output, the payload size is approximately

```text
outresT * (1 timestamp + 2 * outn field values) * 4 bytes
```

plus the 2048-byte header.

## Open in osc-viewer

```bash
/home/joao/Developer/StudioSlab/Build/bin/osc-viewer \
    --filename "/tmp/studioslab-perturbed-run/<generated-file>.oscb"
```

For new two-channel files the viewer uses stored `dphidt` exactly. For older one-channel OSCB files it retains the previous numerical time-derivative fallback.

## Load and analyze in Python

```bash
export PYTHONPATH=/home/joao/Developer/StudioSlab/Studios/Fields/Tools:/home/joao/Developer/StudioSlab/Lib/Python

python3 - <<'PY'
import numpy as np
from DataAnalysis.SimData import SimData

sim = SimData("/tmp/studioslab-perturbed-run/<generated-file>.oscb")

phi = sim.Phi
dphidt = sim.dPhidt
steps = sim.TimeStamps
x = sim.getXDiscrete()

# Spatial Fourier amplitudes for every stored time.
k = 2.0 * np.pi * np.fft.rfftfreq(phi.shape[1], d=sim["L"] / phi.shape[1])
phi_k = np.fft.rfft(phi, axis=1)

print("phi:", phi.shape)
print("dphidt:", dphidt.shape)
print("stored steps:", steps.shape)
print("spectrum:", phi_k.shape, "k:", k.shape)
PY
```

`sim.Phi` and `sim.dPhidt` have shape `(outresT, outresX)`. `sim["phi"]` is an alias for `sim.Phi`; `sim.Data` exposes the raw stored channel matrix. Old one-channel files still load, with `dPhidt` reconstructed numerically for compatibility.
