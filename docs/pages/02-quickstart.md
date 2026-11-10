# Quick start

## Requirements

- Minecraft Bedrock Android running under a compatible Levi Launchroid build; compatibility has **not** been device-validated.
- ARM64 Android target, API level 26+. The `manifest.json` patterns are `"1.26.*"` and `"1.26.6*"`.
- For compiling: Linux, Python 3, and Android NDK r28c (`aarch64-linux-android26-clang++`).

## Build and install

```sh
ANDROID_NDK_HOME=/path/to/android-ndk-r28c ./build.sh
python3 tests/test_static.py
# Optional, with Playwright and Chromium installed:
python3 tests/test_browser.py
```

This generates `dist/libWindUI.so` and `dist/libWindUI_2.0.1.levipack`. Import the `.levipack` in Levi Launchroid's native-mod manager, enable it, start Minecraft, and tap the movable **W** button to open WindUIBE. If injection fails, collect logcat and launcher version details; no device compatibility is guaranteed.

## Minimal native C++ window

```cpp
#include "WindUI.hpp"
using namespace WindUI;

static void changed(int id, const char* value, void* user) {
    // Called from the Android UI thread: keep this handler short.
}

void installUi() {
    auto window = CreateWindow(
        WindowConfig{}.Title("My Super Hub")
            .Author("Native C++")
            .Icon("wind")
            .Size(580, 460)
            .Theme("Dark")
            .SideBarWidth(200)
    );
    auto main = window.Tab(TabConfig{}.Title("Main").Icon("home"));
    main.Toggle(ElementConfig{}.Title("Enabled").Value(true))
        .OnChange(changed);
    main.Slider(ElementConfig{}.Title("Volume").Range(0,100,50,1))
        .OnChange(changed);
    main.Dropdown(ElementConfig{}.Title("Mode")
        .Values(std::vector<std::string>{"Normal","Creative","Advanced"})
        .Value("Normal")).OnChange(changed);
    Notify(NotificationConfig{}.Title("Ready")
        .Content("WindUIBE loaded").Duration(3));
}
```

`Json` converts strings, booleans, numbers, objects and arrays into the serialized command stream. `Config::Set` accepts additional WindUI-style field names: `Config{}.Set("Enabled", true)`; the C++ fluent methods use `.Title(...)`, `.Value(...)`, etc. See `include/WindUI.hpp` for exact signatures.

## Native callbacks

```cpp
void callback(int elementId, const char* value, void* userdata);
```

`value` is a UTF-8 string for strings, numbers (`"75"`), booleans (`"true"`, `"false"`), and JSON-encoded arrays for multiple selections. Callback execution is on Android's UI thread. Do not perform blocking Minecraft operations in a callback.
