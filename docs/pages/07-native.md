# Native ABI and architecture

## How the bridge works

1. Levi Launchroid loads `libWindUI.so` through the `preload-native` manifest.
2. `src/loader.cpp` discovers the launcher Android Activity and creates the overlay.
3. C++ uses an in-memory DEX containing the `windui.Bridge` Java class for Android UI-thread and touch callbacks.
4. The offline `assets/windui.html` + `assets/windui.js` render controls in an Android WebView.
5. C++ calls `WindUI_Command` to send JSON commands to the view. UI events return through `windui://event` URLs and invoke registered `WindUI_Callback` handlers.

There is no Roblox Lua interpreter, injected Roblox objects, or Minecraft GUI scene graph. This is not a pure-C++ graphics renderer, and Android WebView remains a runtime dependency.

## Exported C ABI (`include/WindUI.h`)

```cpp
typedef void (*WindUI_Callback)(int elementId, const char* value, void* userdata);

int WindUI_IsReady(void);
int WindUI_NextId(void);
int WindUI_Command(const char* method,const char* payloadJson,int persistent);
void WindUI_On(int id,WindUI_Callback callback,void* userdata);
void WindUI_Off(int id);
void WindUI_RegisterCallback(WindUI_Callback callback,void* userdata);
void WindUI_Open(void);
void WindUI_Close(void);
void WindUI_SetOverlaySize(int widthDp,int heightDp);
void WindUI_SetOpenButtonEnabled(int enabled);
void WindUI_SetUIScale(double value);
double WindUI_GetUIScale(void);
void WindUI_SetTransparency(double value);
double WindUI_GetTransparency(void);
void WindUI_GetWindowSize(int* widthDp,int* heightDp);
const char* WindUI_GetCurrentTheme(void);
const char* WindUI_GetThemesJSON(void);
const char* WindUI_GetLockedIdsJSON(int getLocked);
```

Other exports: `WindUI_SetTheme`, `WindUI_OnKeyValidation`, `WindUI_GetDiagnosticSignature`; legacy `WindUI_SetTitle`, `WindUI_Notify`, `WindUI_AddTab`, `WindUI_AddButton`, `WindUI_AddToggle`, `WindUI_AddSlider`, `WindUI_AddInput`, `WindUI_AddDropdown`.

Native extensions may call these through `dlsym` if the launcher's Android linker namespace permits access. The C++ convenience layer expects a linked/resolvable C ABI. See `examples/native_extension.cpp` for a concrete extension.

## Native event handlers

```cpp
static void onChange(int id, const char* value, void* userdata) {
    // value points to an event payload; consume it in this call.
}

auto tab = window.Tab(TabConfig{}.Title("Settings"));
int id = tab.Toggle(ElementConfig{}.Title("Enabled").Value(true))
            .OnChange(onChange).ID();
```

The callback runs on the Android UI thread; marshal to your own worker for expensive work. Avoid blocking WebView event dispatch. JSON and individual values should be validated if they come from dynamic control inputs.

## Wildcard signature

The loader includes a 64-byte hardcoded diagnostic pattern with wildcard bytes for the supplied 1.26.62.1 ARM64 ELF. This pattern is **not used to patch memory or hook Minecraft**. Do not assume it matches other builds or enables compatibility.

## Source/API relationship

`WindUI.hpp` declares builder methods and serializes commands. `windui.js` must implement a corresponding `dispatch` operation and actual DOM interaction for a feature to work. **A declared builder does not automatically mean full Roblox behavior exists.**
