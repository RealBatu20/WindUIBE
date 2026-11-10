// Built-in C++ demo. Java is a small JNI bridge; no widget definitions live in Java.
#include "../include/WindUI.hpp"
#include <android/log.h>
#include <atomic>
using namespace WindUI;
static std::atomic<bool> installed{false};
static void onChange(int id,const char* value,void*) {
 __android_log_print(ANDROID_LOG_INFO,"WindUI-Demo","C++ callback %d = %s",id,value?value:"");
}
static void onTheme(int,const char* value,void*) {
 if(value && *value)WindUI::SetTheme(value);
}
static void onAction(int,const char*,void*) {
 Notify(NotificationConfig{}.Title("Native callback").Content("Button event delivered to C++").Icon("check").Duration(3));
}
static void onConfirm(int,const char*,void*) {
 Notify(NotificationConfig{}.Title("Confirmed").Content("The confirmation callback ran in C++.").Duration(3));
}
extern "C" void WindUI_InstallDemo() {
 bool expected=false;if(!installed.compare_exchange_strong(expected,true))return;
 Theme minimal;minimal.Name("Minimal Dark").Accent("#18181b").Background("#101010").Outline("#ffffff").Text("#ffffff").Placeholder("#7a7a7a").Button("#52525b").Icon("#a1a1aa");
 AddTheme(minimal);
 auto win=CreateWindow(WindowConfig{}.Title("WindUI Native").Icon("wind").Author("by TheRealBatu · C++ API").Folder("WindUIBedrock")
    .Size(740,490).MinSize(340,260).MaxSize(1000,720).Resizable(true).Theme("Dark").SideBarWidth(185)
    .HideSearchBar(false).Acrylic(false).NewElements(true).Transparent(true).AutoScale(true));
 auto main=win.Tab(TabConfig{}.Title("Main").Icon("home").Desc("C++ API driven interface").ShowTabTitle(true));
 main.Paragraph(ElementConfig{}.Title("WindUI C++ API").Desc("This demo is constructed in native C++, with a Java loader and an Android WebView renderer. All events are passed to C++. ").Image("check"));
 auto examples=main.Section(SectionConfig{}.Title("Interactive Elements").Icon("settings").Opened(true));
 examples.Toggle(ElementConfig{}.Title("Enable feature").Desc("Native boolean callback").Value(true).Flag("feature.enabled")).OnChange(onChange);
 examples.Slider(ElementConfig{}.Title("Brightness Control").Desc("A configurable value from 0 to 100").Range(0,100,60,1).Flag("brightness")).OnChange(onChange);
 examples.Slider(ElementConfig{}.Title("Volume Settings").Range(0,100,45,1).Flag("volume")).OnChange(onChange);
 auto buttons=main.Group();
 buttons.Button(ElementConfig{}.Title("Save").Icon("check")).OnClick(onAction);
 buttons.Button(ElementConfig{}.Title("Reset").Icon("settings")).OnClick(onAction);
 auto elements=win.Tab(TabConfig{}.Title("Elements").Icon("layers").ShowTabTitle(true));
 elements.Paragraph(ElementConfig{}.Title("Standard WindUI components").Desc("All of these are available through WindUI::Container in C++."));
 elements.Input(ElementConfig{}.Title("Username").Desc("Text input").Placeholder("Type your name").Flag("username")).OnChange(onChange);
 elements.Dropdown(ElementConfig{}.Title("Select mode").Values(std::vector<std::string>{"Normal","Creative","Advanced"}).Value("Normal").Flag("mode")).OnChange(onChange);
 elements.Colorpicker(ElementConfig{}.Title("Accent color").Default("#387cde")).OnChange(onChange);
 elements.Keybind(ElementConfig{}.Title("Bind a key").Value("K")).OnChange(onChange);
 elements.Toggle(ElementConfig{}.Title("Enable notifications").Value(true)).OnChange(onChange);
 elements.ProgressBar(ElementConfig{}.Title("ProgressBar").Progress(68));
 elements.Code(ElementConfig{}.Title("C++ example").Code("auto tab = window.Tab(TabConfig{}.Title(\"Main\"));\ntab.Toggle(ElementConfig{}.Title(\"Enabled\").Value(true));"));
 elements.Divider();elements.Space(ElementConfig{}.Columns(1));
 auto themeTab=win.Tab(TabConfig{}.Title("Themes").Icon("palette").ShowTabTitle(true));
 themeTab.Paragraph(ElementConfig{}.Title("Theme engine").Desc("Use WindUI::AddTheme with Minimal or Advanced theme colors. All theme options below are driven by C++ callbacks."));
 themeTab.Dropdown(ElementConfig{}.Title("Select Theme")
 .Values(std::vector<std::string>{"Dark","Light","Rose","Plant","Red","Indigo","Sky","Violet","Amber","Emerald","Midnight","Crimson","Monokai Pro","Cotton Candy","Mellowsi","Rainbow","Minimal Dark"}).Value("Dark")).OnChange(onTheme);
 themeTab.Colorpicker(ElementConfig{}.Title("Custom color").Default("#a1a1aa")).OnChange(onChange);
 auto info=win.Tab(TabConfig{}.Title("About").Icon("bird").ShowTabTitle(true));
 info.Paragraph(ElementConfig{}.Title("Levi Launchroid").Desc("This mod uses LumaUI-style activity discovery and dynamically loads a small DEX bridge. The layout/control API is C++17. No Minecraft gameplay functions are hooked."));
 info.Image(ElementConfig{}.Title("Offline UI").Desc("No Roblox services or Luau runtime required").Icon("wind"));
 auto confirmation=win.Dialog(ModalConfig{}.Title("Confirm Action").Icon("check").Content("Continue with this native action?")
   .AddButton(DialogButton("Continue",onConfirm,nullptr,"Primary","check"))
   .AddButton(DialogButton("Cancel",nullptr,nullptr,"Secondary","layers")));
 // Show dialog on clicking a separate command button, without holding a dangling C++ object.
 auto openDialog=elements.Button(ElementConfig{}.Title("Open confirmation dialog").Icon("bell"));
 // A tiny function pointer plus userdata lets the callback show the native-managed dialog.
 static Dialog savedDialog; savedDialog=confirmation;
 openDialog.OnClick([](int,const char*,void*){savedDialog.Show();});
 auto popup=Popup(ModalConfig{}.Title("Welcome").Content("This standalone popup is defined through C++.")
  .AddButton(DialogButton("OK",onConfirm,nullptr,"Primary","check")));
 static Dialog savedPopup;savedPopup=popup;
 elements.Button(ElementConfig{}.Title("Open popup").Icon("bell")).OnClick([](int,const char*,void*){savedPopup.Show();});
 elements.Button(ElementConfig{}.Title("Notify").Icon("bell")).OnClick([](int,const char*,void*){Notify(NotificationConfig{}.Title("Notification").Content("Native toast from C++").Duration(3));});
 win.Tag(ElementConfig{}.Title("v2.0.1"));
}
