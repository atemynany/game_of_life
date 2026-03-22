"""Tests for the CLI protocol."""

import sys
import os

# Add paths
build_path = os.path.join(os.path.dirname(__file__), "..", "..", "build", "bindings")
if os.path.exists(build_path):
    sys.path.insert(0, build_path)
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "python"))

from gol.cli import GameCLI


def test_new_grid():
    cli = GameCLI(use_json=True)
    assert cli.handle("new 50 50") == True
    assert cli.grid is not None
    assert cli.grid.width == 50


def test_step():
    cli = GameCLI(use_json=True)
    cli.handle("new 10 10")
    assert cli.handle("step 5") == True
    assert cli.grid.generation == 5


def test_randomize():
    cli = GameCLI(use_json=True)
    cli.handle("new 50 50")
    cli.handle("randomize 0.3")
    assert cli.grid.population > 0


def test_population():
    cli = GameCLI(use_json=True)
    cli.handle("new 10 10")
    cli.handle("randomize 0.5")
    assert cli.handle("population") == True


def test_reset():
    cli = GameCLI(use_json=True)
    cli.handle("new 10 10")
    cli.handle("randomize")
    cli.handle("reset")
    assert cli.grid.population == 0


def test_quit():
    cli = GameCLI()
    assert cli.handle("quit") == False


if __name__ == "__main__":
    import pytest
    pytest.main([__file__, "-v"])
