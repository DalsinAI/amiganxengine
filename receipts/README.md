# Build receipts

Each file is a `receipt.json` (or `data-receipt.json`) exactly as the build
wrote it, byte for byte (its SHA-256 is in the table), kept here because the
build output itself is not committed. `repository_commit` is the commit that
was built; the receipt is committed after it.

| File | Build | Where | Receipt SHA-256 |
|---|---|---|---|
| `1-build-open5-stove_build.json` | Cradle `stove_build` of `f992e08`, target `.` (job `j20261103T020914-854fa3`) | Cradle's `os32-gcc16` stove (open5) | `80fe8bfe7186dfcd832dd9734238200e3b220e1addb6cf73635fd583f8e4560a` |
| `1-data.json` | Cradle `stove_build` of `f992e08`, target `runtime-data` (job `j20261103T021128-d3f6dc`; the data staged for a lab, not committed) | Cradle (open5) | `4d85ed975aa0b026dd3ecb028965bffbbed304e09408d007cc860c3163e93354` |
| `2-presentation-open5-stove_build.json` | Cradle `stove_build` of `f77fa71`, target `.` (job `j20261103T022206-4559fe`) | Cradle's `os32-gcc16` stove (open5) | `5dbf01d4d0ee18897709dd4ccd793591f2b477402f264f88ddbf30929deafcc4` |
| `3-input-open5-stove_build.json` | Cradle `stove_build` of `be4f213`, target `.` (job `j20261103T024016-c2e06d`) | Cradle's `os32-gcc16` stove (open5) | `882377f155af4c68b6e6c63b1754333608c5db0b730415a76b4d30d3692eae97` |
| `4-sound-open5-stove_build.json` | Cradle `stove_build` of `1a78811`, target `.` (job `j20261103T024809-894b37`) | Cradle's `os32-gcc16` stove (open5) | `98605059373c066a183ec7edd395f648390858303de3cc561e6b6c0ac2cdc243` |
| `6-package-open5-stove_build.json` | Cradle `stove_build` of `06b1405`, target `package` (job `j20261103T032941-a1a4bf`; the LhA archive `9413c052...6f57`, 1,484,766 bytes) | Cradle's `os32-gcc16` stove (open5) | `40beb77f91670315579fcfb4ebf5cc961e1fccaa2ca526a58f8b89d21922d4ab` |
| `7-build-open8-team1.json` | Team 1 clean source build of `744f4da`, target `.` (compile/link proof only; no data or runtime pass) | `/home/da1ek/AmiCradle/kitchen/stoves/os32-gcc16-open8` | `71ad66b7aa37f782f9bd1cd92c179d8d8d6024a43152438e949a911bf2d48eed` |
