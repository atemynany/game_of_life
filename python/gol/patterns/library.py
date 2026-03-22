"""Fetch RLE patterns from ConwayLife and cache locally."""

import hashlib
from pathlib import Path

CACHE_DIR = Path.home() / ".cache" / "gol" / "patterns"
BASE_URL = "https://conwaylife.appspot.com/pattern/"


def fetch_pattern(name: str) -> str | None:
    """Fetch an RLE pattern by name, using local cache."""
    CACHE_DIR.mkdir(parents=True, exist_ok=True)

    cache_key = hashlib.md5(name.lower().encode()).hexdigest()
    cache_file = CACHE_DIR / f"{cache_key}.rle"

    if cache_file.exists():
        return cache_file.read_text()

    try:
        import httpx
        url = f"{BASE_URL}{name}"
        resp = httpx.get(url, follow_redirects=True, timeout=10.0)
        if resp.status_code == 200:
            rle = resp.text
            cache_file.write_text(rle)
            return rle
    except Exception:
        pass

    return None
