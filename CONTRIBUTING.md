# Contributing to WindUIBE

WindUIBE is an unofficial native Android UI library. Contributions should be tested with `./build.sh`, `python3 tests/test_static.py`, and, when possible, `python3 tests/test_browser.py` before opening a PR. Device testing and logcat reproduction steps are especially valuable.

Please update both the `assets/windui.js` renderer and `include/WindUI.hpp` when adding a widget method. Every public API change should include a C++ example and an entry in `docs/pages/`. Generated `docs/*.html` must be rebuilt with `python3 scripts/build_docs.py` (requires `mistune`).

Avoid claiming a feature is 100% Roblox-compatible unless this can be demonstrated. Do not include proprietary Minecraft binaries, Android NDK archives, credentials, or unlicensed upstream code in commits. Existing Minecraft binary signature is diagnostic only; no gameplay hooks or memory patches are required by this UI project.

When reporting a bug, include the Minecraft version, Levi Launchroid version, Android version, device architecture, logcat excerpt and reproduction instructions. Refrain from posting private account identifiers or tokens.
