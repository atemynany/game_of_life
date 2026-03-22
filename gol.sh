#!/bin/zsh
# Launcher for Game of Life TUI
GOL_DIR="$HOME/Desktop/workspace/game of life/game_of_life"
PYTHONPATH="$GOL_DIR/build/bindings:$GOL_DIR/python" python3 -m gol.tui.app "$@"
