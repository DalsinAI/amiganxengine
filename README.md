# NXEngine-evo (Cave Story) for AmigaOS 3.x / AmiCradle

NXEngine-evo is the open-source engine of Cave Story (Doukutsu Monogatari,
Studio Pixel, 2004): a rewrite of the game's engine that plays the original
freeware game's data. GPL-3.0 (upstream `LICENSE`). This repository holds
the AmiCradle port around upstream
[nxengine/nxengine-evo](https://github.com/nxengine/nxengine-evo) at
`1f093d1423cc395eb199230cd609b806ef1daa36` (2.6.5): 68040 + FPU, libnix,
SDL 2, SDL2_mixer and SDL2_image through GFOpenGPU, OpenAmigaGCC 16.2.0
(open3, open4 or GFOpenAmigaGCC open5). The port is MIT (`LICENSE`); the
upstream tree is not copied here: the build fetches the pinned commit.

Port Atlas: `g-nxengine-evo`.

## Building

The port never edits upstream: `build-amiga.sh` fetches the pinned commit
(`gameports/catalog.json`), takes a copy with `git archive`, applies
`patches/` with no fuzz and `patch_amigaos3_deps.py` (the bundled JSON and
spdlog headers on libnix), and compiles upstream's own source list (the
globs of its `CMakeLists.txt`, 131 sources) with the stove's compilers,
linking the stove's `libSDL2.a`, `libSDL2_mixer.a`, `libSDL2_image.a` and
`libGL.a` (GFOpenGPU's link stubs) and the locked libpng and zlib staged in
the stove (`toolchain/port-dependencies.lock.json`). spdlog and its bundled
fmt are compiled once, as spdlog's own compiled-library build does
(`amiga/spdlog_lib.cpp`, every other source with `SPDLOG_COMPILED_LIB`):
header-only, each source compiled the logger again, the build took over
nine minutes on Cradle and the program was 23.7 MB; now 90 seconds and
3.6 MB.

Through Cradle (the build the lab tests use): `stove_build` with the stove
`os32-gcc16`, target `.` for the program and target `runtime-data` for the
game data staged for a lab. From a checkout, with a staged stove:

```sh
STOVE=/path/to/os32-gcc16 OUT=build-amiga sh build-amiga.sh
STOVE=/path/to/os32-gcc16 OUT=build-amiga/data sh runtime-data/build-amiga.sh
```

`build-amiga.sh` takes OpenAmigaGCC 16.2.0-open3 (archive SHA-256
`9fcc136ab59e3bb5b91d2764d0bf290b709d72d7380be4ce0cdf005edf8eff21`),
16.2.0-open4 (`495319a2ffd803278f8827cad70074029c7aa9ecbe05d037e8646ba43ab94ab2`)
or GFOpenAmigaGCC 16.2.0-open5 (Cradle's `os32-gcc16` since the evening of
2 November 2026; no archive is published, so it is known by `STOVE.json`'s
release and the compiler's own `--version`, and its receipt records the
SHA-256 of the compiler's own programs) and refuses any other. Its
`receipt.json` names the compiler, the recipe's and patches' digests, the
SDK and codec inputs and the executable's SHA-256; the receipts of the
builds recorded here are in `receipts/`.

The older recipes are kept: `build-amigaos3.sh` (CMake, a staged open3
stove) and `build.sh` (AROS).

## Game data

The Cave Story data is not in this repository and not in the package. It
is the original freeware game's, from the address
`gameports/runtime-data.json` pins (SHA-256
`444c4265b5acc11495b8d5b00ef7e7843b5ee8e0f6a5d9c4201ed36adbd018f0`,
1,209,287 bytes; observed on 1 November 2026, not a publisher's digest),
laid out by `import_data.py` with the engine's own data from the pinned
commit:

```sh
python3 import_data.py \
  --archive /path/to/cavestory-nx-data.zip \
  --engine-data /path/to/nxengine-evo/data \
  --out build/nx-test-data
```

The importer makes a new directory and a digest receipt, flattens the old
`data/data` mirror layout, lays the pinned engine data over it, and adds the
original music names where size and CRC match upstream's extraction table.
It refuses unsafe or conflicting paths and never replaces an existing
output. `runtime-data/build-amiga.sh` does all of it on the build host
(fetching the pinned zip, checking its SHA-256 and size) and packs the
result as `nx-data.tar`, a plain ustar archive, with `untar`, a small Amiga
unpacker (`runtime-data/untar.c`, from amigapowermanga): that is how a lab
gets the data. It is staged there only, never committed or packaged.

## Status

**COMPILED**, not RELEASE (see Observed below and the release gates in
amicradle `docs/porting/MEASUREMENT.md`).

| Gate | State |
|---|---|
| Pinned source, reproducible build, receipts | done (open5, Cradle) |

## Observed

### Build, 3 November 2026

Commit `f992e08`: clean compile and link of upstream at the pinned commit
with `patches/0001-aros-resource-manager.patch` and
`patch_amigaos3_deps.py`, no network code, by Cradle's `stove_build` with
the `os32-gcc16` stove (GFOpenAmigaGCC 16.2.0-open5, job
`j20261103T020914-854fa3`): 131 upstream sources and
`amiga/spdlog_lib.cpp` in 90 seconds; the 17 warnings are upstream's own
(unused variables, signed comparisons, `Pixtone.h`'s anonymous typedefs).

| Compiler | Executable SHA-256 | Bytes |
|---|---|---|
| 16.2.0-open5 (Cradle `stove_build`, job `j20261103T020914-854fa3`) | `dbd926a92a94f57d1508241de88b24e7596cf60be3c187acaf3e0e7498244a4a` | 3,613,692 |

The receipt is `receipts/1-build-open5-stove_build.json`. The game data
staged for the lab (job `j20261103T021128-d3f6dc`, `receipts/1-data.json`):
the pinned zip (SHA-256 checked) and the engine's data, 661 files with 26
music aliases, the import receipt `10c0ae7e...60e4` (the same as on
24 October), archive `15f6468d...b42e` (9,000,960 bytes), `untar`
`2de6090b...7b6d`. The same recipe
with spdlog header-only (commit `a7a8e65`, job `j20261103T015647-c343e7`)
also built, in 9 minutes 8 seconds, to a 23,753,916-byte program
(`1d279411...6d2e`); with two compiles at a time (commit `b84479f`, job
`j20261103T014028-36e125`) it ran past stove_build's 900 s, which is what
the catalogue's "deferred by host load" was.

### Earlier runs, 24 October 2026

The open3 build of 24 October (CMake, `build-amigaos3.sh`, executable
`3b3773b7...c866`) reached the title, the opening dialogue and the Start
Point room; it moved, jumped, saved and reloaded and quit with RC 0
([the run receipt](evidence/runtime-20261024.json)). Its saves went to
`ENVARC:nxengine`.
