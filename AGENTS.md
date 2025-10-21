# Repository Guidelines

## Project Structure & Module Organization
StudioSlab is a CMake-driven C++23 workspace. Shared libraries live in `Lib/`, grouped by domain modules and exported as the `Slab` static library consumed by studio executables under `Studios/`. Assets sit under `Resources/`, notebooks in `Notebooks/`, scripts in `Scripts/`, and generated outputs land in `Build/bin` and `Build/lib`; keep throwaway `cmake-build-*` directories out of version control.

## Build, Test, and Development Commands
Run `Scripts/install-deps.sh` (Ubuntu) before configuring the project. Configure with `cmake -S . -B cmake-build-debug -DSTUDIOSLAB_CUDA_SUPPORT=ON` and rebuild via `cmake --build cmake-build-debug --target Slab`. Studio binaries appear in `Build/bin` (for example `./Build/bin/Fields-RtoR`), while containers use `docker build -t studioslab:latest .` followed by `docker run -it --gpus all studioslab:latest`.

## Coding Style & Naming Conventions
Adopt the existing 4-space indentation with Allman braces for namespaces but K&R braces for functions and methods. Keep PascalCase for classes and callable names, and retain purposeful prefixes (`FApplication`, `p_Platform`). Keep headers beside sources in `Lib/`, prefer expressive aliases (`Real2D`), and mirror local patterns for templates and logging. Run `clang-format` (config in `.clang-format`) before committing and enable the root `.clang-tidy` so CLion enforces PascalCase; Python helpers inside `Lib/Python` stay snake_case.

## Testing Guidelines
Tests live in `Lib/tests` using Catch2 v3. Name files `test_<topic>.cpp`, tag cases (`[Str]`, `[Real2D]`), and cover both success paths and guard-rails. Build with `cmake --build cmake-build-debug --target testsuite` then execute `ctest --test-dir cmake-build-debug --output-on-failure`; include outputs in reviews when fixing bugs.

## Commit & Pull Request Guidelines
Commits are short, declarative summaries (see recent “Tamed molecules”). Keep the subject ≤60 characters, follow with a blank line and optional context. Pull requests should include: issue links, a crisp change overview, required build toggles (CUDA, SFML, OpenMP), and evidence that tests or key studios ran (`ctest`, screenshots, or CLI logs).

## GPU & Configuration Notes
Most studios expect CUDA and graphics drivers; disable with `-DSTUDIOSLAB_CUDA_SUPPORT=OFF` or build via `no-gpu.Dockerfile` when working on CPU-only hosts. Keep platform-specific caches out of commits but mention driver assumptions in PRs.
