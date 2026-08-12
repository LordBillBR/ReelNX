# Release Checklist

Use this checklist before publishing a ReelNX alpha release.

## Local Verification

```bash
cmake -B build-reelnx -DPLATFORM_DESKTOP=ON
cmake --build build-reelnx --target ReelNX -j2
DEVKITPRO=/opt/devkitpro cmake -B build_switch_reelnx -G Ninja -DPLATFORM_SWITCH=ON -DBUILTIN_NSP=OFF -DBOREALIS_USE_DEKO3D=ON
DEVKITPRO=/opt/devkitpro cmake --build build_switch_reelnx --target ReelNX.nro -j2
git diff --check
scripts/test_tls.sh
scripts/test_retry_policy.sh
```

## Switch Smoke Test

- Copy `build_switch_reelnx/ReelNX.nro` to `sdmc:/switch/ReelNX/ReelNX.nro`.
- Launch through title override, not applet mode.
- Confirm the app creates or reads `sdmc:/config/ReelNX/`.
- Confirm startup does not read StreamFinEX config automatically.
- Confirm `sdmc:/switch/ReelNX/reelnx-addon.txt` imports addon settings.
- Confirm HTTPS requests still reject invalid certificates.
- Confirm basic playback opens and exits back to the stream picker.

## Tag Release

The release workflow runs only for tags matching `v*`.

```bash
git tag v0.1.0-alpha.1
git push reelnx v0.1.0-alpha.1
```

The workflow derives the display version from the tag and publishes `ReelNX.nro`.
