"""LLM configuration — API keys from env vars or config file."""

import os
from pathlib import Path

try:
    import tomllib
except ImportError:
    try:
        import tomli as tomllib
    except ImportError:
        tomllib = None


def get_config() -> dict:
    """Load config from env vars, falling back to ~/.config/gol/config.toml."""
    config = {
        "anthropic_api_key": os.environ.get("ANTHROPIC_API_KEY", ""),
        "gemini_api_key": os.environ.get("GEMINI_API_KEY", ""),
    }

    # Try config file if env vars not set
    if not config["anthropic_api_key"] and not config["gemini_api_key"]:
        config_path = Path.home() / ".config" / "gol" / "config.toml"
        if config_path.exists() and tomllib is not None:
            with open(config_path, "rb") as f:
                file_config = tomllib.load(f)
            config["anthropic_api_key"] = file_config.get("anthropic_api_key", "")
            config["gemini_api_key"] = file_config.get("gemini_api_key", "")

    return config
