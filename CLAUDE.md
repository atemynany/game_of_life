# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

### C++ Engine + Tests
```bash
cmake -B build -DGOL_BUILD_TESTS=ON
cmake --build build -j$(sysctl -n hw.ncpu)
```

### C++ Engine + Pybind11 Bindings
```bash
cmake -B build -DGOL_BUILD_TESTS=ON -DGOL_BUILD_BINDINGS=ON
cmake --build build -j$(sysctl -n hw.ncpu)
```

### Python Development Install
```bash
pip install -e .
```

Requires CMake >= 3.16 and a C++17 compiler. OpenMP is optional (`brew install libomp`).

## Tests

### C++ Tests (Catch2)
```bash
./build/test_grid
./build/test_rle
```

### Python Tests
```bash
PYTHONPATH=build/bindings:python python -m pytest tests/python/ -v
```

## Run

### TUI
```bash
PYTHONPATH=build/bindings:python python -m gol.tui.app
```

### CLI (headless, for AI agents)
```bash
echo -e "new 50 50\nrandomize\nstep 10\npopulation\nquit" | PYTHONPATH=build/bindings:python python -m gol.cli
```

## Architecture

### C++ Engine (`engine/`)
- `gol::Grid` — Runtime-sized grid using `std::vector<uint64_t>` with manual bit ops
  - Row layout: each row is `ceil(width/64)` words. Cell `(x,y)` = bit `x%64` of word `y * words_per_row + x/64`
  - Double-buffered (`data_` + `buffer_`), swap after step
  - Toroidal wrapping via modular arithmetic
  - Optional OpenMP parallelization over rows
  - `population()` uses `__builtin_popcountll`
- `gol::RLEPattern` / `parse_rle` / `to_rle` / `load_rle` — RLE parser/serializer
- `gol::text_to_pattern` — Bitmap font renderer (5x7 hardcoded font, 95 printable ASCII)

### Pybind11 Bindings (`bindings/`)
- Module `gol_engine` wraps Grid, RLE, and text_to_pattern
- `to_numpy()` → `(height, width)` uint8 array
- `to_numpy_packed()` → raw uint64 array (zero-copy for ML)
- GIL released during `step`, `step_n`, `randomize`

### Python (`python/gol/`)
- `cli.py` — stdin/stdout line protocol for AI agents
- `tui/` — Textual TUI: grid widget (unicode half-block), chat panel, controls bar
- `llm/` — LLM integration: tool schemas, Anthropic client, tool executor
- `patterns/` — ConwayLife pattern fetcher with local cache

### Old Code (`old/`)
- Archived original `gol.cpp`, `gol.hpp`, `gol_gui.cpp` (template-based `Gol<X,Y>` with `std::bitset`)

## Key Design Decisions
- `std::vector<uint64_t>` over `std::vector<bool>` — has `data()` pointer for numpy zero-copy interop
- Runtime grid size (not template) — enables Python/CLI to create grids dynamically
- CMake target names: `gol_engine_lib` (C++ static lib) vs `gol_engine` (pybind11 module)
