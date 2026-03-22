"""Grid widget — renders Game of Life using unicode half-block characters."""

from textual.widget import Widget
from textual.reactive import reactive
from textual.strip import Strip
from rich.segment import Segment
from rich.style import Style

try:
    import gol_engine
except ImportError:
    gol_engine = None

ALIVE_STYLE = Style(color="green", bgcolor="black")
DEAD_STYLE = Style(color="black", bgcolor="black")


class GridWidget(Widget):
    """Renders the GoL grid using half-block unicode chars.

    Each terminal row displays two grid rows, doubling vertical resolution.
    """

    DEFAULT_CSS = """
    GridWidget {
        width: 1fr;
        height: 1fr;
        background: black;
    }
    """

    viewport_x: reactive[int] = reactive(0)
    viewport_y: reactive[int] = reactive(0)

    def __init__(self, grid=None, **kwargs):
        super().__init__(**kwargs)
        self.grid = grid

    def set_grid(self, grid):
        self.grid = grid
        self.viewport_x = 0
        self.viewport_y = 0

    def _build_line(self, y: int) -> str:
        """Build a string for terminal line y (representing 2 grid rows)."""
        if self.grid is None:
            return ""

        term_width = self.size.width
        grid_y_top = self.viewport_y + y * 2
        grid_y_bot = grid_y_top + 1

        chars = []
        for tx in range(term_width):
            gx = self.viewport_x + tx
            if gx >= self.grid.width:
                chars.append(" ")
                continue

            top = self.grid.get_cell(gx, grid_y_top) if grid_y_top < self.grid.height else False
            bot = self.grid.get_cell(gx, grid_y_bot) if grid_y_bot < self.grid.height else False

            if top and bot:
                chars.append("\u2588")  # █
            elif top:
                chars.append("\u2580")  # ▀
            elif bot:
                chars.append("\u2584")  # ▄
            else:
                chars.append(" ")

        return "".join(chars)

    def render_line(self, y: int) -> Strip:
        """Called by Textual to render each line."""
        if self.grid is None:
            return Strip([Segment(" " * self.size.width, DEAD_STYLE)])

        line_text = self._build_line(y)
        return Strip([Segment(line_text, ALIVE_STYLE)])

    def scroll_viewport(self, dx: int, dy: int):
        if self.grid is None:
            return
        self.viewport_x = max(0, min(self.viewport_x + dx, max(0, self.grid.width - self.size.width)))
        self.viewport_y = max(0, min(self.viewport_y + dy, max(0, self.grid.height - self.size.height * 2)))
        self.refresh()
