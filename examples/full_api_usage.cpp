#include "../include/WindUI.hpp"
#include <android/log.h>
using namespace WindUI;
static void changed(int id,const char* value,void* userdata){
 __android_log_print(ANDROID_LOG_INFO,"MyNativeUI","%d: %s",id,value?value:"");
}
static int keyValid(const char* value,void*) {return value && std::string(value)=="1234";}
void installModUI(){
 Theme minimal;minimal.Name("MyDark").Accent("#18181b").Background("#101010")
   .Outline("#ffffff").Text("#ffffff").Placeholder("#7a7a7a")
   .Button("#52525b").Icon("#a1a1aa");AddTheme(minimal);
 Config user;user.Set("Enabled",true).Set("Anonymous",true);
 Config key;key.Set("Note","Your access key").Set("SaveKey",true);
 auto window=CreateWindow(WindowConfig{}.Title("My Native Hub").Icon("bird")
   .Author("C++17 on Android").Folder("MyHub").Size(580,460)
   .MinSize(400,300).MaxSize(850,560).Theme("MyDark")
   .Resizable(true).SideBarWidth(200).HideSearchBar(false)
   .User(user.ToJson()).KeySystem(key.ToJson()));
 window.KeyValidator(keyValid);
 window.OnOpen(changed);window.OnClose(changed);window.OnUserClick(changed);
 auto sidebarSection=window.Section(SectionConfig{}.Title("Utility"));
 auto main=sidebarSection.Tab(TabConfig{}.Title("Main").Icon("home").ShowTabTitle(true));
 main.Toggle(ElementConfig{}.Title("Enabled").Value(true).Flag("enabled")).OnChange(changed);
 main.Slider(ElementConfig{}.Title("Volume").Range(0,100,50,1)).OnChange(changed);
 main.Dropdown(ElementConfig{}.Title("Mode").Values(std::vector<std::string>{"A","B","C"})).OnChange(changed);
 main.Input(ElementConfig{}.Title("Username").Placeholder("Type here")).OnChange(changed);
 main.Colorpicker(ElementConfig{}.Title("Accent").Default("#0091ff")).OnChange(changed);
 auto nested=main.Section(SectionConfig{}.Title("Advanced").Opened(false));
 nested.Keybind(ElementConfig{}.Title("Shortcut").Value("K")).OnChange(changed);
 nested.ProgressBar(ElementConfig{}.Title("Downloading").Progress(65));
 auto actions=main.HStack();actions.Button(ElementConfig{}.Title("Yes")).OnClick(changed);
 actions.Button(ElementConfig{}.Title("No")).OnClick(changed);
 auto dialog=window.Dialog(ModalConfig{}.Title("Confirm").Content("Are you sure?")
   .AddButton(DialogButton("Yes",changed)).AddButton(DialogButton("No",changed,nullptr,"Secondary")));
 dialog.Show();
 Popup(ModalConfig{}.Title("Welcome").Content("WindUI native API").AddButton(DialogButton("OK",changed))).Show();
 Notify(NotificationConfig{}.Title("Ready").Content("C++ callbacks enabled").Duration(3));
 SetNotificationLower(true);
 window.SetBackgroundTransparency(.1);window.SetUIScale(1);
 window.Tag(ElementConfig{}.Title("v2.0.1"));window.SelectTab(main);
}
