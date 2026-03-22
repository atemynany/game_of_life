"""Chat panel widgets for the TUI."""

from textual.widgets import RichLog, Input
from textual.containers import Vertical
from textual.widget import Widget


class ChatLog(RichLog):
    """Scrollable chat log for LLM conversation."""

    DEFAULT_CSS = """
    ChatLog {
        height: 1fr;
        border: solid $accent;
        padding: 0 1;
        background: $surface;
    }
    """


class ChatInput(Input):
    """Chat input field."""

    DEFAULT_CSS = """
    ChatInput {
        dock: bottom;
        height: 3;
        border: solid $accent;
        padding: 0 1;
    }
    """

    def __init__(self, **kwargs):
        super().__init__(placeholder="Chat with AI (Enter to send)...", **kwargs)


class ChatPanel(Vertical):
    """Combined chat log + input panel."""

    DEFAULT_CSS = """
    ChatPanel {
        width: 40%;
        min-width: 30;
        height: 1fr;
    }
    """

    def compose(self):
        yield ChatLog(id="chat-log")
        yield ChatInput(id="chat-input")
