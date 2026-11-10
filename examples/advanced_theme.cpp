// Advanced WindUI theme mapping, based on the WindUI documentation and user sample.
// Hex colors replace Roblox Color3. All properties are accepted by the C++ API;
// properties without Android/WebView equivalents are retained as theme metadata.
#include "../include/WindUI.hpp"
void InstallAdvancedDarkTheme(){
 using namespace WindUI;
 Theme t;
 t.Name("Advanced Dark")
 .Accent("#18181b").Background("#101010").Outline("#ffffff").Text("#ffffff")
 .Placeholder("#7a7a7a").Button("#52525b").Icon("#a1a1aa");
 t.Color("Primary","#a1a1aa")
  .Color("White","#ffffff").Color("Black","#000000")
  .Color("Dialog","#18181b").Color("Hover","#ffffff")
  .Color("PanelBackground","#ffffff").Value("PanelBackgroundTransparency",.95)
  .Color("WindowBackground","#101010").Color("WindowShadow","#000000")
  .Color("WindowTopbarTitle","#ffffff").Color("WindowTopbarAuthor","#ffffff")
  .Color("WindowTopbarIcon","#a1a1aa").Color("WindowTopbarButtonIcon","#a1a1aa")
  .Color("WindowSearchBarBackground","#101010")
  .Color("TabBackground","#ffffff").Color("TabBackgroundHover","#ffffff")
  .Value("TabBackgroundHoverTransparency",.97)
  .Color("TabBackgroundActive","#ffffff").Value("TabBackgroundActiveTransparency",.93)
  .Color("TabText","#ffffff").Value("TabTextTransparency",.3)
  .Value("TabTextTransparencyActive",0)
  .Color("TabTitle","#ffffff").Color("TabIcon","#a1a1aa")
  .Value("TabIconTransparency",.4).Value("TabIconTransparencyActive",.1)
  .Value("TabBorderTransparency",1).Value("TabBorderTransparencyActive",.75)
  .Color("TabBorder","#ffffff")
  .Color("ElementBackground","#ffffff").Value("ElementBackgroundTransparency",.93)
  .Color("ElementBackgroundHover","#ffffff").Value("ElementBackgroundHoverTransparency",.90)
  .Color("ElementTitle","#ffffff").Color("ElementDesc","#ffffff")
  .Color("ElementIcon","#a1a1aa")
  .Color("PopupBackground","#101010").Value("PopupBackgroundTransparency","BackgroundTransparency")
  .Color("PopupTitle","#ffffff").Color("PopupContent","#ffffff")
  .Color("PopupIcon","#a1a1aa")
  .Color("DialogBackground","#101010").Value("DialogBackgroundTransparency","BackgroundTransparency")
  .Color("DialogTitle","#ffffff").Color("DialogContent","#ffffff")
  .Color("DialogIcon","#a1a1aa")
  .Color("Toggle","#52525b").Color("ToggleBar","#ffffff")
  .Color("Checkbox","#0091ff").Color("CheckboxIcon","#ffffff")
  .Color("CheckboxBorder","#ffffff").Value("CheckboxBorderTransparency",.75)
  .Color("SliderIcon","#a1a1aa").Color("Slider","#0091ff")
  .Color("SliderThumb","#ffffff").Color("SliderIconFrom","#a1a1aa")
  .Color("SliderIconTo","#a1a1aa")
  .Color("Tooltip","#4c4c4c").Color("TooltipText","#ffffff")
  .Color("TooltipSecondary","#0091ff").Color("TooltipSecondaryText","#ffffff")
  .Color("TabSectionIcon","#a1a1aa")
  .Color("SectionIcon","#a1a1aa")
  .Color("SectionExpandIcon","#ffffff").Value("SectionExpandIconTransparency",.4)
  .Color("SectionBox","#ffffff").Value("SectionBoxTransparency",.95)
  .Color("SectionBoxBorder","#ffffff").Value("SectionBoxBorderTransparency",.75)
  .Color("SectionBoxBackground","#ffffff").Value("SectionBoxBackgroundTransparency",.95)
  .Color("SearchBarBorder","#ffffff").Value("SearchBarBorderTransparency",.75)
  .Color("Notification","#101010").Color("NotificationTitle","#ffffff")
  .Value("NotificationTitleTransparency",0).Color("NotificationContent","#ffffff")
  .Value("NotificationContentTransparency",.4).Color("NotificationDuration","#ffffff")
  .Value("NotificationDurationTransparency",.95)
  .Color("NotificationBorder","#ffffff").Value("NotificationBorderTransparency",.75)
  .Color("DropdownTabBorder","#ffffff")
  .Color("LabelBackground","#ffffff").Value("LabelBackgroundTransparency",.95);
 AddTheme(t);
 SetTheme("Advanced Dark");
}
