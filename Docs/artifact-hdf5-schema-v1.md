# StudioSlab HDF5 Artifact Schema V1

## Scope

This is the first portable artifact contract. Each `.h5` file contains exactly one completed numeric run and is intended to be directly readable with standard HDF5 tools, Python `h5py`, Julia, MATLAB, and similar scientific software.

The first producer is the Model V2 explicit-first-order ODE path. The file stores sampled scalar state and observable series plus the runtime seed needed to understand the run. It does not serialize executable model objects, expression trees, listeners, tasks, or UI state.

## Versioning

The root group carries:

| Attribute | Type | V1 value |
| --- | --- | --- |
| `studioslab_format` | UTF-8 string | `studioslab.artifact-run` |
| `schema_major` | little-endian `uint32` | `1` |
| `schema_minor` | little-endian `uint32` | `0` |
| `producer` | UTF-8 string | `StudioSlab` |

Readers must reject an unknown major version. A newer minor version is compatible: V1 readers load the required V1 fields and may ignore additions.

## File Tree

```text
/
├── run
│   └── runtime
│       ├── scalar_bindings
│       │   ├── count                         (attribute)
│       │   ├── 000000
│       │   └── ...
│       └── initial_state
│           ├── count                         (attribute)
│           ├── 000000
│           └── ...
└── artifacts
    ├── state
    │   ├── count                             (attribute)
    │   ├── 000000
    │   └── ...
    └── observable
        ├── count                             (attribute)
        ├── 000000
        └── ...
```

Indexed child names are zero-padded six-digit decimal numbers. `count` is a `uint64` attribute.

## Run Metadata

`/run` attributes:

- UTF-8 strings: `run_id`, `model_id`, `model_name`, `task_name`, `status`
- signed `int64` Unix nanoseconds: `created_utc_unix_ns`, `started_utc_unix_ns`, `finished_utc_unix_ns`, `exported_utc_unix_ns`

`status` is one of `success`, `error`, or `aborted`. Only terminal runs may be exported.

## Runtime Provenance Seed

`/run/runtime` attributes:

| Attribute | Type | Meaning |
| --- | --- | --- |
| `runtime_kind` | UTF-8 string | Current value: `ode.explicit_first_order` |
| `solver_id` | UTF-8 string | Current value: `rk4` |
| `scalar_precision_bits` | `uint32` | Producer scalar width, 32 or 64 |
| `time_step` | `float64` | Integrator step |
| `run_mode` | UTF-8 string | `finite_steps`, `finite_simulation_time`, or `open_ended` |
| `has_max_steps` | `uint8` boolean | Whether `max_steps` exists |
| `max_steps` | optional `uint64` | Finite step limit |
| `has_max_simulation_time` | `uint8` boolean | Whether `max_simulation_time` exists |
| `max_simulation_time` | optional `float64` | Finite simulation-time limit |
| `time_coordinate_definition_id` | UTF-8 string | Model definition used as time |
| `initial_time` | `float64` | Initial simulation time |
| `capture_state_history` | `uint8` boolean | State-series capture requested |
| `capture_observable_history` | `uint8` boolean | Observable-series capture requested |
| `artifact_sample_interval_steps` | `uint64` | Scheduled capture cadence |
| `has_max_artifact_samples` | `uint8` boolean | Whether `max_artifact_samples` exists |
| `max_artifact_samples` | optional `uint64` | Per-listener retained-sample limit |

Each indexed `scalar_bindings` and `initial_state` child has a UTF-8 `definition_id` attribute and a `float64` `value` attribute. This metadata is a reproducibility seed, not a guarantee that the original executable model can be reconstructed.

## Scalar Time Series

Each indexed group under `/artifacts/state` or `/artifacts/observable` has these UTF-8 attributes:

- `artifact_type` = `scalar_time_series`
- `role` = `state` or `observable`, matching the parent group
- `definition_id`
- `display_label`
- `canonical_notation`

It contains equal-length, rank-one datasets:

| Dataset | File type | Meaning |
| --- | --- | --- |
| `step` | little-endian `uint64` | Simulation step |
| `simulation_time` | little-endian IEEE `float64` | Simulation time or placeholder zero |
| `simulation_time_valid` | `uint8` | `1` when the corresponding time is present |
| `wall_clock_seconds` | little-endian IEEE `float64` | Elapsed task wall time |
| `value` | little-endian IEEE `float64` | Scalar sample; NaN and infinity are preserved |
| `event_reason` | `uint8` | Publication reason code |
| `published_version` | little-endian `uint64` | Session publication version |

Event reason codes are stable:

| Code | Meaning |
| --- | --- |
| `0` | initial |
| `1` | scheduled |
| `2` | forced |
| `3` | final |
| `4` | abort final |

Empty datasets are valid. Series of at least 1024 samples are chunked and shuffled; DEFLATE level 4 is used when the installed HDF5 library exposes an encoder.

## Write Semantics

- Export never overwrites an existing destination.
- The writer creates a sibling temporary file, flushes and closes it, then renames it to the destination.
- Failed writes remove the temporary file when possible.
- Loading validates the format marker, schema, required objects, column lengths, enums, booleans, and the in-memory run invariants.

## Minimal `h5py` Read

```python
import h5py

with h5py.File("run.h5", "r") as file:
    run_id = file["run"].attrs["run_id"]
    state = file["artifacts/state/000000"]
    step = state["step"][:]
    value = state["value"][:]
    valid = state["simulation_time_valid"][:].astype(bool)
    simulation_time = state["simulation_time"][:]
```
