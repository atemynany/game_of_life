"""Tool definitions for LLM function calling."""

TOOLS = [
    {
        "name": "place_pattern",
        "description": "Place an RLE-encoded Game of Life pattern on the grid at the given position.",
        "input_schema": {
            "type": "object",
            "properties": {
                "rle": {"type": "string", "description": "RLE-encoded pattern string"},
                "x": {"type": "integer", "description": "X offset on grid"},
                "y": {"type": "integer", "description": "Y offset on grid"},
            },
            "required": ["rle", "x", "y"],
        },
    },
    {
        "name": "step",
        "description": "Advance the simulation by N generations.",
        "input_schema": {
            "type": "object",
            "properties": {
                "n": {"type": "integer", "description": "Number of steps (default 1)", "default": 1},
            },
        },
    },
    {
        "name": "get_state",
        "description": "Get the current grid state as ASCII art (. for dead, # for alive).",
        "input_schema": {
            "type": "object",
            "properties": {},
        },
    },
    {
        "name": "place_text",
        "description": "Render text as alive cells using a bitmap font and place on grid.",
        "input_schema": {
            "type": "object",
            "properties": {
                "text": {"type": "string", "description": "Text to render"},
                "x": {"type": "integer", "description": "X offset"},
                "y": {"type": "integer", "description": "Y offset"},
            },
            "required": ["text", "x", "y"],
        },
    },
    {
        "name": "fetch_pattern",
        "description": "Fetch a named pattern from the ConwayLife pattern library.",
        "input_schema": {
            "type": "object",
            "properties": {
                "name": {"type": "string", "description": "Pattern name (e.g. 'glider', 'gosperglidergun')"},
            },
            "required": ["name"],
        },
    },
    {
        "name": "new_grid",
        "description": "Create a new empty grid with the given dimensions.",
        "input_schema": {
            "type": "object",
            "properties": {
                "width": {"type": "integer", "description": "Grid width"},
                "height": {"type": "integer", "description": "Grid height"},
            },
            "required": ["width", "height"],
        },
    },
    {
        "name": "randomize",
        "description": "Fill the grid randomly with the given density (0.0 to 1.0).",
        "input_schema": {
            "type": "object",
            "properties": {
                "density": {"type": "number", "description": "Fraction of cells alive (default 0.1)", "default": 0.1},
            },
        },
    },
]

# Convert to Anthropic tool format
ANTHROPIC_TOOLS = TOOLS

# Convert to Gemini/OpenAI function format
OPENAI_TOOLS = [
    {
        "type": "function",
        "function": {
            "name": t["name"],
            "description": t["description"],
            "parameters": t["input_schema"],
        },
    }
    for t in TOOLS
]
