"""Tests for the gol_engine Python bindings."""

import sys
import os
import pytest

# Add build directory to path for development
build_path = os.path.join(os.path.dirname(__file__), "..", "..", "build", "bindings")
if os.path.exists(build_path):
    sys.path.insert(0, build_path)

import gol_engine


def test_grid_creation():
    g = gol_engine.Grid(50, 30)
    assert g.width == 50
    assert g.height == 30
    assert g.population == 0


def test_step():
    g = gol_engine.Grid(10, 10)
    # Blinker
    g.set_cell(3, 4, True)
    g.set_cell(4, 4, True)
    g.set_cell(5, 4, True)

    g.step()
    assert g.generation == 1
    assert g.get_cell(4, 3) == True
    assert g.get_cell(4, 4) == True
    assert g.get_cell(4, 5) == True


def test_numpy():
    g = gol_engine.Grid(10, 10)
    g.set_cell(0, 0, True)
    arr = g.to_numpy()
    assert arr.shape == (10, 10)
    assert arr[0, 0] == 1
    assert arr[1, 1] == 0


def test_numpy_packed():
    g = gol_engine.Grid(100, 100)
    g.randomize(0.5, 42)
    packed = g.to_numpy_packed()
    assert packed.dtype.name == "uint64"


def test_rle():
    g = gol_engine.Grid(20, 20)
    gol_engine.load_rle(g, "x = 3, y = 1\n3o!", 5, 5)
    assert g.get_cell(5, 5) == True
    assert g.get_cell(6, 5) == True
    assert g.get_cell(7, 5) == True


def test_text_to_pattern():
    p = gol_engine.text_to_pattern("A")
    assert p.width == 5
    assert p.height == 7
    assert p.population > 0


def test_to_rle():
    g = gol_engine.Grid(5, 5)
    g.set_cell(0, 0, True)
    g.set_cell(1, 0, True)
    rle = gol_engine.to_rle(g)
    assert "!" in rle


def test_ascii():
    g = gol_engine.Grid(3, 3)
    g.set_cell(1, 1, True)
    ascii_str = g.to_ascii()
    lines = ascii_str.split("\n")
    assert len(lines) == 3
    assert lines[1] == ".#."


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
