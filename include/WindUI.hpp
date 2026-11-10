#pragma once
// C++17 WindUI for Android/Levi Launchroid. C++ is the public API; an embedded
// WebView renders widgets. Config names follow Footagesus WindUI documentation.
// Android has no Roblox UDim2, Color3, Instance or Enum.KeyCode; use pixels
// in density-independent dp, #RRGGBB and Android/DOM key names instead.
#include "WindUI.h"
#include <string>
#include <vector>
#include <map>
#include <initializer_list>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <utility>
namespace WindUI {
class Json {
public:
 std::string raw;
 Json():raw("null"){} Json(std::nullptr_t):raw("null"){}
 Json(const char* s):Json(std::string(s?s:"")){}
 Json(const std::string& s){raw="\"";for(unsigned char c:s){switch(c){case '"':raw+="\\\"";break;case '\\':raw+="\\\\";break;case '\n':raw+="\\n";break;case '\r':raw+="\\r";break;case '\t':raw+="\\t";break;default:if(c<32){char b[8];std::snprintf(b,sizeof b,"\\u%04x",c);raw+=b;}else raw+=char(c);}}raw+='"';}
 Json(bool x):raw(x?"true":"false"){}
 Json(int x):raw(std::to_string(x)){}
 Json(long x):raw(std::to_string(x)){}
 Json(double x){if(!std::isfinite(x)){raw="null";return;}char b[64];std::snprintf(b,sizeof b,"%.10g",x);raw=b;}
 static Json Raw(std::string s){Json v;v.raw=std::move(s);return v;}
 static Json Object(std::initializer_list<std::pair<const std::string,Json>> members){std::string s="{";bool first=true;for(const auto& m:members){if(!first)s+=',';first=false;s+=Json(m.first).raw+":"+m.second.raw;}return Raw(s+"}");}
 static Json Array(std::initializer_list<Json> items){std::string s="[";for(const auto& i:items){if(s.size()>1)s+=',';s+=i.raw;}return Raw(s+"]");}
 static Json Array(const std::vector<Json>& items){std::string s="[";for(const auto& i:items){if(s.size()>1)s+=',';s+=i.raw;}return Raw(s+"]");}
};
class Config {
protected: std::map<std::string,Json> values_;
public:
 Config()=default;
 Config(std::initializer_list<std::pair<const std::string,Json>> o):values_(o){}
 Config& Set(const std::string& name,Json value){values_[name]=std::move(value);return *this;}
 const Json* Get(const std::string& name)const{auto i=values_.find(name);return i==values_.end()?nullptr:&i->second;}
 Json ToJson()const{std::string s="{";for(auto const& p:values_){if(s.size()>1)s+=',';s+=Json(p.first).raw+":"+p.second.raw;}return Json::Raw(s+"}");}
};
inline void Send(const char* cmd,const Json& props={},bool persistent=true){WindUI_Command(cmd,props.raw.c_str(),persistent?1:0);}
inline int Id(){return WindUI_NextId();}
#define WUI_PROPERTY(Type,Field) Type& Field(Json v){Set(#Field,std::move(v));return *this;}
struct Theme : public Config {Theme& Color(std::string k,std::string hex){Set(k,hex);return *this;} Theme& Value(std::string k,Json v){Set(k,std::move(v));return *this;} WUI_PROPERTY(Theme,Name) WUI_PROPERTY(Theme,Accent) WUI_PROPERTY(Theme,Background) WUI_PROPERTY(Theme,Outline) WUI_PROPERTY(Theme,Text) WUI_PROPERTY(Theme,Placeholder) WUI_PROPERTY(Theme,Button) WUI_PROPERTY(Theme,Icon)};
struct WindowConfig : public Config {
 WUI_PROPERTY(WindowConfig,Title) WUI_PROPERTY(WindowConfig,Icon) WUI_PROPERTY(WindowConfig,Author) WUI_PROPERTY(WindowConfig,Folder)
 WUI_PROPERTY(WindowConfig,Transparent) WUI_PROPERTY(WindowConfig,Theme) WUI_PROPERTY(WindowConfig,Resizable) WUI_PROPERTY(WindowConfig,SideBarWidth)
 WUI_PROPERTY(WindowConfig,HideSearchBar) WUI_PROPERTY(WindowConfig,ScrollBarEnabled) WUI_PROPERTY(WindowConfig,BackgroundImageTransparency)
 WUI_PROPERTY(WindowConfig,Background) WUI_PROPERTY(WindowConfig,ShadowTransparency) WUI_PROPERTY(WindowConfig,Radius) WUI_PROPERTY(WindowConfig,ElementsRadius)
 WUI_PROPERTY(WindowConfig,TopBarButtonIconSize) WUI_PROPERTY(WindowConfig,ToggleKey) WUI_PROPERTY(WindowConfig,Acrylic) WUI_PROPERTY(WindowConfig,NewElements)
 WUI_PROPERTY(WindowConfig,IgnoreAlerts) WUI_PROPERTY(WindowConfig,HidePanelBackground) WUI_PROPERTY(WindowConfig,AutoScale)
 WUI_PROPERTY(WindowConfig,Topbar) WUI_PROPERTY(WindowConfig,Footer) WUI_PROPERTY(WindowConfig,OpenButton)
 WUI_PROPERTY(WindowConfig,User) WUI_PROPERTY(WindowConfig,KeySystem) WUI_PROPERTY(WindowConfig,Position)
 WindowConfig& Size(int w,int h){Set("Size",Json::Object({{"X",w},{"Y",h}}));width=w;height=h;return *this;}
 WindowConfig& MinSize(int w,int h){Set("MinSize",Json::Object({{"X",w},{"Y",h}}));return *this;}
 WindowConfig& MaxSize(int w,int h){Set("MaxSize",Json::Object({{"X",w},{"Y",h}}));return *this;}
 int width=740,height=490;
};
struct TabConfig : public Config {
 WUI_PROPERTY(TabConfig,Title) WUI_PROPERTY(TabConfig,Desc) WUI_PROPERTY(TabConfig,Icon) WUI_PROPERTY(TabConfig,IconColor)
 WUI_PROPERTY(TabConfig,IconShape) WUI_PROPERTY(TabConfig,IconThemed) WUI_PROPERTY(TabConfig,Locked) WUI_PROPERTY(TabConfig,ShowTabTitle)
 WUI_PROPERTY(TabConfig,Border) WUI_PROPERTY(TabConfig,TabTitleAlign) WUI_PROPERTY(TabConfig,CustomEmptyPage)
};
struct ElementConfig : public Config {
 WUI_PROPERTY(ElementConfig,Title) WUI_PROPERTY(ElementConfig,Desc) WUI_PROPERTY(ElementConfig,Icon) WUI_PROPERTY(ElementConfig,IconColor)
 WUI_PROPERTY(ElementConfig,Value) WUI_PROPERTY(ElementConfig,Default) WUI_PROPERTY(ElementConfig,Placeholder) WUI_PROPERTY(ElementConfig,Locked)
 WUI_PROPERTY(ElementConfig,Flag) WUI_PROPERTY(ElementConfig,Values) WUI_PROPERTY(ElementConfig,Multi) WUI_PROPERTY(ElementConfig,AllowNone)
 WUI_PROPERTY(ElementConfig,SearchBarEnabled) WUI_PROPERTY(ElementConfig,Step) WUI_PROPERTY(ElementConfig,Min) WUI_PROPERTY(ElementConfig,Max)
 WUI_PROPERTY(ElementConfig,Color) WUI_PROPERTY(ElementConfig,Transparency) WUI_PROPERTY(ElementConfig,Variant) WUI_PROPERTY(ElementConfig,Image)
 WUI_PROPERTY(ElementConfig,ImageSize) WUI_PROPERTY(ElementConfig,Thumbnail) WUI_PROPERTY(ElementConfig,ThumbnailSize)
 WUI_PROPERTY(ElementConfig,Code) WUI_PROPERTY(ElementConfig,Columns) WUI_PROPERTY(ElementConfig,Opened)
 WUI_PROPERTY(ElementConfig,TextSize) WUI_PROPERTY(ElementConfig,FontWeight) WUI_PROPERTY(ElementConfig,Password)
 WUI_PROPERTY(ElementConfig,Numeric) WUI_PROPERTY(ElementConfig,Type) WUI_PROPERTY(ElementConfig,Icons) WUI_PROPERTY(ElementConfig,Width) WUI_PROPERTY(ElementConfig,LockedTitle) WUI_PROPERTY(ElementConfig,Finished) WUI_PROPERTY(ElementConfig,Continuous)
 WUI_PROPERTY(ElementConfig,Format) WUI_PROPERTY(ElementConfig,Disabled) WUI_PROPERTY(ElementConfig,Progress)
 WUI_PROPERTY(ElementConfig,Justify) WUI_PROPERTY(ElementConfig,IconAlign) WUI_PROPERTY(ElementConfig,MenuWidth)
 ElementConfig& Range(double min,double max,double def,double step=1){Set("Value",Json::Object({{"Min",min},{"Max",max},{"Default",def},{"Step",step}}));return *this;}
 ElementConfig& Values(const std::vector<std::string>& arr){std::vector<Json> x;for(const auto& v:arr)x.emplace_back(v);Set("Values",Json::Array(x));return *this;}
};
struct DialogButton {
 std::string Title="Button",Icon="",Variant="Primary";
 WindUI_Callback Callback=nullptr;void* Userdata=nullptr;
 DialogButton()=default;
 DialogButton(std::string title,WindUI_Callback cb=nullptr,void* userdata=nullptr,std::string variant="Primary",std::string icon=""):
  Title(std::move(title)),Icon(std::move(icon)),Variant(std::move(variant)),Callback(cb),Userdata(userdata){}
};
struct ModalConfig : public Config {
 std::vector<DialogButton> buttons;
 WUI_PROPERTY(ModalConfig,Title) WUI_PROPERTY(ModalConfig,Content) WUI_PROPERTY(ModalConfig,Icon) WUI_PROPERTY(ModalConfig,IconThemed)
 ModalConfig& AddButton(DialogButton button){buttons.push_back(std::move(button));return *this;}
};
struct NotificationConfig:public Config{
 WUI_PROPERTY(NotificationConfig,Title) WUI_PROPERTY(NotificationConfig,Content) WUI_PROPERTY(NotificationConfig,Icon) WUI_PROPERTY(NotificationConfig,IconThemed) WUI_PROPERTY(NotificationConfig,Duration)
};
struct SectionConfig:public Config { WUI_PROPERTY(SectionConfig,Title) WUI_PROPERTY(SectionConfig,Desc) WUI_PROPERTY(SectionConfig,Icon) WUI_PROPERTY(SectionConfig,IconThemed) WUI_PROPERTY(SectionConfig,Opened) WUI_PROPERTY(SectionConfig,Box) WUI_PROPERTY(SectionConfig,BoxBorder) WUI_PROPERTY(SectionConfig,TextXAlignment) WUI_PROPERTY(SectionConfig,TextTransparency) WUI_PROPERTY(SectionConfig,TextSize) WUI_PROPERTY(SectionConfig,FontWeight) };
class Element {
 int id_=0;
public:
 explicit Element(int id=0):id_(id){}int ID()const{return id_;}
 Element& OnChange(WindUI_Callback cb,void* u=nullptr){WindUI_On(id_,cb,u);return *this;}
 Element& OnClick(WindUI_Callback cb,void* u=nullptr){return OnChange(cb,u);}
 void Off(){WindUI_Off(id_);}
 void SetTitle(const std::string& t)const{Set("Title",t);} void SetDesc(const std::string& t)const{Set("Desc",t);}
 void SetValue(Json v)const{Set("Value",std::move(v));} void Set(Json v)const{Set("Value",std::move(v));}
 void Set(const std::string& property,Json v)const{Send("element.update",Json::Object({{"id",id_},{"changes",Json::Object({{property,v}})}}));}
 void SetCode(const std::string& v)const{Set("Code",v);} void SetImage(const std::string& v)const{Set("Image",v);}
 void SetColor(const std::string& v)const{Set("Color",v);} void SetThumbnail(const std::string& v)const{Set("Thumbnail",v);}
 void Refresh()const{Act("refresh");}
 void Highlight()const{Act("highlight");} void Toggle()const{Act("toggle");}
 void SetMin(double value)const{Set("Min",value);}void SetMax(double value)const{Set("Max",value);}
 void SetWidth(double value)const{Set("Width",value);}
 void Lock()const{Set("Locked",true);}void Unlock()const{Set("Locked",false);}
 void SetValues(const std::vector<std::string>& v)const{std::vector<Json>a;for(auto &x:v)a.emplace_back(x);Set("Values",Json::Array(a));}
 void Destroy()const{Act("destroy");WindUI_Off(id_);}void Select()const{Act("select");}
 void Act(const std::string& action)const{Send("element.action",Json::Object({{"id",id_},{"action",action}}));}
};
class Tab;
class Container {
protected: int id_=0;const char* kind_="tab";
public:
 explicit Container(int id=0,const char* kind="tab"):id_(id),kind_(kind){}int ID()const{return id_;}
 Element Add(const std::string& type,const ElementConfig& cfg={})const {int id=Id();Send("element.create",Json::Object({{"id",id},{"parent",id_},{"type",type},{"config",cfg.ToJson()}}));return Element(id);}
 Element Button(const ElementConfig& c={})const{return Add("button",c);}
 Element Toggle(const ElementConfig& c={})const{return Add("toggle",c);}
 Element Checkbox(const ElementConfig& c={})const{return Add("checkbox",c);}
 Element Slider(const ElementConfig& c={})const{return Add("slider",c);}
 Element Input(const ElementConfig& c={})const{return Add("input",c);}
 Element Dropdown(const ElementConfig& c={})const{return Add("dropdown",c);}
 Element Paragraph(const ElementConfig& c={})const{return Add("paragraph",c);}
 Element Keybind(const ElementConfig& c={})const{return Add("keybind",c);}
 Element Colorpicker(const ElementConfig& c={})const{return Add("colorpicker",c);}
 Element Code(const ElementConfig& c={})const{return Add("code",c);}
 Element Divider()const{return Add("divider");}
 Element Space(const ElementConfig& c={})const{return Add("space",c);}
 Element Image(const ElementConfig& c={})const{return Add("image",c);}
 Element ProgressBar(const ElementConfig& c={})const{return Add("progressbar",c);}
 Element Label(const ElementConfig& c={})const{return Add("label",c);}
 Container Section(const SectionConfig& cfg={})const{return NewContainer("section",cfg.ToJson());}
 Container Group(const ElementConfig& cfg={})const{return NewContainer("group",cfg.ToJson());}
 Container HStack(const ElementConfig& cfg={})const{return NewContainer("hstack",cfg.ToJson());}
 Container VStack(const ElementConfig& cfg={})const{return NewContainer("vstack",cfg.ToJson());}
 WindUI::Tab TabPage(const TabConfig& cfg)const;
 WindUI::Tab Tab(const TabConfig& cfg)const;
 Container NewContainer(const char* type,const Json& cfg)const {int id=Id();Send("container.create",Json::Object({{"id",id},{"parent",id_},{"type",type},{"config",cfg}}));return Container(id,type);}
 void SetTitle(const std::string& title)const{Send("container.update",Json::Object({{"id",id_},{"changes",Json::Object({{"Title",title}})}}));}
 void SetDesc(const std::string& d)const{Send("container.update",Json::Object({{"id",id_},{"changes",Json::Object({{"Desc",d}})}}));}
 void Lock()const{SetLocked(true);}void Unlock()const{SetLocked(false);}void SetLocked(bool b)const{Send("container.update",Json::Object({{"id",id_},{"changes",Json::Object({{"Locked",b}})}}));}
 void Select()const{Action("select");}
 void Open()const{Action("open");}void Close()const{Action("close");}void Destroy()const{Action("destroy");}
 void Action(const std::string& action)const{Send("container.action",Json::Object({{"id",id_},{"action",action}}),action=="destroy");}
 void ScrollToTheElement(int elementId)const{Send("container.action",Json::Object({{"id",id_},{"action","scrollTo"},{"target",elementId}}),false);}
};
class Tab:public Container {public:explicit Tab(int id=0):Container(id,"tab"){} };
inline WindUI::Tab Container::TabPage(const TabConfig& cfg)const{
 int id=Id();Send("tab.create",Json::Object({{"id",id},{"parent",id_},{"config",cfg.ToJson()}}));return WindUI::Tab(id);
}
inline WindUI::Tab Container::Tab(const TabConfig& cfg)const{return TabPage(cfg);}
class Dialog {
 int id_=0;
public:
 explicit Dialog(int id=0):id_(id){}
 void Show()const{WindUI_Open();Send("modal.action",Json::Object({{"id",id_},{"action","show"}}),false);}
 void Close()const{Send("modal.action",Json::Object({{"id",id_},{"action","close"}}),false);}
 int ID()const{return id_;}
};
inline Dialog Modal(const std::string& type,const ModalConfig& config){
 int id=Id();Json b=Json::Raw("[");std::string s="[";for(const auto& x:config.buttons){if(s.size()>1)s+=',';int bid=Id();WindUI_On(bid,x.Callback,x.Userdata);s+=Json::Object({{"id",bid},{"Title",x.Title},{"Variant",x.Variant},{"Icon",x.Icon}}).raw;}
 s+="]";Send("modal.create",Json::Object({{"id",id},{"type",type},{"config",config.ToJson()},{"buttons",Json::Raw(s)}}));return Dialog(id);
}
class ConfigManager {
 std::string name_;
public:
 explicit ConfigManager(std::string name="Default"):name_(std::move(name)){}
 void Save()const{Send("config.action",Json::Object({{"name",name_},{"action","save"}}),false);}
 void Load()const{Send("config.action",Json::Object({{"name",name_},{"action","load"}}),false);}
 void Delete()const{Send("config.action",Json::Object({{"name",name_},{"action","delete"}}),false);}
};
class Window {
 int id_=0;
public:
 explicit Window(int id=0):id_(id){}int ID()const{return id_;}
 ConfigManager Config(const std::string& name="Default")const{return ConfigManager(name);}
 WindUI::Tab TabPage(const TabConfig& cfg)const{int id=Id();Send("tab.create",Json::Object({{"id",id},{"parent",id_},{"config",cfg.ToJson()}}));return WindUI::Tab(id);}
 WindUI::Tab Tab(const TabConfig& cfg)const{return TabPage(cfg);}
 Container Section(const SectionConfig& cfg={})const{int id=Id();Send("navsection.create",Json::Object({{"id",id},{"parent",id_},{"config",cfg.ToJson()}}));return Container(id,"navsection");}
 WindUI::Dialog DialogBox(const ModalConfig& cfg)const{return Modal("dialog",cfg);} WindUI::Dialog Dialog(const ModalConfig& cfg)const{return DialogBox(cfg);}
 void SetTitle(const std::string& v)const{Update("Title",v);}void SetAuthor(const std::string& v)const{Update("Author",v);}
 void SetIcon(const std::string& v)const{Update("Icon",v);}void SetIconSize(int v)const{Update("IconSize",v);}
 void SetSize(int w,int h)const{WindUI_SetOverlaySize(w,h);Update("Size",Json::Object({{"X",w},{"Y",h}}));}
 void SetToggleKey(const std::string& v)const{Update("ToggleKey",v);}void SetTheme(const std::string& v)const{WindUI_SetTheme(v.c_str());}
 void SetBackgroundImage(const std::string& v)const{Update("Background",v);}
 void SetBackgroundImageTransparency(double v)const{Update("BackgroundImageTransparency",v);}void SetBackgroundTransparency(double v)const{WindUI_SetTransparency(v);Update("BackgroundTransparency",v);}
 void SetUIScale(double v)const{WindUI_SetUIScale(v);Update("UIScale",v);} double GetUIScale()const{return WindUI_GetUIScale();}
 void SetPanelBackground(bool v)const{Update("HidePanelBackground",!v);}void IsResizable(bool v)const{Update("Resizable",v);}
 void ToggleTransparency(bool v)const{Update("Transparent",v);}void SetToTheCenter()const{Action("center");}
 void Toggle()const{Action("toggle");}void Open()const{WindUI_Open();Action("open");}void Close()const{Action("close");WindUI_Close();}
 void Destroy()const{Action("destroy");}void ToggleFullscreen()const{Action("fullscreen");}
 void SelectTab(const WindUI::Tab& t)const{Send("container.action",Json::Object({{"id",t.ID()},{"action","select"}}),false);}
 void LockAll()const{Action("lockAll");}void UnlockAll()const{Action("unlockAll");}
 std::vector<int> GetLocked()const{return LockedIds(true);}
 std::vector<int> GetUnlocked()const{return LockedIds(false);}
 static std::vector<int> LockedIds(bool locked){std::vector<int> ids;const char* c=WindUI_GetLockedIdsJSON(locked);if(!c)return ids;std::string s(c),num;for(char k:s){if((k>='0'&&k<='9')||k=='-')num+=k;else if(!num.empty()){ids.push_back(std::atoi(num.c_str()));num.clear();}}return ids;}
 void OnOpen(WindUI_Callback cb,void* user=nullptr)const{WindUI_On(-id_*10-1,cb,user);}
 void OnClose(WindUI_Callback cb,void* user=nullptr)const{WindUI_On(-id_*10-2,cb,user);}
 void OnDestroy(WindUI_Callback cb,void* user=nullptr)const{WindUI_On(-id_*10-3,cb,user);}
 void OnUserClick(WindUI_Callback cb,void* user=nullptr)const{WindUI_On(-id_*10-5,cb,user);}
 void KeyValidator(WindUI_KeyValidator cb,void* user=nullptr)const{WindUI_OnKeyValidation(id_,cb,user);}
 Element Tag(const ElementConfig& c)const{int id=Id();Send("tag.create",Json::Object({{"id",id},{"config",c.ToJson()}}));return Element(id);}
 void EditOpenButton(const WindUI::Config& c)const{
  if(const Json* v=c.Get("Enabled")){WindUI_SetOpenButtonEnabled(v->raw=="true"?1:0);}
  Update("OpenButton",c.ToJson());}
 Element Divider()const{int id=Id();Send("navsection.divider",Json::Object({{"id",id}}));return Element(id);}
 void DisableTopbarButtons(const WindUI::Config& c)const{Update("DisabledTopbarButtons",c.ToJson());}
 Element CreateTopbarButton(const ElementConfig& c)const{int id=Id();Send("topbarButton.create",Json::Object({{"id",id},{"config",c.ToJson()}}));return Element(id);}
 void Update(std::string key,Json value)const{Send("window.update",Json::Object({{"id",id_},{"changes",Json::Object({{key,value}})}}));}
 void Action(const std::string& action)const{Send("window.action",Json::Object({{"id",id_},{"action",action}}),false);}
};
inline Window CreateWindow(const WindowConfig& cfg){int id=Id();WindUI_SetOverlaySize(cfg.width,cfg.height);if(const Json* t=cfg.Get("Theme")){std::string n=t->raw;if(n.size()>1)WindUI_SetTheme(n.substr(1,n.size()-2).c_str());}Send("window.create",Json::Object({{"id",id},{"config",cfg.ToJson()}}));return Window(id);}
inline void AddTheme(const Theme& t){Send("theme.add",t.ToJson());}
inline void SetTheme(const std::string& t){WindUI_SetTheme(t.c_str());}
inline std::string GetCurrentTheme(){const char* s=WindUI_GetCurrentTheme();return s?s:"Dark";}
inline std::string GetThemes(){const char* s=WindUI_GetThemesJSON();return s?s:"[]";}
inline void Notify(const NotificationConfig& c){Send("notify",c.ToJson(),false);}
inline Dialog Popup(const ModalConfig& c){return Modal("popup",c);}
inline void SetNotificationLower(bool value){Send("library.update",Json::Object({{"NotificationLower",value}}));}
inline void SetFont(const std::string& font){Send("library.update",Json::Object({{"Font",font}}));}
inline void SetLanguage(const std::string& code){Send("library.update",Json::Object({{"Language",code}}));}
inline void Localization(const Config& cfg){Send("library.update",Json::Object({{"Localization",cfg.ToJson()}}));}
inline void ToggleAcrylic(bool v){Send("library.update",Json::Object({{"Acrylic",v}}));}
inline Json Gradient(const Json& colors,const Config& cfg={}){Json out=Json::Object({{"Colors",colors},{"Config",cfg.ToJson()}});Send("library.update",Json::Object({{"Gradient",colors},{"GradientConfig",cfg.ToJson()}}));return out;}
inline void SetParent(const std::string& /*androidViewId*/){/* Roblox Instance parenting has no Android equivalent. */}
inline double GetTransparency(){return WindUI_GetTransparency();}
inline Json GetWindowSize(){int w=0,h=0;WindUI_GetWindowSize(&w,&h);return Json::Object({{"X",w},{"Y",h}});}
#undef WUI_PROPERTY
} // namespace WindUI
