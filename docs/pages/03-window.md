# Windows, tabs and layout

## CreateWindow

```cpp
auto window = WindUI::CreateWindow(
  WindUI::WindowConfig{}
    .Title("My Super Hub")
    .Icon("door-open")
    .Author("by RealBatu20")
    .Folder("MySuperHub")
    .Size(580, 460)
    .MinSize(400, 300)
    .MaxSize(850, 560)
    .Theme("Dark")
    .Resizable(true)
    .SideBarWidth(200)
    .HideSearchBar(false)
    .Acrylic(false)
);
```

Values are pixels in a density-independent layout model, not Roblox `UDim2`. `Size` also informs the Android overlay. Some UI window configuration properties are stored but not fully honored by the Android renderer.

### WindowConfig fields

| Group | Available builders |
| --- | --- |
| Branding | `Title`, `Icon`, `Author`, `Folder` |
| Geometry | `Size(w,h)`, `MinSize(w,h)`, `MaxSize(w,h)`, `Position` |
| Appearance | `Transparent`, `Theme`, `Radius`, `ElementsRadius`, `Background`, `BackgroundImageTransparency`, `ShadowTransparency`, `Acrylic` |
| Navigation | `SideBarWidth`, `HideSearchBar`, `ScrollBarEnabled`, `ToggleKey`, `Topbar`, `Footer`, `OpenButton` |
| Behavior | `Resizable`, `AutoScale`, `NewElements`, `IgnoreAlerts`, `HidePanelBackground`, `TopBarButtonIconSize` |
| Advanced | `User`, `KeySystem` |

For nested fields, pass `Json::Object({{"Enabled",true},{"Anonymous",true}})` to a builder such as `.User(...)`. Roblox-specific optional fields may be accepted without effective rendering.

## Window methods

`window.Tab(TabConfig)`, `window.Section(SectionConfig)` (sidebar section), `window.Config(name)`, `window.Dialog(ModalConfig)`, `window.Tag(ElementConfig)`, `window.Divider()`, `window.CreateTopbarButton(ElementConfig)` and `window.DisableTopbarButtons(Config)`.

**Setters:** `SetTitle`, `SetAuthor`, `SetIcon`, `SetIconSize`, `SetSize`, `SetToggleKey`, `SetTheme`, `SetBackgroundImage`, `SetBackgroundImageTransparency`, `SetBackgroundTransparency`, `SetUIScale`, `GetUIScale`, `SetPanelBackground`, `IsResizable`, `ToggleTransparency`, `SetToTheCenter`.

**Window control:** `Open`, `Close`, `Toggle`, `Destroy`, `ToggleFullscreen`, `SelectTab`, `LockAll`, `UnlockAll`, `GetLocked`, `GetUnlocked`, `EditOpenButton`, `OnOpen`, `OnClose`, `OnDestroy`, `OnUserClick`, `KeyValidator`. Callbacks use `WindUI_Callback` unless otherwise specified.

## Tabs

```cpp
auto tab = window.Tab(TabConfig{}
    .Title("Tools")
    .Desc("Tools available in this panel")
    .Icon("settings")
    .IconColor("#ff6464")
    .IconShape("Square")
    .IconThemed(true)
    .Locked(false)
    .ShowTabTitle(true)
    .Border(false)
    .CustomEmptyPage(Json::Object({
        {"Title","No controls yet"}, {"Desc","Add C++ elements."},
        {"Icon","layers"}
    }))
);
```

TabConfig also exposes `TabTitleAlign`. Available tab methods are `SetTitle`, `SetDesc`, `Lock`, `Unlock`, `Select`, `Open`, `Close`, `Destroy`, and `ScrollToTheElement(element.ID())`.

## Sections, groups and stacks

```cpp
auto sidebar = window.Section(SectionConfig{}.Title("Administration"));
auto settings = sidebar.Tab(TabConfig{}.Title("Settings"));
auto advanced = settings.Section(SectionConfig{}
  .Title("Advanced options").Icon("settings").Opened(false));
advanced.Toggle(ElementConfig{}.Title("Expert mode").Value(false));

auto row = settings.HStack();
row.Button(ElementConfig{}.Title("Save"));
row.Button(ElementConfig{}.Title("Reset"));

settings.Group().Input(ElementConfig{}.Title("Username"));
settings.VStack().Label(ElementConfig{}.Title("Additional information"));
```

`SectionConfig`: `Title`, `Desc`, `Icon`, `IconThemed`, `Opened`, `Box`, `BoxBorder`, `TextXAlignment`, `TextTransparency`, `TextSize`, `FontWeight`. Nested container rendering is supported; individual Roblox layout semantics are approximated by flexbox.

## Persistence

```cpp
auto config = window.Config("default");
config.Save(); config.Load(); config.Delete();
```

This is WebView localStorage for control flags, not Roblox filesystem storage. Only elements using `.Flag("name")` are saved/restored in the built-in control flag model.
