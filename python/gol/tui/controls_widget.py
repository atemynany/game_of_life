"""Status bar and controls for the TUI."""

from textual.widgets import Static
from textual.reactive import reactive
from rich.text import Text


class ControlsBar(Static):
    """Bottom status bar showing generation, population, speed, and key hints."""

    DEFAULT_CSS = """
    ControlsBar {
        dock: bottom;
        height: 3;
        background: #1a1a2e;
        color: #e0e0e0;
        padding: 0 1;
    }
    """

    generation: reactive[int] = reactive(0)
    population: reactive[int] = reactive(0)
    speed: reactive[float] = reactive(10.0)
    paused: reactive[bool] = reactive(False)
    grid_size: reactive[str] = reactive("0x0")

    def render(self) -> Text:
        state_color = "red" if self.paused else "green"
        state_text = "PAUSED" if self.paused else "RUNNING"

        t = Text()
        t.append(" Gen: ", style="bold white")
        t.append(f"{self.generation}", style="bold cyan")
        t.append("  Pop: ", style="bold white")
        t.append(f"{self.population}", style="bold cyan")
        t.append("  Speed: ", style="bold white")
        t.append(f"{self.speed:.0f}/s", style="bold cyan")
        t.append(f"  {self.grid_size}", style="dim white")
        t.append(f"  {state_text}", style=f"bold {state_color}")
        t.append("\n")
        t.append(" [p]", style="bold yellow")
        t.append("ause ", style="white")
        t.append("[s]", style="bold yellow")
        t.append("tep ", style="white")
        t.append("[+/-]", style="bold yellow")
        t.append("speed ", style="white")
        t.append("[r]", style="bold yellow")
        t.append("eset ", style="white")
        t.append("[n]", style="bold yellow")
        t.append("ew ", style="white")
        t.append("[q]", style="bold yellow")
        t.append("uit ", style="white")
        t.append("[arrows]", style="bold yellow")
        t.append("scroll", style="white")
        return t
