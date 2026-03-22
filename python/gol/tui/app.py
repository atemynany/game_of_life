"""Textual TUI application for Game of Life."""

from textual.app import App, ComposeResult
from textual.containers import Horizontal
from textual.binding import Binding
from textual import work

from .grid_widget import GridWidget
from .chat_widget import ChatPanel, ChatLog, ChatInput
from .controls_widget import ControlsBar

try:
    import gol_engine
except ImportError:
    gol_engine = None


class GameOfLifeApp(App):
    """Game of Life TUI with grid visualization and LLM chat."""

    TITLE = "Game of Life"

    CSS = """
    Screen {
        layout: vertical;
    }
    #main-area {
        height: 1fr;
    }
    """

    BINDINGS = [
        Binding("p", "toggle_pause", "Pause/Resume"),
        Binding("s", "single_step", "Step", priority=True),
        Binding("equal,plus", "speed_up", "+Speed"),
        Binding("minus", "speed_down", "-Speed"),
        Binding("r", "reset", "Reset"),
        Binding("n", "new_grid", "New Grid"),
        Binding("q", "quit", "Quit"),
        Binding("left", "scroll_left", "Scroll Left", show=False),
        Binding("right", "scroll_right", "Scroll Right", show=False),
        Binding("up", "scroll_up", "Scroll Up", show=False),
        Binding("down", "scroll_down", "Scroll Down", show=False),
    ]

    def __init__(self, width: int = 200, height: int = 100, **kwargs):
        super().__init__(**kwargs)
        self._grid_width = width
        self._grid_height = height
        self._paused = False
        self._speed = 10.0  # steps per second
        self._timer = None
        self._grid = None
        self._llm_client = None

    def compose(self) -> ComposeResult:
        with Horizontal(id="main-area"):
            yield GridWidget(id="grid-widget")
            yield ChatPanel(id="chat-panel")
        yield ControlsBar(id="controls")

    def on_mount(self):
        if gol_engine is None:
            self.notify("gol_engine not available — install with pip install -e .", severity="error")
            return

        self._grid = gol_engine.Grid(self._grid_width, self._grid_height)
        self._grid.randomize(0.1)

        grid_widget = self.query_one("#grid-widget", GridWidget)
        grid_widget.set_grid(self._grid)

        controls = self.query_one("#controls", ControlsBar)
        controls.grid_size = f"{self._grid_width}x{self._grid_height}"
        controls.speed = self._speed

        self._update_controls()
        self._start_timer()

        # Welcome message in chat
        chat_log = self.query_one("#chat-log", ChatLog)
        chat_log.write("[bold]Welcome to Game of Life![/bold]")
        chat_log.write("Chat with an AI to place patterns, control the simulation, and more.")
        chat_log.write("Set ANTHROPIC_API_KEY or GEMINI_API_KEY to enable AI chat.")

        # Try to initialize LLM client
        self._init_llm()

    def _init_llm(self):
        try:
            from ..llm.client import LLMClient
            self._llm_client = LLMClient(self._grid)
            if self._llm_client.available:
                chat_log = self.query_one("#chat-log", ChatLog)
                chat_log.write(f"[green]AI connected: {self._llm_client.provider}[/green]")
        except Exception:
            pass

    def _start_timer(self):
        if self._timer:
            self._timer.stop()
        interval = 1.0 / self._speed if self._speed > 0 else 1.0
        self._timer = self.set_interval(interval, self._tick)

    def _tick(self):
        if self._paused or self._grid is None:
            return
        self._grid.step()
        self.query_one("#grid-widget", GridWidget).refresh()
        self._update_controls()

    def _update_controls(self):
        if self._grid is None:
            return
        controls = self.query_one("#controls", ControlsBar)
        controls.generation = self._grid.generation
        controls.population = self._grid.population
        controls.paused = self._paused

    def action_toggle_pause(self):
        self._paused = not self._paused
        self._update_controls()

    def action_single_step(self):
        if self._grid is None:
            return
        self._paused = True
        self._grid.step()
        self.query_one("#grid-widget", GridWidget).refresh()
        self._update_controls()

    def action_speed_up(self):
        self._speed = min(60.0, self._speed + 2)
        controls = self.query_one("#controls", ControlsBar)
        controls.speed = self._speed
        self._start_timer()

    def action_speed_down(self):
        self._speed = max(1.0, self._speed - 2)
        controls = self.query_one("#controls", ControlsBar)
        controls.speed = self._speed
        self._start_timer()

    def action_reset(self):
        if self._grid is None:
            return
        self._grid.clear()
        self._grid.randomize(0.1)
        self.query_one("#grid-widget", GridWidget).refresh()
        self._update_controls()

    def action_new_grid(self):
        if gol_engine is None:
            return
        self._grid = gol_engine.Grid(self._grid_width, self._grid_height)
        self._grid.randomize(0.1)
        self.query_one("#grid-widget", GridWidget).set_grid(self._grid)
        self._update_controls()

    def action_scroll_left(self):
        self.query_one("#grid-widget", GridWidget).scroll_viewport(-5, 0)

    def action_scroll_right(self):
        self.query_one("#grid-widget", GridWidget).scroll_viewport(5, 0)

    def action_scroll_up(self):
        self.query_one("#grid-widget", GridWidget).scroll_viewport(0, -5)

    def action_scroll_down(self):
        self.query_one("#grid-widget", GridWidget).scroll_viewport(0, 5)

    async def on_input_submitted(self, event: ChatInput.Submitted) -> None:
        if event.input.id != "chat-input":
            return

        message = event.value.strip()
        if not message:
            return

        event.input.value = ""
        chat_log = self.query_one("#chat-log", ChatLog)
        chat_log.write(f"[bold cyan]You:[/bold cyan] {message}")

        if self._llm_client and self._llm_client.available:
            self._send_to_llm(message)
        else:
            chat_log.write("[yellow]No AI provider configured. Set ANTHROPIC_API_KEY or GEMINI_API_KEY.[/yellow]")

    @work(thread=True)
    def _send_to_llm(self, message: str):
        chat_log = self.query_one("#chat-log", ChatLog)
        try:
            response = self._llm_client.chat(message)
            self.call_from_thread(chat_log.write, f"[bold green]AI:[/bold green] {response}")
            # Refresh grid in case the AI modified it
            self.call_from_thread(self.query_one("#grid-widget", GridWidget).refresh)
            self.call_from_thread(self._update_controls)
        except Exception as e:
            self.call_from_thread(chat_log.write, f"[red]Error: {e}[/red]")


def main():
    import argparse
    parser = argparse.ArgumentParser(description="Game of Life TUI")
    parser.add_argument("--width", type=int, default=200, help="Grid width")
    parser.add_argument("--height", type=int, default=100, help="Grid height")
    args = parser.parse_args()

    app = GameOfLifeApp(width=args.width, height=args.height)
    app.run()


if __name__ == "__main__":
    main()
