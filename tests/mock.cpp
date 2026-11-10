#include <cstdio>
#include <atomic>
#include <string>
#include "../include/WindUI.h"
static std::atomic<int> uid{10};
extern "C" int WindUI_NextId(){return uid++;}
extern "C" int WindUI_Command(const char* op,const char* j,int){printf("{\"op\":\"%s\",\"payload\":%s}\n",op,j);return 1;}
extern "C" void WindUI_On(int,WindUI_Callback,void*){}
extern "C" void WindUI_Off(int){}
extern "C" void WindUI_SetOverlaySize(int,int){}
extern "C" void WindUI_SetTheme(const char*){}
extern "C" int WindUI_IsReady(){return 1;}
extern "C" void WindUI_Open(){}
extern "C" void WindUI_Close(){}
extern "C" const char* WindUI_GetCurrentTheme(){return "Dark";}
extern "C" const char* WindUI_GetThemesJSON(){return "[]";}
extern "C" void WindUI_SetUIScale(double){}
extern "C" double WindUI_GetUIScale(){return 1;}
extern "C" void WindUI_SetTransparency(double){}
extern "C" double WindUI_GetTransparency(){return 0;}
extern "C" void WindUI_GetWindowSize(int*w,int*h){*w=740;*h=490;}
extern "C" const char* WindUI_GetLockedIdsJSON(int){return "[]";}
extern "C" void WindUI_SetOpenButtonEnabled(int){}
extern "C" void WindUI_OnKeyValidation(int,WindUI_KeyValidator,void*){}
extern "C" void WindUI_InstallDemo();
int main(){WindUI_InstallDemo();}
