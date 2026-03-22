"""Headless CLI for Game of Life — stdin/stdout line protocol for AI agents."""

import sys
import json
import base64
import argparse
import struct

try:
    import gol_engine
except ImportError:
    gol_engine = None


class GameCLI:
    def __init__(self, use_json: bool = False):
        self.grid = None
        self.use_json = use_json

    def respond(self, status: str, data=None, message: str = ""):
        if self.use_json:
            resp = {"status": status}
            if data is not None:
                resp["data"] = data
            if message:
                resp["message"] = message
            print(json.dumps(resp), flush=True)
        else:
            if message:
                print(message, flush=True)
            elif data is not None:
                print(data, flush=True)
            else:
                print("OK", flush=True)

    def error(self, msg: str):
        self.respond("error", message=f"ERROR: {msg}")

    def handle(self, line: str) -> bool:
        parts = line.strip().split()
        if not parts:
            return True

        cmd = parts[0].lower()

        if cmd == "quit":
            return False

        if gol_engine is None:
            self.error("gol_engine not available — build with pybind11")
            return True

        try:
            if cmd == "new":
                w = int(parts[1]) if len(parts) > 1 else 100
                h = int(parts[2]) if len(parts) > 2 else 100
                self.grid = gol_engine.Grid(w, h)
                self.respond("ok")

            elif cmd == "place":
                if not self.grid:
                    self.error("no grid — use 'new' first")
                    return True
                rle = parts[1] if len(parts) > 1 else ""
                x = int(parts[2]) if len(parts) > 2 else 0
                y = int(parts[3]) if len(parts) > 3 else 0
                gol_engine.load_rle(self.grid, rle, x, y)
                self.respond("ok")

            elif cmd == "step":
                if not self.grid:
                    self.error("no grid")
                    return True
                n = int(parts[1]) if len(parts) > 1 else 1
                self.grid.step_n(n)
                msg = f"OK gen={self.grid.generation} pop={self.grid.population}"
                self.respond("ok", {"gen": self.grid.generation, "pop": self.grid.population}, msg)

            elif cmd == "state":
                if not self.grid:
                    self.error("no grid")
                    return True
                packed = self.grid.to_numpy_packed()
                b64 = base64.b64encode(packed.tobytes()).decode()
                self.respond("ok", b64, b64)

            elif cmd == "state_ascii":
                if not self.grid:
                    self.error("no grid")
                    return True
                ascii_str = self.grid.to_ascii()
                self.respond("ok", ascii_str, ascii_str)

            elif cmd == "state_region":
                if not self.grid:
                    self.error("no grid")
                    return True
                x, y = int(parts[1]), int(parts[2])
                w, h = int(parts[3]), int(parts[4])
                region = self.grid.to_ascii_region(x, y, w, h)
                self.respond("ok", region, region)

            elif cmd == "text":
                if not self.grid:
                    self.error("no grid")
                    return True
                # text <string> <x> <y>
                text = parts[1] if len(parts) > 1 else ""
                x = int(parts[2]) if len(parts) > 2 else 0
                y = int(parts[3]) if len(parts) > 3 else 0
                pattern = gol_engine.text_to_pattern(text)
                self.grid.paste(pattern, x, y)
                self.respond("ok")

            elif cmd == "population":
                if not self.grid:
                    self.error("no grid")
                    return True
                pop = self.grid.population
                self.respond("ok", pop, str(pop))

            elif cmd == "randomize":
                if not self.grid:
                    self.error("no grid")
                    return True
                density = float(parts[1]) if len(parts) > 1 else 0.1
                self.grid.randomize(density)
                self.respond("ok")

            elif cmd == "reset":
                if not self.grid:
                    self.error("no grid")
                    return True
                self.grid.clear()
                self.respond("ok")

            else:
                self.error(f"unknown command: {cmd}")

        except Exception as e:
            self.error(str(e))

        return True

    def run(self):
        for line in sys.stdin:
            if not self.handle(line):
                break


def main():
    parser = argparse.ArgumentParser(description="Game of Life CLI")
    parser.add_argument("--json", action="store_true", help="JSON output mode")
    args = parser.parse_args()

    cli = GameCLI(use_json=args.json)
    cli.run()


if __name__ == "__main__":
    main()
