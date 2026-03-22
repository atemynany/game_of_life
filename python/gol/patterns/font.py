"""Re-export text_to_pattern from the C++ engine."""

try:
    from gol_engine import text_to_pattern
except ImportError:
    def text_to_pattern(text: str, char_spacing: int = 1):
        raise RuntimeError("gol_engine not available — build with pybind11")
