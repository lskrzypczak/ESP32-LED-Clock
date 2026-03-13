#!/usr/bin/env python3
import os

# Paths
# PlatformIO runs extra scripts without setting __file__, so rely on the current working directory.
ROOT = os.path.abspath(os.getcwd())
SRC = os.path.join(ROOT, 'src')
WEB = os.path.join(ROOT, 'web')
INPUT = os.path.join(WEB, 'index.html')
OUTPUT = os.path.join(SRC, 'WebRoot.h')

if not os.path.isfile(INPUT):
    raise SystemExit(f"Missing input HTML file: {INPUT}")

with open(INPUT, 'r', encoding='utf-8') as f:
    html = f.read()

# Ensure the raw literal delimiter doesn't appear in the HTML
if ')rawliteral"' in html:
    raise SystemExit('The HTML contains the sequence )rawliteral" which conflicts with raw string literal delimiter.')

header = f"""#ifndef WEBROOT_H
#define WEBROOT_H

#include <pgmspace.h>

static const char index_html[] PROGMEM = R"rawliteral({html})rawliteral";

#endif // WEBROOT_H
"""

with open(OUTPUT, 'w', encoding='utf-8') as f:
    f.write(header)

print(f"Generated {OUTPUT} from {INPUT}")
