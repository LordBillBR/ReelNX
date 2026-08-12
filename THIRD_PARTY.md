# Third-Party Notices

ReelNX is derived from StreamFinEX, StreamFin, and Switchfin. It preserves the root Apache License 2.0 license and upstream notices in source files.

This project is not affiliated with or endorsed by Stremio or Nintendo.

## Upstream Projects

- StreamFinEX: immediate fork base for the current codebase and Stremio Addon Protocol integration work.
- StreamFin: Stremio-on-Switch base project.
- Switchfin: original native media client foundation, player integration, Borealis integration, platform code, and build system heritage.

## Bundled / Linked Dependencies

- Borealis: Apache License 2.0. See `library/borealis/LICENSE`.
- lunasvg: MIT License. See `library/lunasvg/LICENSE`.
- libromfs: see `library/borealis/library/lib/extern/libromfs/LICENSE`.
- nanovg: see vendored license files under `library/borealis/library/lib/extern/nanovg/`.
- fmt: see `library/borealis/library/lib/extern/fmt/LICENSE`.
- Yoga: see `library/borealis/library/lib/extern/yoga/LICENSE`.
- tweeny: see `library/borealis/library/lib/extern/tweeny/LICENSE`.
- Material Icons font: see `resources/material/LICENSE.txt`.

## Toolchain / Portlibs

Switch builds use devkitPro/devkitA64/libnx and portlibs including libmpv, FFmpeg, curl, mbedTLS, libssh2, dav1d, and related dependencies. Their licenses are provided by their upstream projects and devkitPro packages.

## Protocol Compatibility

ReelNX supports the Stremio Addon Protocol. References to Stremio in technical code, protocol URLs, or addon compatibility documentation are protocol references only and do not imply endorsement or affiliation.
