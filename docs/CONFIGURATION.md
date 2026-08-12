# ReelNX Configuration

ReelNX keeps its runtime files separate from StreamFinEX, StreamFin, and Switchfin.

## Nintendo Switch Paths

```text
Application: sdmc:/switch/ReelNX/ReelNX.nro
Config:      sdmc:/config/ReelNX/
Addon file:  sdmc:/switch/ReelNX/reelnx-addon.txt
Log file:    reelnx.log when passed through --log-file without a path
```

## Addon File

`reelnx-addon.txt` is optional. When present, it is imported at startup and persisted to `sdmc:/config/ReelNX/stremio_addon.json`.

Supported lines:

```text
https://your-stream-addon.example.com/manifest.json
rpdb=YOUR_RPDB_KEY
poster=https://example.com/{imdbId}.jpg
subtitles=https://your-subtitles-addon.example.com/manifest.json
```

Rules:

- The first non-comment addon URL sets the stream addon.
- `rpdb=` expands to a RatingPosterDB poster template.
- `poster=` overrides the poster template directly.
- `subtitles=` sets a compatible subtitle addon.
- Missing `poster`, `rpdb`, or `subtitles` lines disable those optional values when the file exists.
- A missing addon URL does not erase an already-saved stream addon.

## Coexistence

Phase 2 does not migrate or import old StreamFinEX configuration automatically. Keep old config directories intact while testing ReelNX.
