#!/usr/bin/env python3
"""Materialize the approved Chaos title artwork and GBA text-BG tilemap."""
import base64
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
src = ROOT / "graphics/title_screen/chaos_title_source.png.base64"
png = ROOT / "graphics/title_screen/chaos_title.png"
tilemap = ROOT / "graphics/title_screen/chaos_title.bin"

png.write_bytes(base64.b64decode(src.read_text().strip(), validate=True))

entries = [0] * (32 * 32)
for y in range(20):
    for x in range(30):
        entries[y * 32 + x] = y * 30 + x
tilemap.write_bytes(b"".join(struct.pack("<H", n) for n in entries))

print(f"Materialized Chaos title: {png} ({png.stat().st_size} bytes)")
print(f"Generated Chaos title tilemap: {tilemap} ({tilemap.stat().st_size} bytes)")
