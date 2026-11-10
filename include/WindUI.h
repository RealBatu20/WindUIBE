#ifndef WINDUI_BEDROCK_H
#define WINDUI_BEDROCK_H
// WindUI Bedrock native ABI v2.0 — C ABI callable from independent ARM64 plugins.
// Resolve with dlsym(RTLD_DEFAULT, ...); no Roblox/Lua runtime is present.
#ifdef __cplusplus
extern "C" {
#endif
typedef void (*WindUI_Callback)(int elementId, const char* value, void* userdata);
const char* WindUI_GetDiagnosticSignature(void); // diagnostic only, never patched
int WindUI_IsReady(void);
int WindUI_NextId(void);
int WindUI_Command(const char* method, const char* payloadJson, int persistent);
void WindUI_RegisterCallback(WindUI_Callback callback, void* userdata); // legacy global callback
void WindUI_On(int id, WindUI_Callback callback, void* userdata);
void WindUI_Off(int id);
void WindUI_SetOverlaySize(int widthDp, int heightDp);
void WindUI_SetOpenButtonEnabled(int enabled);
typedef int (*WindUI_KeyValidator)(const char* enteredKey, void* userdata);
void WindUI_OnKeyValidation(int windowId,WindUI_KeyValidator callback,void* userdata);
void WindUI_Open(void);
void WindUI_Close(void);
void WindUI_SetUIScale(double value);
double WindUI_GetUIScale(void);
void WindUI_SetTransparency(double value);
double WindUI_GetTransparency(void);
void WindUI_GetWindowSize(int* widthDp,int* heightDp);
const char* WindUI_GetLockedIdsJSON(int getLocked);
const char* WindUI_GetCurrentTheme(void);
const char* WindUI_GetThemesJSON(void);
// Legacy extension ABI, retained for v1 native modules.
void WindUI_SetTitle(const char* title);
void WindUI_SetTheme(const char* name);
void WindUI_Notify(const char* title, const char* body);
void WindUI_AddTab(const char* name, const char* icon);
int WindUI_AddButton(const char* tab, const char* title, const char* desc);
int WindUI_AddToggle(const char* tab, const char* title, const char* desc, int enabled);
int WindUI_AddSlider(const char* tab, const char* title, const char* desc,int min,int max,int value,int step);
int WindUI_AddInput(const char* tab,const char* title,const char* desc);
int WindUI_AddDropdown(const char* tab,const char* title,const char* desc,const char* optionsJson);
#ifdef __cplusplus
}
#endif
#endif
