# WindUI C++17 API quick reference (v2.0.1)

All public declarations are in `include/WindUI.hpp`. Include it in a **C++17**
project. Names follow WindUI documentation, but Lua tables become `Config`
objects and Roblox types have Android/C++ equivalents. See the README for
important incomplete parity. This reference describes the **implemented API**,
not a guarantee of identical Roblox behavior.

## Library functions

| Roblox-style concept | Native API | Notes |
|---|---|---|
| CreateWindow | `Window CreateWindow(WindowConfig)` | Window + sidebar |
| AddTheme | `void AddTheme(Theme)` | Hex colors and extra key-value theme slots |
| GetCurrentTheme / GetThemes | `std::string GetCurrentTheme()` / `GetThemes()` | `GetThemes()` returns a JSON array string |
| Set theme | `SetTheme(name)` | Applies a preset or user theme |
| Gradient | `Json Gradient(Json colors, Config options)` | CSS gradient approximation |
| SetFont / SetLanguage / Localization | Same names | Language/localization is metadata, not complete translation |
| Notify | `Notify(NotificationConfig)` | Toast: Title, Content, Icon, Duration |
| Popup | `Dialog Popup(ModalConfig)` | Call `.Show()` / `.Close()` |
| SetNotificationLower | `SetNotificationLower(bool)` | Places toasts near bottom |
| GetTransparency / GetWindowSize | Same names | C++ tracked values |
| ToggleAcrylic | `ToggleAcrylic(bool)` | CSS translucency |
| SetParent | `SetParent(std::string)` | Intentionally no-op |

## WindowConfig

`.Title`, `.Icon`, `.Author`, `.Folder`, `.Size(widthDp,heightDp)`,
`.MinSize(w,h)`, `.MaxSize(w,h)`, `.Position(Json)`, `.Transparent`, `.Theme`,
`.Resizable`, `.SideBarWidth`, `.HideSearchBar`, `.ScrollBarEnabled`,
`.BackgroundImageTransparency`, `.Background`, `.ShadowTransparency`,
`.Radius`, `.ElementsRadius`, `.TopBarButtonIconSize`, `.ToggleKey`,
`.Acrylic`, `.NewElements`, `.IgnoreAlerts`, `.HidePanelBackground`,
`.AutoScale`, `.Topbar`, `.Footer`, `.OpenButton`, `.User`, `.KeySystem`.

If a nested Lua table is required, pass a JSON object built with
`Json::Object({{"Enabled",true},{"Anonymous",true}})` or a `Config{}.ToJson()`.
Use `Window::KeyValidator(function,userdata)` for a native validation callback.

## Window methods

`Tab(TabConfig)`, `Section(SectionConfig)`, `Config(name)` (save/load/delete
manager), `Tag(ElementConfig)`, `Dialog(ModalConfig)`, `Divider()`,
`CreateTopbarButton(ElementConfig)`, `DisableTopbarButtons(Config)`,
`EditOpenButton(Config)`.

`SetTitle`, `SetAuthor`, `SetIcon`, `SetIconSize`, `SetSize`, `SetToggleKey`,
`SetTheme`, `SetBackgroundImage`, `SetBackgroundImageTransparency`,
`SetBackgroundTransparency`, `SetUIScale`, `GetUIScale`,
`SetPanelBackground`, `IsResizable`, `ToggleTransparency`, `SetToTheCenter`,
`Toggle`, `Open`, `Close`, `Destroy`, `ToggleFullscreen`, `SelectTab`,
`LockAll`, `UnlockAll`, `GetLocked`, `GetUnlocked`, `OnOpen`, `OnClose`,
`OnDestroy`, `OnUserClick`, `KeyValidator`.

Some actions affect HTML presentation only; platform-level Android window
resizing and full lifecycle callbacks remain launcher-dependent.

## Tab/Section/Group/Stack

`TabConfig`: `.Title`, `.Desc`, `.Icon`, `.IconColor`, `.IconShape`,
`.IconThemed`, `.Locked`, `.ShowTabTitle`, `.Border`, `.TabTitleAlign`,
`.CustomEmptyPage`. `Container`/`Tab` methods: `SetTitle`, `SetDesc`,
`Lock`, `Unlock`, `Select`, `Open`, `Close`, `Destroy`, `ScrollToTheElement`.

`SectionConfig`: `.Title`, `.Desc`, `.Icon`, `.IconThemed`, `.Opened`, `.Box`,
`.BoxBorder`, `.TextXAlignment`, `.TextTransparency`, `.TextSize`, `.FontWeight`.

Every `Tab` and nested `Container` exposes `Section`, `Group`, `HStack`,
`VStack`, as well as the element builders below. Sidebar sections are created
with `Window::Section()`; content sections with `Tab::Section()`.

## Elements

Builders returning `Element`:

`Button`, `Toggle`, `Checkbox`, `Slider`, `Input`, `Dropdown`, `Paragraph`,
`Keybind`, `Colorpicker`, `Code`, `Divider`, `Space`, `Image`, `ProgressBar`,
`Label`.

`ElementConfig` accepts `.Title`, `.Desc`, `.Icon`, `.Value`, `.Default`,
`.Placeholder`, `.Locked`, `.Flag`, `.Values`, `.Multi`, `.AllowNone`,
`.SearchBarEnabled`, `.Min`, `.Max`, `.Step`, `.Range(min,max,default,step)`,
`.Color`, `.Transparency`, `.Variant`, `.Image`, `.Thumbnail`, `.Code`,
`.Columns`, `.Opened`, `.TextSize`, `.FontWeight`, `.Password`, `.Numeric`,
`.Type`, `.Icons`, `.Width`, `.Finished`, `.Progress` and other documented
properties in the header.

Each `Element` supports `OnChange`, `OnClick`, `Off`, `Set`, `SetValue`,
`SetTitle`, `SetDesc`, `SetCode`, `SetImage`, `SetColor`, `SetThumbnail`,
`SetMin`, `SetMax`, `SetWidth`, `SetValues`, `Lock`, `Unlock`, `Toggle`,
`Highlight`, `Select`, `Refresh`, `Destroy`.

Callback signature:

```cpp
void callback(int elementId, const char* value, void* userdata);
tab.Toggle(ElementConfig{}.Title("Enabled").Value(true)).OnChange(callback);
```

Callbacks fire from the Android UI thread. Avoid CPU-heavy operations and
avoid direct Minecraft modification on that thread.

## Dialog, Popup, Notification

`ModalConfig{}.Title(...).Icon(...).IconThemed(...).Content(...)
.AddButton(DialogButton("Continue", callback, userdata, "Primary", "check"))`.
Variants: `Primary`, `Secondary`, `Destructive`, `Tertiary` (visual distinctions
may be approximate). `Window::Dialog(config)` and `WindUI::Popup(config)`
return a `WindUI::Dialog` handle with `.Show()`, `.Close()`, `.ID()`.

`Notify(NotificationConfig{}.Title(...).Content(...).Icon(...).Duration(3))`.
Use `SetNotificationLower(true)` to position toasts at bottom.

## Advanced theme

`Theme` accepts the *entire supplied advanced-theme field list* via
`.Color("Field", "#rrggbb")` and `.Value("TransparencyField", .95)`.
Actual WebView CSS currently maps the most visible theme slots. Fields that
require Roblox graphics structures are stored as metadata; CSS approximates
opacity/transparency where possible. See `examples/advanced_theme.cpp`.

## C ABI for plugins

`include/WindUI.h` exports `WindUI_Command`, `WindUI_NextId`, `WindUI_On`,
`WindUI_Off`, `WindUI_RegisterCallback`, `WindUI_Open`, `WindUI_Close`,
`WindUI_IsReady`, `WindUI_SetTheme`, size/scale/transparency getters, theme
getters, window-state and key-validation calls. Pre-v2 `WindUI_AddButton`,
`WindUI_AddToggle`, `WindUI_AddSlider`, `WindUI_AddInput`, `WindUI_AddDropdown`,
`WindUI_AddTab` and `WindUI_Notify` remain available for legacy extensions.
