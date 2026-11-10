# Themes and appearance

## Minimal theme

```cpp
Theme dark;
dark.Name("Dark Custom")
  .Accent("#18181b")
  .Background("#101010")
  .Outline("#ffffff")
  .Text("#ffffff")
  .Placeholder("#7a7a7a")
  .Button("#52525b")
  .Icon("#a1a1aa");
AddTheme(dark);
SetTheme("Dark Custom");
```

This is the C++ equivalent of WindUI's minimal Lua `WindUI:AddTheme`. Hex values use strings such as `"#0091ff"` (not `Color3`).

## Advanced theme fields

Any advanced property can be added with `Theme::Color(name, hex)` or `Theme::Value(name, Json)`. The *API accepts* the following WindUI-style keys; the current renderer maps the most visible ones to CSS custom properties, with other fields retained as configuration metadata.

| Area | Fields |
| --- | --- |
| Core | `Primary`, `White`, `Black`, `Dialog`, `Background`, `BackgroundTransparency`, `Hover` |
| Panels | `PanelBackground`, `PanelBackgroundTransparency`, `WindowBackground`, `WindowShadow`, `WindowTopbarTitle`, `WindowTopbarAuthor`, `WindowTopbarIcon`, `WindowTopbarButtonIcon`, `WindowSearchBarBackground` |
| Tabs | `TabBackground`, `TabBackgroundHover`, `TabBackgroundHoverTransparency`, `TabBackgroundActive`, `TabBackgroundActiveTransparency`, `TabText`, `TabTextTransparency`, `TabTextTransparencyActive`, `TabTitle`, `TabIcon`, `TabIconTransparency`, `TabIconTransparencyActive`, `TabBorder`, `TabBorderTransparency`, `TabBorderTransparencyActive`, `TabSectionIcon` |
| Elements | `ElementBackground`, `ElementBackgroundTransparency`, `ElementBackgroundHover`, `ElementTitle`, `ElementDesc`, `ElementIcon`, `Toggle`, `ToggleBar`, `Checkbox`, `CheckboxIcon`, `CheckboxBorder`, `CheckboxBorderTransparency`, `Slider`, `SliderThumb`, `SliderIcon`, `SliderIconFrom`, `SliderIconTo` |
| Popups | `PopupBackground`, `PopupBackgroundTransparency`, `PopupTitle`, `PopupContent`, `PopupIcon`, `DialogBackground`, `DialogBackgroundTransparency`, `DialogTitle`, `DialogContent`, `DialogIcon` |
| Tooltips | `Tooltip`, `TooltipText`, `TooltipSecondary`, `TooltipSecondaryText` |
| Sections | `SectionIcon`, `SectionExpandIcon`, `SectionExpandIconTransparency`, `SectionBox`, `SectionBoxTransparency`, `SectionBoxBorder`, `SectionBoxBorderTransparency`, `SectionBoxBackground`, `SectionBoxBackgroundTransparency` |
| Other | `SearchBarBorder`, `SearchBarBorderTransparency`, `Notification`, `NotificationTitle`, `NotificationTitleTransparency`, `NotificationContent`, `NotificationContentTransparency`, `NotificationDuration`, `NotificationDurationTransparency`, `NotificationBorder`, `NotificationBorderTransparency`, `DropdownTabBorder`, `LabelBackground`, `LabelBackgroundTransparency` |

**Example:**

```cpp
Theme t;
t.Name("Advanced Dark")
 .Color("Primary","#a1a1aa")
 .Color("WindowBackground","#101010")
 .Color("ElementTitle","#ffffff")
 .Color("TabBackgroundActive","#ffffff")
 .Value("TabBackgroundActiveTransparency",0.93)
 .Color("Toggle","#52525b")
 .Color("Slider","#0091ff")
 .Color("SliderThumb","#ffffff")
 .Value("PanelBackgroundTransparency",0.95);
AddTheme(t);
```

The repository contains a longer field-by-field sample at [`examples/advanced_theme.cpp`](https://github.com/RealBatu20/WindUIBE/blob/main/examples/advanced_theme.cpp) and the original renderer mapping in `assets/windui.js`.

## Built-in presets

`Dark`, `Light`, `Rose`, `Plant`, `Red`, `Indigo`, `Sky`, `Violet`, `Amber`, `Emerald`, `Midnight`, `Crimson`, `Monokai Pro`, `Cotton Candy`, `Mellowsi`, `Rainbow`. The C++ demo also registers `Minimal Dark`.

```cpp
auto names = GetThemes();          // JSON array encoded as std::string
auto current = GetCurrentTheme(); // std::string
SetTheme("Rainbow");
ToggleAcrylic(true);               // CSS transparency; not a live scene blur
```

`Gradient(Json::Array({...}), Config{})` approximates multi-color gradients through CSS. `SetFont`, `SetLanguage`, `Localization`, and arbitrary `Theme::Color` properties exist, but font availability and localization behavior depend on the renderer. **Advanced 1:1 theme fidelity is not verified**.
