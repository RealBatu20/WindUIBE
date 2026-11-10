# Build, tests and limitations

## Build environment

Use a Linux host with Python 3 and Android NDK r28c. The supplied build uses the Android API 26 ARM64 Clang toolchain. Java source is retained as a reference; `tools/make_bridge_dex.py` constructs the small embedded DEX without a JDK/R8 dependency.

```sh
export ANDROID_NDK_HOME=/path/to/android-ndk-r28c
./build.sh
python3 tests/test_static.py
```

`build.sh` writes a 16K-page-compatible ARM64 `dist/libWindUI.so` and wraps it with `manifest.json`, `LICENSE`, and `ATTRIBUTION.txt` into `dist/libWindUI_2.0.1.levipack`.

The stripped library depends on Android system libraries and `libc++_shared.so` at runtime. Not all Levi Launchroid installations necessarily supply these dependencies in the module's linker namespace.

## Optional renderer tests

```sh
# Requires g++ / Playwright / Chromium
# Host mock stubs are included for generating test commands.
g++ -std=c++17 -Itests/hoststub src/demo.cpp tests/mock.cpp -o tests/demo_host
./tests/demo_host > tests/demo_commands.jsonl
python3 tests/test_browser.py
```

The source bundle includes `tests/mock.cpp`, `tests/hoststub/android/log.h`, and `tests/demo_commands.jsonl` so the optional browser test can run independently. The standalone static test runs after an NDK build. A browser test only checks HTML/JavaScript behavior and cannot certify actual Minecraft integration.

## Verified offline

- ARM64 ELF compilation/linking, packaging and ZIP integrity.
- Manifest fields, embedded DEX structure and checksums.
- Chromium rendering of four demo tabs, nested sections, toggles, sliders, dropdowns, inputs, dialogs, popups, notifications and theme switching with zero page JavaScript errors.
- The v2.0.1 change replaces the demo's **Enable notifications** checkbox with a rounded toggle and the system dropdown picker with an **in-panel menu**.

## Not verified / remaining differences

- On-device Levi Launchroid 1.6.x and Minecraft injection, touch dispatch, overlay lifetime or activity switching.
- Every WindUI advanced theme key affecting a visible element.
- Accurate translation/localization, Roblox asset resolution, custom key binding across physical controllers, secure key validation, original acrylic/gradient/animation visuals.
- Every configuration value receiving an observable GUI effect. Some builders are accepted and stored as metadata.
- True native Minecraft GUI integration (current implementation is WebView).

## Troubleshooting

**The `W` button does not appear.** Check launcher native-mod enablement, architecture and logcat for `WindUI` tags. Activity discovery uses `InbuiltOverlayManager` and may need adaptation to a newer launcher.

**The panel opens but is blank.** Check Android WebView availability and JavaScript errors. The HTML and DEX are embedded in `libWindUI.so`.

**The module fails to load.** Check `libc++_shared.so` and Android API 26 support. `readelf -d dist/libWindUI.so` shows its dynamic dependencies.

**A dropdown opens an Android full-screen list.** That indicates an older v2.0.0 renderer containing `<select>`. Rebuild/replace with v2.0.1, where the dropdown uses buttons inside the WindUI scroll area.

**`GetThemes()` doesn't show your custom theme.** Theme registration queues commands asynchronously, and C++ getters may reflect tracked state rather than a synchronous WebView query.

**Minecraft versions:** `"1.26.*"`, `"1.26.6*"` are the requested manifest strings, not evidence all listed versions were tested.

## Credits and license

The project is MIT-licensed. WindUIBE is an independent Android adaptation inspired by [Footagesus/WindUI](https://github.com/Footagesus/WindUI) and the Java/Dex loader approach of [Zxwmwmwm/LumaUI](https://github.com/Zxwmwmwm/LumaUI). Neither upstream project endorses this mod. See `ATTRIBUTION.txt` and `LICENSE`.
