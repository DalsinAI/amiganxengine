#!/bin/sh
# The Cave Story data for NXEngine-evo on AmigaOS 3.x / AmiCradle, staged for
# a lab: one archive and the small unpacker that puts it beside the program
# (PROGDIR:data). This is what Cradle's stove_build runs for the target
# runtime-data (STOVE= and OUT= set; the stove compiles only untar.c, the
# unpacker from amigapowermanga).
# The freeware data is fetched from the address gameports/runtime-data.json
# pins (its SHA-256 and size are checked: scripts/fetch_runtime_data.py), and
# import_data.py lays it out with the engine's own data from the pinned
# upstream commit, as the README says. It is staged on the build host only:
# never committed and not put in the package (runtime-data.json: do not
# republish the archive).
#   STOVE      the stove (default ~/AmiCradle/stoves/os32-gcc16, then
#              ~/AmigaChrome/stoves/os32-gcc16)
#   OUT        the output directory (default build-amiga beside this script)
#   NX_SOURCE  a local upstream checkout holding the pinned commit
#   NX_DATA    a local copy of the pinned data zip (not fetched then)
# Outputs: nx-data.tar (plain ustar of data/), untar (Amiga program:
# "untar nx-data.tar <drawer>"), SHA256SUMS (one line a data file),
# data-receipt.json.
# MIT, Copyright (c) 2026 AmiCradle Team.
set -eu

HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(dirname "$HERE")
if [ -z "${STOVE:-}" ]; then
    STOVE="$HOME/AmiCradle/stoves/os32-gcc16"
    [ -d "$STOVE" ] || STOVE="$HOME/AmigaChrome/stoves/os32-gcc16"
fi
OUT=${OUT:-"$HERE/build-amiga"}
CC="$STOVE/prefix/bin/m68k-amigaos-gcc"
mkdir -p "$OUT"

eval "$(python3 - "$ROOT/gameports/catalog.json" <<'EOF'
import json, shlex, sys
port = json.load(open(sys.argv[1]))["ports"]["nxengine-evo"]
print(f"PIN={shlex.quote(port['source']['commit'])}")
print(f"URL={shlex.quote(port['source']['url'])}")
print(f"DATASET={shlex.quote(port['runtimeData']['dataset'])}")
EOF
)"

WORK=$(mktemp -d "${TMPDIR:-/tmp}/nxengine-data.XXXXXX")
trap 'rm -rf "$WORK"' EXIT INT TERM

if [ -n "${NX_SOURCE:-}" ]; then
    UP=$NX_SOURCE
else
    UP="$WORK/upstream"
    git init -q "$UP"
    git -C "$UP" fetch -q --depth 1 "$URL" "$PIN"
fi
EPOCH=$(git -C "$UP" show -s --format=%ct "$PIN")
mkdir -p "$WORK/engine"
git -C "$UP" archive "$PIN" data | tar -xf - -C "$WORK/engine"

mkdir -p "$WORK/cache"
[ -z "${NX_DATA:-}" ] || cp "$NX_DATA" "$WORK/cache/"
python3 "$ROOT/scripts/fetch_runtime_data.py" --root "$ROOT" --dataset "$DATASET" \
    --cache "$WORK/cache" --allow-network > "$WORK/fetch.json"
ZIP=$(python3 -c '
import json, sys
r = json.load(open(sys.argv[1]))
r = r["results"][0] if "results" in r else r
assert r["state"] in ("downloaded", "cached"), r
print(r["path"])' "$WORK/fetch.json")

python3 "$ROOT/import_data.py" --archive "$ZIP" --engine-data "$WORK/engine/data" --out "$WORK/import"

LIST="$WORK/list"
(cd "$WORK/import" && find data -type f -print | LC_ALL=C sort) > "$LIST"
(cd "$WORK/import" && while read -r f; do sha256sum "$f"; done < "$LIST") > "$OUT/SHA256SUMS"
# the same bytes from the same files: names sorted, owners and times fixed
(cd "$WORK/import" && tar --format=ustar --no-recursion --owner=0 --group=0 --numeric-owner \
    --mode='u=rw,go=r' --mtime="@$EPOCH" -cf "$OUT/nx-data.tar" -T "$LIST")

"$CC" --version | head -n 1
"$CC" -noixemul -m68040 -O2 -Wall -Wno-stringop-truncation \
    -ffile-prefix-map="$HERE"=. -o "$OUT/untar" "$HERE/untar.c"
[ "$(od -An -tx1 -N4 "$OUT/untar" | tr -d ' \n')" = "000003f3" ] || { echo "untar is not an Amiga HUNK executable"; exit 2; }

python3 - "$ROOT" "$OUT" "$PIN" "$ZIP" "$WORK/import/IMPORT.json" <<'PY'
import hashlib, json, subprocess, sys
from pathlib import Path
root, out, pin, zip_path, imported = Path(sys.argv[1]), Path(sys.argv[2]), sys.argv[3], Path(sys.argv[4]), Path(sys.argv[5])
sha = lambda p: hashlib.sha256(Path(p).read_bytes()).hexdigest()
sums = (out / "SHA256SUMS").read_bytes()
imp = json.loads(imported.read_text())
try:
    rev = subprocess.check_output(["git", "-C", str(root), "rev-parse", "HEAD"], text=True).strip()
except Exception:
    rev = "unknown"
receipt = {
    "upstream_commit": pin, "repository_commit": rev,
    "data_zip_sha256": sha(zip_path), "data_zip_bytes": zip_path.stat().st_size,
    "import_sha256": sha(imported),
    "music_aliases": len(imp["music_aliases_by_upstream_crc"]),
    "files": len(sums.splitlines()),
    "sha256sums_sha256": hashlib.sha256(sums).hexdigest(),
    "archive_sha256": sha(out / "nx-data.tar"),
    "archive_bytes": (out / "nx-data.tar").stat().st_size,
    "untar_sha256": sha(out / "untar"),
    "policy": "Freeware Cave Story data, fetched from the pinned address and staged for a lab; not committed, not packaged",
}
(out / "data-receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
print(json.dumps(receipt, indent=2))
PY
