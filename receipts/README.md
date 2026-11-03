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
