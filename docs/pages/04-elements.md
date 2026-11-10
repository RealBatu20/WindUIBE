# Controls & element APIs

Every `Tab`, `Section`, `Group`, `HStack` and `VStack` is a `Container`. All of them provide the same control builders returning `WindUI::Element`. Call `OnChange(callback, userdata)` or `OnClick(callback, userdata)` on the returned element handle.

## Supported control builders

| Builder | Primary fields | Behavior |
| --- | --- | --- |
| `Button` | `Title`, `Icon`, `Variant`; `ButtonText` via `.Set()` | Click callback; icon/variant styling |
| `Toggle` | `Title`, `Value(bool)`, `Flag` | Rounded WindUI pill switch |
| `Checkbox` | `Title`, `Value(bool)` | Small square checkmark, distinct from toggle |
| `Slider` | `Title`, `Range(min,max,default,step)` | HTML range; value callback on change |
| `Dropdown` | `Title`, `Values`, `Value`, `Multi` | Expands **inside** UI; single or multi select |
| `Input` | `Title`, `Placeholder`, `Password`, `Numeric` | Text input with callback on change |
| `Colorpicker` | `Title`, `Default("#rrggbb")` | Android WebView color picker may vary |
| `Keybind` | `Title`, `Value("K")` | Listens for DOM keydown; keyboard behavior depends on Android |
| `Paragraph` | `Title`, `Desc`, `Image`, `Buttons` | Informational card |
| `Code` | `Title`, `Code` | Code display and copy button |
| `Image` | `Title`, `Image`, `Icon` | HTTPS URL or bundled icon fallback |
| `ProgressBar` | `Title`, `Progress` | Non-interactive progress indicator |
| `Label` | `Title` | Group label |
| `Divider` | none | Horizontal separator |
| `Space` | `Columns` | Vertical spacing |

See `include/WindUI.hpp` for the complete `ElementConfig` field list. Exposed fields such as `SearchBarEnabled`, `AllowNone`, `Continuous`, `Transparency`, `ImageSize`, `MenuWidth` and `IconAlign` are not guaranteed to have full Roblox behavior.

## Button and toggle

```cpp
static void clicked(int id, const char* value, void*) { /* "click" */ }
static void toggled(int id, const char* value, void*) { /* "true" or "false" */ }

auto action = tab.Button(ElementConfig{}.Title("Save").Icon("check")
  .Variant("Primary"));
action.OnClick(clicked);

auto enabled = tab.Toggle(ElementConfig{}.Title("Enable feature")
  .Desc("C++ boolean event")
  .Value(false).Flag("feature.enabled"));
enabled.OnChange(toggled);
```

The demo's **Enable notifications** setting is now a `Toggle`, not a square checkbox. A genuine `Checkbox` remains available for explicit checkbox-style forms.

## Slider and dropdown

```cpp
// Value range: Min=0 Max=100 Default=50 Step=5
tab.Slider(ElementConfig{}.Title("Volume").Range(0,100,50,5));

auto mode = tab.Dropdown(ElementConfig{}
  .Title("Select mode")
  .Values(std::vector<std::string>{"Normal","Creative","Advanced"})
  .Value("Normal")
  .Flag("mode"));
mode.OnChange(clicked);

// Multiple selections produce JSON-array callback values.
tab.Dropdown(ElementConfig{}.Title("Tags")
  .Values(std::vector<std::string>{"A","B","C"})
  .Multi(true));
```

Dropdown options are rendered as rows directly beneath the trigger **inside the scrollable WindUI panel**. They do not open the Android full-screen selection dialog. `Value` can be a scalar or a JSON array when `Multi` is enabled. Complex arbitrary dropdown object config is only partly supported.

## Inputs, colors, keybinds

```cpp
tab.Input(ElementConfig{}.Title("Username")
    .Placeholder("Type your name")
    .Value(""));
tab.Input(ElementConfig{}.Title("PIN")
    .Password(true).Placeholder("Secret"));
tab.Colorpicker(ElementConfig{}.Title("Accent")
    .Default("#0091ff"));
tab.Keybind(ElementConfig{}.Title("Shortcut").Value("K"));
```

The HTML `type="color"` picker can still use Android's own picker UI; only **dropdowns** were converted to WindUI's in-panel rows. Keybind selection relies on keyboard events and is not guaranteed to work with all gamepad or Android navigation buttons.

## Element handle methods

| Method | Purpose |
| --- | --- |
| `.OnChange(cb, ptr)`, `.OnClick(cb, ptr)` | Register native callback |
| `.Off()` | Unregister callback |
| `.SetValue(Json)`, `.Set(Json)` | Update value |
| `.SetTitle(str)`, `.SetDesc(str)` | Update text |
| `.SetCode`, `.SetImage`, `.SetColor`, `.SetThumbnail` | Update type-specific fields |
| `.SetMin`, `.SetMax`, `.SetWidth`, `.SetValues` | Change range/layout/options |
| `.Lock()`, `.Unlock()` | Disable/enable input |
| `.Toggle()` | Toggle state through native command |
| `.Highlight()`, `.Refresh()`, `.Select()` | Presentation or action request |
| `.Destroy()` | Remove element and callback registration |
| `.ID()` | Stable element identifier |

Some actions (`Refresh`, selection on a non-tab element, some settable fields) are no-ops in the current renderer. They remain part of the API surface but should not be treated as fully realized Roblox behavior.
