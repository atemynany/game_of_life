"""LLM client supporting Claude (Anthropic) with tool use."""

from .config import get_config
from .tools import ANTHROPIC_TOOLS

try:
    import gol_engine
except ImportError:
    gol_engine = None


class ToolExecutor:
    """Dispatches tool calls to Grid operations."""

    def __init__(self, grid):
        self.grid = grid
        self._pattern_library = None

    def execute(self, name: str, args: dict) -> str:
        if name == "place_pattern":
            gol_engine.load_rle(self.grid, args["rle"], args["x"], args["y"])
            return f"Placed pattern at ({args['x']}, {args['y']})"

        elif name == "step":
            n = args.get("n", 1)
            self.grid.step_n(n)
            return f"Advanced {n} steps. Gen={self.grid.generation}, Pop={self.grid.population}"

        elif name == "get_state":
            w = min(self.grid.width, 80)
            h = min(self.grid.height, 40)
            return self.grid.to_ascii_region(0, 0, w, h)

        elif name == "place_text":
            pattern = gol_engine.text_to_pattern(args["text"])
            self.grid.paste(pattern, args["x"], args["y"])
            return f"Placed text '{args['text']}' at ({args['x']}, {args['y']})"

        elif name == "fetch_pattern":
            return self._fetch_pattern(args["name"])

        elif name == "new_grid":
            self.grid = gol_engine.Grid(args["width"], args["height"])
            return f"Created {args['width']}x{args['height']} grid"

        elif name == "randomize":
            density = args.get("density", 0.1)
            self.grid.randomize(density)
            return f"Randomized with density {density}"

        return f"Unknown tool: {name}"

    def _fetch_pattern(self, name: str) -> str:
        try:
            from ..patterns.library import fetch_pattern
            rle = fetch_pattern(name)
            if rle:
                return f"Fetched pattern '{name}':\n{rle}"
            return f"Pattern '{name}' not found"
        except Exception as e:
            return f"Error fetching pattern: {e}"


SYSTEM_PROMPT = """You are a Game of Life assistant. You can control a Conway's Game of Life simulation.
You have tools to place patterns (using RLE notation), advance the simulation, view the grid state,
render text as cell patterns, and fetch patterns from the ConwayLife library.

Common RLE patterns:
- Glider: bo$2bo$3o!
- Block: 2o$2o!
- Blinker: 3o!
- Gosper Glider Gun: 24bo$22bobo$12b2o6b2o12b2o$11bo3bo4b2o12b2o$2o8bo5bo3b2o$2o8bo3bob2o4bobo$10bo5bo7bo$11bo3bo$12b2o!
- Lightweight spaceship (LWSS): bo2bo$o4b$o3bo$4ob!

Help the user discover, create, and explore patterns. Be concise."""


class LLMClient:
    """LLM client with tool use for controlling the Game of Life."""

    def __init__(self, grid):
        self.grid = grid
        self.executor = ToolExecutor(grid)
        self.config = get_config()
        self.provider = None
        self.available = False
        self._messages = []
        self._client = None

        if self.config["anthropic_api_key"]:
            try:
                import anthropic
                self._client = anthropic.Anthropic(api_key=self.config["anthropic_api_key"])
                self.provider = "Claude"
                self.available = True
            except ImportError:
                pass

    def chat(self, message: str) -> str:
        """Send a message and handle tool calls. Returns the final text response."""
        if not self.available:
            return "No AI provider configured."

        self._messages.append({"role": "user", "content": message})

        # Conversation loop with tool use
        max_turns = 10
        for _ in range(max_turns):
            response = self._client.messages.create(
                model="claude-sonnet-4-20250514",
                max_tokens=1024,
                system=SYSTEM_PROMPT,
                tools=ANTHROPIC_TOOLS,
                messages=self._messages,
            )

            self._messages.append({"role": "assistant", "content": response.content})

            # Check if there are tool calls
            tool_uses = [b for b in response.content if b.type == "tool_use"]
            if not tool_uses:
                # Extract text response
                text_blocks = [b.text for b in response.content if b.type == "text"]
                return "\n".join(text_blocks) if text_blocks else ""

            # Execute tools and send results back
            tool_results = []
            for tool_use in tool_uses:
                result = self.executor.execute(tool_use.name, tool_use.input)
                tool_results.append({
                    "type": "tool_result",
                    "tool_use_id": tool_use.id,
                    "content": result,
                })

            self._messages.append({"role": "user", "content": tool_results})

            # Update grid reference if new_grid was called
            self.grid = self.executor.grid

        return "Max tool-use turns reached."
