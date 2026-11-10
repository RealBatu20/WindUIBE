# Dialogs, popups and notifications

The C++ API represents confirmation dialogs, standalone popups, and toasts as distinct components.

## Dialog

Dialogs use a modal backdrop and block interaction with the underlying panel until dismissed.

```cpp
static void confirmed(int id,const char* value,void*) {}

auto dialog = window.Dialog(
    ModalConfig{}.Title("Confirm Delete")
      .Icon("alert-circle")
      .IconThemed(true)
      .Content("This action cannot be undone.")
      .AddButton(DialogButton("Delete", confirmed, nullptr,
                              "Destructive", "trash-2"))
      .AddButton(DialogButton("Cancel", nullptr, nullptr,
                              "Secondary", "x"))
);
dialog.Show();
// later: dialog.Close();
```

`DialogButton` parameters are `(title, callback, userdata, variant, icon)`; supported labels include `Primary`, `Secondary`, `Destructive` and `Tertiary`. The labels are preserved but visual behavior is not pixel-identical to Roblox.

## Popup

```cpp
auto popup = WindUI::Popup(
  ModalConfig{}.Title("Welcome")
    .Icon("bell")
    .Content("Welcome to the native UI")
    .AddButton(DialogButton("Get Started", confirmed))
);
popup.Show();
popup.Close();
```

Popups render as separate styled windows inside the main UI; they are not Android system windows.

## Notification

```cpp
Notify(NotificationConfig{}
  .Title("Success")
  .Content("Operation completed")
  .Icon("check-circle")
  .Duration(3));

SetNotificationLower(true); // show lower in UI, not Roblox jump button detection
```

Notifications are toast-like messages that auto-dismiss. The lower setting positions them near the bottom of the WindUI view; no Android system notification permission is involved. Roblox-specific jump-button avoidance is not implemented.

## Library API

| Call | Behavior |
| --- | --- |
| `CreateWindow(WindowConfig)` | Create UI state and view |
| `AddTheme(Theme)` | Register color theme |
| `SetTheme(name)`, `GetThemes()`, `GetCurrentTheme()` | Preset/theme control (getter state is C++ tracked) |
| `Gradient(colors, config)` | CSS gradient approximation |
| `SetNotificationLower(bool)` | Toast placement |
| `SetFont(name)` | CSS font-family request |
| `SetLanguage(code)`, `Localization(Config)` | Metadata; incomplete translation |
| `ToggleAcrylic(bool)` | CSS blur/translucency |
| `GetTransparency()`, `GetWindowSize()` | Native model information |
| `SetParent(string)` | **No-op**: Roblox `Instance` not available |

## Key system and user config

The `WindowConfig` accepts `.KeySystem(Json)` and `.User(Json)`. `window.KeyValidator(function, userdata)` registers a C++ key-validation callback. Optional local saving uses WebView storage; **this is a convenience UI gate, not secure authentication**. The Roblox `URL`/thumbnail behaviors are not fully replicated.
