// Example: how another C++ native mod can use WindUI without linking at build time.
// The two mods must already be loaded into a shared linker namespace with their
// public symbols visible (or use dlopen on the actual installed WindUI .so path).
#include <dlfcn.h>
#include <android/log.h>
#include <cstdio>
#include "../include/WindUI.h"
using AddTab=decltype(&WindUI_AddTab);
using AddToggle=decltype(&WindUI_AddToggle);
using Register=decltype(&WindUI_RegisterCallback);
static void onEvent(int id,const char* value,void*){
  __android_log_print(ANDROID_LOG_INFO,"WindUIExtension","Control %d = %s",id,value?value:"");
}
extern "C" void exampleAttach(){
  auto addTab=reinterpret_cast<AddTab>(dlsym(RTLD_DEFAULT,"WindUI_AddTab"));
  auto addToggle=reinterpret_cast<AddToggle>(dlsym(RTLD_DEFAULT,"WindUI_AddToggle"));
  auto reg=reinterpret_cast<Register>(dlsym(RTLD_DEFAULT,"WindUI_RegisterCallback"));
  if(!addTab||!addToggle||!reg)return;
  reg(onEvent,nullptr);
  addTab("My Native Mod","layers");
  addToggle("My Native Mod","Example toggle","This sends callbacks to native code",1);
}
