#!/usr/bin/env python3
"""Fail closed: an internal compiling binary is not a complete GLE release."""
import json
from pathlib import Path
p = Path(__file__).with_name('release_manifest.json')
s = json.loads(p.read_text())
missing = [k for k, passed in s['gates'].items() if passed is not True]
if s.get('complete_release') is not True or missing:
    print('GLE release blocked: implementation/validation incomplete.')
    print('\n'.join('- ' + name for name in missing))
    raise SystemExit(1)
print('GLE release gates complete. Review matching build/test provenance before packaging.')
