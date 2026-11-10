// WindUI for Bedrock: JNI + Android WindowManager/WebView overlay, ARM64.
// Adapted loading architecture from Zxwmwmwm/LumaUI (MIT).
// Not a Minecraft injector: no game symbols are invoked and no code is patched.
#include <jni.h>
#include <android/log.h>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <utility>
#include <map>
#include <unordered_map>
#include "../include/WindUI.h"

#include "embedded.hpp"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "WindUIBedrock", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "WindUIBedrock", __VA_ARGS__)

// Build-specific, VERIFIED diagnostic-only signature. Not used for patching or hooking.
// Source ELF: Minecraft 1.26.62.1 ARM64, BuildID c60fa32908019eecae39ebfe7bac5755dc35cde2
// File offset 0x07934C80, 64 bytes, 24 wildcards, exactly one match on supplied binary.
static const char kMinecraft126621DiagnosticSignature[] =
    "A1 ? ? 54 E8 02 40 F9 08 ? ? 8B E8 02 00 F9 "
    "? ? ? 17 E8 02 40 B9 ? ? ? 32 08 7D 1A 53 "
    "? ? 00 71 ? ? ? 54 E0 03 17 AA ? ? ? 95 "
    "01 00 13 CB E0 03 17 AA ? ? ? 95 ? ? ? 17";
extern "C" __attribute__((visibility("default")))
const char* WindUI_GetDiagnosticSignature() { return kMinecraft126621DiagnosticSignature; }

static JavaVM* g_vm = nullptr;
static jclass g_bridge = nullptr;
static jobject g_handler = nullptr;
static jobject g_runnable = nullptr;
static jobject g_activity = nullptr;
static jobject g_wm = nullptr;
static jobject g_bubble = nullptr;
static jobject g_wmParams = nullptr;
static jobject g_dialog = nullptr;
static jobject g_webview = nullptr;
static std::atomic<bool> g_enabled{false};
static std::atomic<bool> g_requestOpen{false};
static std::atomic<bool> g_webReady{false};
static int g_loadTicks=0;
static float g_downX=0, g_downY=0, g_startX=0, g_startY=0;
static bool g_dragged = false;
static std::mutex g_queueLock;
static std::vector<std::string> g_commands;
static std::vector<std::string> g_replay;
static std::vector<std::string> g_pendingTransient;
static bool g_needReplay=true;
static std::mutex g_callbacksLock;
static std::unordered_map<int,std::pair<WindUI_Callback,void*>> g_callbacks;
static std::atomic<int> g_windowWidth{740},g_windowHeight{490};
static std::string g_currentTheme="Dark";
static std::atomic<double> g_uiScale{1.0},g_transparency{0.0};
static std::unordered_map<int,bool> g_locked;
static bool g_lockAll=false;
using KeyValidator=int (*)(const char*,void*);
static std::unordered_map<int,std::pair<KeyValidator,void*>> g_keyValidators;
static std::atomic<bool> g_bubbleEnabled{true};
static std::vector<std::string> g_themes={"Dark","Light","Rose","Plant","Red","Indigo","Sky","Violet","Amber","Emerald","Midnight","Crimson","Monokai Pro","Cotton Candy","Mellowsi","Rainbow"};
extern "C" void WindUI_InstallDemo();
static std::atomic<int> g_nextId{10};
using UserCallback = void (*)(int id, const char* value, void* user);
static UserCallback g_userCallback=nullptr;
static void* g_userData=nullptr;

class JEnv {
    bool didAttach_=false;
public:
    JNIEnv* e=nullptr;
    JEnv() {
        if(!g_vm) return;
        if(g_vm->GetEnv(reinterpret_cast<void**>(&e),JNI_VERSION_1_6)!=JNI_OK) {
            if(g_vm->AttachCurrentThread(&e,nullptr)==JNI_OK) didAttach_=true;
        }
    }
    ~JEnv(){if(didAttach_&&g_vm)g_vm->DetachCurrentThread();}
    explicit operator bool()const{return e!=nullptr;}
};
static bool check(JNIEnv* e,const char* where){
    if(e&&e->ExceptionCheck()) {
        LOGE("JNI exception at %s",where);
        e->ExceptionDescribe();e->ExceptionClear();return true;
    }
    return false;
}
static jclass cls(JNIEnv* e,const char* name){jclass c=e->FindClass(name);if(check(e,name))return nullptr;return c;}
static jmethodID meth(JNIEnv* e,jclass c,const char* n,const char* sig) {
    if(!c)return nullptr;
    jmethodID m=e->GetMethodID(c,n,sig);if(check(e,n))return nullptr;return m;
}
static jmethodID smeth(JNIEnv* e,jclass c,const char* n,const char* sig) {
    if(!c)return nullptr;
    jmethodID m=e->GetStaticMethodID(c,n,sig);if(check(e,n))return nullptr;return m;
}
static jobject newBridge(JNIEnv* e,int id){if(!g_bridge)return nullptr;return e->NewObject(g_bridge,meth(e,g_bridge,"<init>","(I)V"),jint(id));}
static std::string utf(JNIEnv* e,jstring s){if(!s)return {};const char* v=e->GetStringUTFChars(s,nullptr);std::string ret=v?v:"";if(v)e->ReleaseStringUTFChars(s,v);return ret;}
static std::string jsonq(const std::string& s){
    std::string v="\"";v.reserve(s.size()+2);
    for(unsigned char c:s) {
        switch(c){case '\\':v+="\\\\";break;case '"':v+="\\\"";break;case '\n':v+="\\n";break;case '\r':v+="\\r";break;case '\t':v+="\\t";break;
            default: if(c<32){char buf[8];std::snprintf(buf,sizeof(buf),"\\u%04x",c);v+=buf;}else v+=char(c);break;
        }
    }
    return v+'"';
}
static void enqueue(const std::string& js){
    std::lock_guard<std::mutex> lk(g_queueLock);
    if(g_commands.size()<4000)g_commands.push_back(js);
    if(g_pendingTransient.size()<1000)g_pendingTransient.push_back(js);
}
static void enqueueCommand(const std::string& js,bool persistent){
    std::lock_guard<std::mutex> lk(g_queueLock);
    if(persistent && g_replay.size()<4000)g_replay.push_back(js);
    if(g_commands.size()<4000)g_commands.push_back(js);
    if(!persistent&&g_pendingTransient.size()<1000)g_pendingTransient.push_back(js);
}
static void js(JNIEnv* e,const std::string& code){
    if(!g_webview)return;
    jclass w=cls(e,"android/webkit/WebView");jmethodID m=meth(e,w,"evaluateJavascript","(Ljava/lang/String;Landroid/webkit/ValueCallback;)V");
    if(m){jstring src=e->NewStringUTF(code.c_str());e->CallVoidMethod(g_webview,m,src,nullptr);e->DeleteLocalRef(src);check(e,"evaluateJavascript");}
    if(w)e->DeleteLocalRef(w);
}
static void flushCommands(JNIEnv* e){
    if(!g_webview||!g_webReady)return;
    std::vector<std::string> batch;
    bool replay=false;
    {std::lock_guard<std::mutex> lk(g_queueLock);
       replay=g_needReplay;
       if(replay){batch=g_replay;batch.insert(batch.end(),g_pendingTransient.begin(),g_pendingTransient.end());g_needReplay=false;g_commands.clear();}
       else batch.swap(g_commands);
       g_pendingTransient.clear();
    }
    if(replay)js(e,"window.WindUIAPI&&WindUIAPI.reset();");
    for(const std::string& cmd:batch)js(e,cmd);
}
static void ref(JNIEnv* e,jobject& dst,jobject src){if(dst)e->DeleteGlobalRef(dst);dst=src?e->NewGlobalRef(src):nullptr;}
static void drop(JNIEnv* e,jobject& src){if(src)e->DeleteGlobalRef(src);src=nullptr;}
static jint dp(JNIEnv* e,jobject context,float size){
    jclass cc=cls(e,"android/content/Context");jmethodID res=meth(e,cc,"getResources","()Landroid/content/res/Resources;");
    jobject r=e->CallObjectMethod(context,res);
    jclass rr=cls(e,"android/content/res/Resources");jobject dm=e->CallObjectMethod(r,meth(e,rr,"getDisplayMetrics","()Landroid/util/DisplayMetrics;"));
    jclass dmc=cls(e,"android/util/DisplayMetrics");jfieldID field=e->GetFieldID(dmc,"density","F");
    float density=e->GetFloatField(dm,field);
    e->DeleteLocalRef(dm);e->DeleteLocalRef(r);e->DeleteLocalRef(dmc);e->DeleteLocalRef(rr);e->DeleteLocalRef(cc);
    return (jint)std::round(size*density);
}
static std::pair<int,int> screen(JNIEnv* e,jobject context){
    jclass cc=cls(e,"android/content/Context");jobject r=e->CallObjectMethod(context,meth(e,cc,"getResources","()Landroid/content/res/Resources;"));
    jclass rr=cls(e,"android/content/res/Resources");jobject dm=e->CallObjectMethod(r,meth(e,rr,"getDisplayMetrics","()Landroid/util/DisplayMetrics;"));
    jclass dd=cls(e,"android/util/DisplayMetrics");jfieldID wf=e->GetFieldID(dd,"widthPixels","I"),hf=e->GetFieldID(dd,"heightPixels","I");
    int w=e->GetIntField(dm,wf),h=e->GetIntField(dm,hf);
    e->DeleteLocalRef(dd);e->DeleteLocalRef(dm);e->DeleteLocalRef(rr);e->DeleteLocalRef(r);e->DeleteLocalRef(cc);
    return {w,h};
}
static void detach(JNIEnv* e){
    g_webReady=false;g_loadTicks=0; {std::lock_guard<std::mutex> lk(g_queueLock);g_needReplay=true;}
    if(g_dialog){
        jclass d=cls(e,"android/app/Dialog");jmethodID m=meth(e,d,"dismiss","()V");if(m)e->CallVoidMethod(g_dialog,m);check(e,"dismiss");e->DeleteLocalRef(d);
    }
    if(g_wm&&g_bubble){
        jclass w=cls(e,"android/view/WindowManager");jmethodID m=meth(e,w,"removeViewImmediate","(Landroid/view/View;)V");if(m)e->CallVoidMethod(g_wm,m,g_bubble);check(e,"remove bubble");e->DeleteLocalRef(w);
    }
    if(g_webview){jclass w=cls(e,"android/webkit/WebView");jmethodID m=meth(e,w,"destroy","()V");if(m)e->CallVoidMethod(g_webview,m);check(e,"destroy WebView");e->DeleteLocalRef(w);}
    drop(e,g_webview);drop(e,g_dialog);drop(e,g_bubble);drop(e,g_wmParams);drop(e,g_wm);drop(e,g_activity);
}
static jobject getActivity(JNIEnv* e){
    // Launcher-specific bridge matching the original LumaUI architecture.
    jclass m=cls(e,"org/levimc/launcher/core/mods/inbuilt/overlay/InbuiltOverlayManager");if(!m)return nullptr;
    jmethodID get=smeth(e,m,"getInstance","()Lorg/levimc/launcher/core/mods/inbuilt/overlay/InbuiltOverlayManager;");
    if(!get){e->DeleteLocalRef(m);return nullptr;}
    jobject inst=e->CallStaticObjectMethod(m,get);
    if(check(e,"getInstance")||!inst){e->DeleteLocalRef(m);return nullptr;}
    jclass klass=cls(e,"java/lang/Class");jstring actName=e->NewStringUTF("activity");
    jobject field=e->CallObjectMethod(m,meth(e,klass,"getDeclaredField","(Ljava/lang/String;)Ljava/lang/reflect/Field;"),actName);
    e->DeleteLocalRef(actName);
    if(check(e,"activity field")||!field){e->DeleteLocalRef(inst);e->DeleteLocalRef(m);e->DeleteLocalRef(klass);return nullptr;}
    jclass fieldcls=cls(e,"java/lang/reflect/Field");
    e->CallVoidMethod(field,meth(e,fieldcls,"setAccessible","(Z)V"),JNI_TRUE);
    jobject result=e->CallObjectMethod(field,meth(e,fieldcls,"get","(Ljava/lang/Object;)Ljava/lang/Object;"),inst);
    if(check(e,"activity.get"))result=nullptr;
    e->DeleteLocalRef(fieldcls);e->DeleteLocalRef(field);e->DeleteLocalRef(klass);e->DeleteLocalRef(inst);e->DeleteLocalRef(m);
    return result;
}
static bool attach(JNIEnv* e,jobject act){
    detach(e);
    if(!act)return false;
    jclass activity=cls(e,"android/app/Activity");
    jboolean finishing=e->CallBooleanMethod(act,meth(e,activity,"isFinishing","()Z"));
    if(finishing){e->DeleteLocalRef(activity);return false;}
    ref(e,g_activity,act);
    jobject wm=e->CallObjectMethod(act,meth(e,activity,"getWindowManager","()Landroid/view/WindowManager;"));
    if(check(e,"getWindowManager")||!wm){e->DeleteLocalRef(activity);detach(e);return false;}
    ref(e,g_wm,wm);
    jclass windowCls=cls(e,"android/view/Window");jobject window=e->CallObjectMethod(act,meth(e,activity,"getWindow","()Landroid/view/Window;"));
    jobject decor=e->CallObjectMethod(window,meth(e,windowCls,"getDecorView","()Landroid/view/View;"));
    jclass view=cls(e,"android/view/View");
    jobject token=e->CallObjectMethod(decor,meth(e,view,"getWindowToken","()Landroid/os/IBinder;"));
    if(check(e,"windowToken")||!token){
        if(decor)e->DeleteLocalRef(decor);if(window)e->DeleteLocalRef(window);if(windowCls)e->DeleteLocalRef(windowCls);
        e->DeleteLocalRef(view);e->DeleteLocalRef(activity);e->DeleteLocalRef(wm);detach(e);return false;
    }
    jclass tv=cls(e,"android/widget/TextView");jobject bubble=e->NewObject(tv,meth(e,tv,"<init>","(Landroid/content/Context;)V"),act);
    jstring label=e->NewStringUTF("W");e->CallVoidMethod(bubble,meth(e,tv,"setText","(Ljava/lang/CharSequence;)V"),label);e->DeleteLocalRef(label);
    e->CallVoidMethod(bubble,meth(e,tv,"setTextColor","(I)V"),0xfff5f5f7);
    e->CallVoidMethod(bubble,meth(e,tv,"setTextSize","(F)V"),19.f);
    e->CallVoidMethod(bubble,meth(e,tv,"setGravity","(I)V"),17);
    jclass draw=cls(e,"android/graphics/drawable/GradientDrawable");
    jobject bg=e->NewObject(draw,meth(e,draw,"<init>","()V"));
    e->CallVoidMethod(bg,meth(e,draw,"setColor","(I)V"),0xff151923);
    e->CallVoidMethod(bg,meth(e,draw,"setCornerRadius","(F)V"),float(dp(e,act,28)));
    e->CallVoidMethod(bg,meth(e,draw,"setStroke","(II)V"),dp(e,act,1),0xff438cf4);
    e->CallVoidMethod(bubble,meth(e,view,"setBackground","(Landroid/graphics/drawable/Drawable;)V"),bg);
    jobject listener=newBridge(e,1);
    e->CallVoidMethod(bubble,meth(e,view,"setOnTouchListener","(Landroid/view/View$OnTouchListener;)V"),listener);
    e->CallVoidMethod(bubble,meth(e,view,"setOnClickListener","(Landroid/view/View$OnClickListener;)V"),listener);
    e->CallVoidMethod(bubble,meth(e,view,"setContentDescription","(Ljava/lang/CharSequence;)V"),e->NewStringUTF("Open WindUI"));
    jclass lp=cls(e,"android/view/WindowManager$LayoutParams");
    const int bsize=dp(e,act,52);
    jobject params=e->NewObject(lp,meth(e,lp,"<init>","(IIIII)V"),bsize,bsize,1000,0x28,-3);
    jfieldID tokenField=e->GetFieldID(lp,"token","Landroid/os/IBinder;");e->SetObjectField(params,tokenField,token);
    jfieldID gravityField=e->GetFieldID(lp,"gravity","I");e->SetIntField(params,gravityField,0x33);
    jfieldID xField=e->GetFieldID(lp,"x","I"),yField=e->GetFieldID(lp,"y","I");
    const auto wh=screen(e,act);
    e->SetIntField(params,xField,std::max(0,wh.first-bsize-dp(e,act,14)));
    e->SetIntField(params,yField,std::max(0,wh.second/2-bsize/2));
    jclass wmCls=cls(e,"android/view/WindowManager");
    e->CallVoidMethod(wm,meth(e,wmCls,"addView","(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V"),bubble,params);
    bool ok=!check(e,"WindowManager.addView");
    if(ok){ref(e,g_bubble,bubble);ref(e,g_wmParams,params);LOGI("WindowManager bubble attached");}
    e->DeleteLocalRef(wmCls);e->DeleteLocalRef(params);e->DeleteLocalRef(lp);e->DeleteLocalRef(listener);e->DeleteLocalRef(bg);e->DeleteLocalRef(draw);e->DeleteLocalRef(bubble);e->DeleteLocalRef(tv);e->DeleteLocalRef(token);e->DeleteLocalRef(decor);e->DeleteLocalRef(window);e->DeleteLocalRef(windowCls);e->DeleteLocalRef(view);e->DeleteLocalRef(activity);e->DeleteLocalRef(wm);
    if(!ok)detach(e);
    return ok;
}
static void show(JNIEnv* e){
    if(!g_activity||!g_bubble)return;
    if(g_dialog){jclass d=cls(e,"android/app/Dialog");jmethodID m=meth(e,d,"show","()V");if(m)e->CallVoidMethod(g_dialog,m);check(e,"dialog.show");e->DeleteLocalRef(d);return;}
    g_webReady=false;g_loadTicks=0;{std::lock_guard<std::mutex> lk(g_queueLock);g_needReplay=true;}
    jclass d=cls(e,"android/app/Dialog");jobject dialog=e->NewObject(d,meth(e,d,"<init>","(Landroid/content/Context;)V"),g_activity);
    if(check(e,"Dialog.new")||!dialog)return;
    e->CallBooleanMethod(dialog,meth(e,d,"requestWindowFeature","(I)Z"),1);
    jclass web=cls(e,"android/webkit/WebView");jobject view=e->NewObject(web,meth(e,web,"<init>","(Landroid/content/Context;)V"),g_activity);
    if(check(e,"WebView.new")||!view){e->DeleteLocalRef(dialog);return;}
    jobject settings=e->CallObjectMethod(view,meth(e,web,"getSettings","()Landroid/webkit/WebSettings;"));
    jclass ws=cls(e,"android/webkit/WebSettings");
    e->CallVoidMethod(settings,meth(e,ws,"setJavaScriptEnabled","(Z)V"),JNI_TRUE);
    e->CallVoidMethod(settings,meth(e,ws,"setDomStorageEnabled","(Z)V"),JNI_TRUE);
    e->CallVoidMethod(settings,meth(e,ws,"setAllowFileAccess","(Z)V"),JNI_FALSE);
    e->CallVoidMethod(settings,meth(e,ws,"setAllowContentAccess","(Z)V"),JNI_FALSE);
    jobject client=newBridge(e,3);e->CallVoidMethod(view,meth(e,web,"setWebViewClient","(Landroid/webkit/WebViewClient;)V"),client);
    e->CallVoidMethod(view,meth(e,web,"setBackgroundColor","(I)V"),0x00000000);
    jstring base=e->NewStringUTF("https://windui.local/");jstring html=e->NewStringUTF(kWindUiHtml);
    jstring mime=e->NewStringUTF("text/html"),encoding=e->NewStringUTF("UTF-8");
    e->CallVoidMethod(view,meth(e,web,"loadDataWithBaseURL","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"),base,html,mime,encoding,nullptr);
    if(check(e,"loadDataWithBaseURL")){e->DeleteLocalRef(dialog);e->DeleteLocalRef(view);return;}
    e->CallVoidMethod(dialog,meth(e,d,"setContentView","(Landroid/view/View;)V"),view);
    e->CallVoidMethod(dialog,meth(e,d,"setCanceledOnTouchOutside","(Z)V"),JNI_TRUE);
    e->CallVoidMethod(dialog,meth(e,d,"show","()V"));
    jobject window=e->CallObjectMethod(dialog,meth(e,d,"getWindow","()Landroid/view/Window;"));
    if(window){
        jclass wc=cls(e,"android/view/Window");
        e->CallVoidMethod(window,meth(e,wc,"setBackgroundDrawableResource","(I)V"),17170445);
        auto wh=screen(e,g_activity);int maxw=std::max(dp(e,g_activity,230),wh.first-dp(e,g_activity,20)),maxh=std::max(dp(e,g_activity,220),wh.second-dp(e,g_activity,26));
        e->CallVoidMethod(window,meth(e,wc,"setLayout","(II)V"),std::min(dp(e,g_activity,float(g_windowWidth.load())),maxw),std::min(dp(e,g_activity,float(g_windowHeight.load())),maxh));
        e->DeleteLocalRef(wc);e->DeleteLocalRef(window);
    }
    if(check(e,"Dialog.show")){e->DeleteLocalRef(dialog);e->DeleteLocalRef(view);return;}
    ref(e,g_dialog,dialog);ref(e,g_webview,view);
    e->DeleteLocalRef(encoding);e->DeleteLocalRef(mime);e->DeleteLocalRef(html);e->DeleteLocalRef(base);e->DeleteLocalRef(client);e->DeleteLocalRef(ws);e->DeleteLocalRef(settings);e->DeleteLocalRef(view);e->DeleteLocalRef(web);e->DeleteLocalRef(dialog);e->DeleteLocalRef(d);
    LOGI("WindUI window shown");
}
static void hide(JNIEnv* e){if(!g_dialog)return;jclass d=cls(e,"android/app/Dialog");jmethodID mid=meth(e,d,"dismiss","()V");if(mid)e->CallVoidMethod(g_dialog,mid);check(e,"dismiss");e->DeleteLocalRef(d);}
static std::string decode(const std::string& s){std::string t;for(size_t i=0;i<s.size();++i){if(s[i]=='%'&&i+2<s.size()) {char tmp[3]={s[i+1],s[i+2],0};char* end=nullptr;long val=strtol(tmp,&end,16);if(end==tmp+2){t.push_back(char(val));i+=2;continue;}}t.push_back(s[i]=='+'?' ':s[i]);}return t;}
static std::string queryParam(const std::string& s,const char* name){std::string prefix=std::string(name)+"=";size_t n=s.find('?');if(n==std::string::npos)return {};for(size_t i=n+1;i<s.size();) {size_t end=s.find('&',i);if(end==std::string::npos)end=s.size();if(s.compare(i,prefix.size(),prefix)==0)return decode(s.substr(i+prefix.size(),end-i-prefix.size()));i=end+1;}return {};}
static void nativeRun(JNIEnv* e,jclass,jint type){
    if(type==2){detach(e);return;}
    if(type==4){hide(e);return;}
    if(type==5){if(g_bubble){jclass v=cls(e,"android/view/View");if(v){e->CallVoidMethod(g_bubble,meth(e,v,"setVisibility","(I)V"),g_bubbleEnabled?0:8);e->DeleteLocalRef(v);}}return;}
    if(type==6){if(g_dialog&&g_activity){jclass d=cls(e,"android/app/Dialog");jobject win=e->CallObjectMethod(g_dialog,meth(e,d,"getWindow","()Landroid/view/Window;"));if(win){jclass wc=cls(e,"android/view/Window");auto wh=screen(e,g_activity);int w=std::min(dp(e,g_activity,float(g_windowWidth.load())),std::max(dp(e,g_activity,250),wh.first-dp(e,g_activity,12)));int h=std::min(dp(e,g_activity,float(g_windowHeight.load())),std::max(dp(e,g_activity,220),wh.second-dp(e,g_activity,16)));e->CallVoidMethod(win,meth(e,wc,"setLayout","(II)V"),w,h);e->DeleteLocalRef(wc);e->DeleteLocalRef(win);}e->DeleteLocalRef(d);}return;}
    if(!g_enabled)return;
    jobject act=getActivity(e);
    if(act){if(!g_activity||!e->IsSameObject(g_activity,act))attach(e,act);e->DeleteLocalRef(act);}
    if(g_requestOpen.exchange(false)&&g_activity)show(e);
    // Fallback for WebView versions that do not intercept custom-scheme iframes.
    if(g_webview&&!g_webReady&&++g_loadTicks>=7)g_webReady=true;
    flushCommands(e);
    if(g_handler&&g_runnable){jclass handler=cls(e,"android/os/Handler");e->CallBooleanMethod(g_handler,meth(e,handler,"postDelayed","(Ljava/lang/Runnable;J)Z"),g_runnable,(jlong)450);check(e,"postDelayed");e->DeleteLocalRef(handler);}
}
static void nativeClick(JNIEnv* e,jclass,jint id,jobject){if(id==1)show(e);}
static jboolean nativeTouch(JNIEnv* e,jclass,jint id,jobject view,jobject event){
    if(id!=1||!g_wmParams||!g_wm)return JNI_FALSE;
    jclass me=cls(e,"android/view/MotionEvent");int action=e->CallIntMethod(event,meth(e,me,"getActionMasked","()I"));
    float x=e->CallFloatMethod(event,meth(e,me,"getRawX","()F")),y=e->CallFloatMethod(event,meth(e,me,"getRawY","()F"));
    jclass lp=cls(e,"android/view/WindowManager$LayoutParams");
    jfieldID xf=e->GetFieldID(lp,"x","I"),yf=e->GetFieldID(lp,"y","I");
    if(action==0){g_downX=x;g_downY=y;g_startX=e->GetIntField(g_wmParams,xf);g_startY=e->GetIntField(g_wmParams,yf);g_dragged=false;}
    else if(action==2){float dx=x-g_downX,dy=y-g_downY;
        if(!g_dragged&&(dx*dx+dy*dy)>float(dp(e,g_activity,7)*dp(e,g_activity,7)))g_dragged=true;
        if(g_dragged){auto wh=screen(e,g_activity);int sz=dp(e,g_activity,52);
            e->SetIntField(g_wmParams,xf,std::max(0,std::min(wh.first-sz,int(g_startX+dx))));
            e->SetIntField(g_wmParams,yf,std::max(0,std::min(wh.second-sz,int(g_startY+dy))));
            jclass w=cls(e,"android/view/WindowManager");e->CallVoidMethod(g_wm,meth(e,w,"updateViewLayout","(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V"),g_bubble,g_wmParams);check(e,"bubble drag");e->DeleteLocalRef(w);
        }
    } else if(action==1&&!g_dragged){jclass v=cls(e,"android/view/View");e->CallBooleanMethod(view,meth(e,v,"performClick","()Z"));e->DeleteLocalRef(v);}
    e->DeleteLocalRef(lp);e->DeleteLocalRef(me);return JNI_TRUE;
}
static jboolean nativeUrl(JNIEnv* e,jclass,jint,jobject,jstring jurl){
    std::string url=utf(e,jurl);
    if(url=="windui://ready"){g_webReady=true;flushCommands(e);return JNI_TRUE;}
    if(url=="windui://close"){hide(e);return JNI_TRUE;}
    if(url.rfind("windui://event?",0)==0){
        const auto id=queryParam(url,"id"),value=queryParam(url,"value");
        if(id.size()<16&&value.size()<8192){
            int numeric=std::atoi(id.c_str());WindUI_Callback cb=nullptr;void* data=nullptr;
            {std::lock_guard<std::mutex> lk(g_callbacksLock);
               auto it=g_callbacks.find(numeric);
               if(it!=g_callbacks.end()){cb=it->second.first;data=it->second.second;}
            }
            if(numeric<0 && numeric%10==-4){
               // C++ key-validation callback: asynchronous response to WebView.
               int win=(-numeric-4)/10;KeyValidator validator=nullptr;void* opaque=nullptr;
               {std::lock_guard<std::mutex> lk(g_callbacksLock);auto it=g_keyValidators.find(win);if(it!=g_keyValidators.end()){validator=it->second.first;opaque=it->second.second;}}
               if(validator){bool allowed=validator(value.c_str(),opaque)!=0;enqueue("window.WindUIAPI&&WindUIAPI.dispatch(\"key.validate\",{allowed:"+std::string(allowed?"true":"false")+"});");}
            }
            if(cb)cb(numeric,value.c_str(),data);
            if(g_userCallback)g_userCallback(numeric,value.c_str(),g_userData);
        }
        return JNI_TRUE;
    }
    // All other link navigations are deliberately blocked (no external network UI).
    return JNI_TRUE;
}
static bool postRun(int id){JEnv env;if(!env||!g_handler)return false;JNIEnv* e=env.e;jobject r=newBridge(e,id);jclass h=cls(e,"android/os/Handler");jmethodID m=meth(e,h,"post","(Ljava/lang/Runnable;)Z");jboolean done=m?e->CallBooleanMethod(g_handler,m,r):JNI_FALSE;check(e,"handler.post");if(h)e->DeleteLocalRef(h);if(r)e->DeleteLocalRef(r);return done;}

extern "C" __attribute__((visibility("default"))) bool PLMod_Load(JavaVM* vm,const void*){
    g_vm=vm;JEnv env;if(!env)return false;JNIEnv* e=env.e;
    jclass manager=cls(e,"org/levimc/launcher/core/mods/inbuilt/overlay/InbuiltOverlayManager");if(!manager)return false;
    jclass classCls=cls(e,"java/lang/Class");
    jobject parent=e->CallObjectMethod(manager,meth(e,classCls,"getClassLoader","()Ljava/lang/ClassLoader;"));
    if(check(e,"class loader")||!parent)return false;
    jclass dexcls=cls(e,"dalvik/system/InMemoryDexClassLoader");if(!dexcls)return false;
    jobject data=e->NewDirectByteBuffer(const_cast<unsigned char*>(kWindUiDex),kWindUiDexSize);
    jobject loader=e->NewObject(dexcls,meth(e,dexcls,"<init>","(Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V"),data,parent);
    if(check(e,"DexClassLoader")||!loader)return false;
    jstring name=e->NewStringUTF("windui.Bridge");
    jobject bridge=e->CallObjectMethod(loader,meth(e,dexcls,"loadClass","(Ljava/lang/String;)Ljava/lang/Class;"),name);
    if(check(e,"load bridge")||!bridge)return false;
    g_bridge=(jclass)e->NewGlobalRef(bridge);
    static JNINativeMethod methods[]={
        {const_cast<char*>("nativeRun"),const_cast<char*>("(I)V"),reinterpret_cast<void*>(nativeRun)},
        {const_cast<char*>("nativeClick"),const_cast<char*>("(ILandroid/view/View;)V"),reinterpret_cast<void*>(nativeClick)},
        {const_cast<char*>("nativeTouch"),const_cast<char*>("(ILandroid/view/View;Landroid/view/MotionEvent;)Z"),reinterpret_cast<void*>(nativeTouch)},
        {const_cast<char*>("nativeUrl"),const_cast<char*>("(ILandroid/webkit/WebView;Ljava/lang/String;)Z"),reinterpret_cast<void*>(nativeUrl)},
    };
    if(e->RegisterNatives(g_bridge,methods,4)<0||check(e,"RegisterNatives"))return false;
    jclass loop=cls(e,"android/os/Looper");jobject mainLooper=e->CallStaticObjectMethod(loop,smeth(e,loop,"getMainLooper","()Landroid/os/Looper;"));
    jclass h=cls(e,"android/os/Handler");jobject handler=e->NewObject(h,meth(e,h,"<init>","(Landroid/os/Looper;)V"),mainLooper);
    if(check(e,"Handler")||!handler)return false;
    ref(e,g_handler,handler);jobject runnable=newBridge(e,0);ref(e,g_runnable,runnable);
    e->DeleteLocalRef(runnable);e->DeleteLocalRef(handler);e->DeleteLocalRef(h);e->DeleteLocalRef(mainLooper);e->DeleteLocalRef(loop);e->DeleteLocalRef(bridge);e->DeleteLocalRef(name);e->DeleteLocalRef(loader);e->DeleteLocalRef(data);e->DeleteLocalRef(dexcls);e->DeleteLocalRef(parent);e->DeleteLocalRef(classCls);e->DeleteLocalRef(manager);
    LOGI("WindUI JNI bridge loaded, DEX bytes=%zu",size_t(kWindUiDexSize));
    return true;
}
extern "C" __attribute__((visibility("default"))) bool PLMod_Enable(){g_enabled=true;WindUI_InstallDemo();return postRun(0);}
extern "C" __attribute__((visibility("default"))) bool PLMod_Disable(){g_enabled=false;return postRun(2);}
extern "C" __attribute__((visibility("default"))) bool PLMod_Unload(){g_enabled=false;postRun(2);return true;}

// Native extension API. Other ARM64 mods can resolve these symbols with dlsym.
extern "C" __attribute__((visibility("default"))) void WindUI_RegisterCallback(UserCallback cb,void* userdata){g_userCallback=cb;g_userData=userdata;}
extern "C" __attribute__((visibility("default"))) void WindUI_SetTitle(const char* title){if(title)enqueueCommand("window.WindUIAPI&&WindUIAPI.dispatch(\"window.update\",{changes:{Title:"+jsonq(title)+"}});",true);}
extern "C" __attribute__((visibility("default"))) void WindUI_SetTheme(const char* theme){if(theme){ {std::lock_guard<std::mutex> lk(g_queueLock);g_currentTheme=theme;} enqueueCommand("window.WindUIAPI&&WindUIAPI.dispatch(\"theme.set\",{Name:"+jsonq(theme)+"});",true);}}
extern "C" __attribute__((visibility("default"))) void WindUI_Notify(const char* title,const char* body){enqueue("window.WindUI&&WindUI.notify("+jsonq(title?title:"")+","+jsonq(body?body:"")+");");}
extern "C" __attribute__((visibility("default"))) void WindUI_Open(){g_requestOpen=true;}
extern "C" __attribute__((visibility("default"))) int WindUI_IsReady(){return g_enabled&&g_bubble?1:0;}
extern "C" __attribute__((visibility("default"))) void WindUI_AddTab(const char* name,const char* icon){
    if(name)enqueue("window.WindUI&&WindUI.addTab("+jsonq(name)+","+jsonq(icon?icon:"layers")+");");
}
static int addElement(const char* tab,const char* type,const char* title,const char* desc,const std::string& value,int min,int max,int step,const std::string& options="[]"){
    if(!tab||!type||!title)return -1;int id=g_nextId++;
    enqueue("window.WindUI&&WindUI.addElement("+jsonq(tab)+","+jsonq(type)+","+jsonq(title)+","+jsonq(desc?desc:"")+","+std::to_string(id)+","+value+","+std::to_string(min)+","+std::to_string(max)+","+std::to_string(step)+","+options+");");
    return id;
}
extern "C" __attribute__((visibility("default"))) int WindUI_AddButton(const char* tab,const char* title,const char* desc){return addElement(tab,"button",title,desc,"0",0,100,1);}
extern "C" __attribute__((visibility("default"))) int WindUI_AddToggle(const char* tab,const char* title,const char* desc,int on){return addElement(tab,"toggle",title,desc,on?"true":"false",0,100,1);}
extern "C" __attribute__((visibility("default"))) int WindUI_AddSlider(const char* tab,const char* title,const char* desc,int min,int max,int value,int step){return addElement(tab,"slider",title,desc,std::to_string(value),min,max,step);}
extern "C" __attribute__((visibility("default"))) int WindUI_AddInput(const char* tab,const char* title,const char* desc){return addElement(tab,"input",title,desc,"0",0,0,1);}
extern "C" __attribute__((visibility("default"))) int WindUI_AddDropdown(const char* tab,const char* title,const char* desc,const char* jsonOptions){
    // Caller-supplied options must be a JSON array, e.g. ["One","Two"].
    std::string opts=jsonOptions?jsonOptions:"[]";
    if(opts.size()>4096||opts.empty()||opts[0]!='[')opts="[]";
    return addElement(tab,"dropdown",title,desc,"0",0,0,1,opts);
}

// v2 native API. Commands and their authoritative replay log are C++ owned.
// The WebView only turns declarative, JSON-serialized state into interactive views.
extern "C" __attribute__((visibility("default"))) int WindUI_NextId(){return g_nextId.fetch_add(1);}
extern "C" __attribute__((visibility("default"))) int WindUI_Command(const char* method,const char* payload,int persistent){
 if(!method||!payload)return 0;
 std::string op(method),json(payload);
 if(op.size()>90||json.size()>262144||json.empty())return 0;
 for(char ch:op)if(!((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||ch=='.'))return 0;
 const std::string code="window.WindUIAPI&&WindUIAPI.dispatch("+jsonq(op)+",JSON.parse("+jsonq(json)+"));";
 // Simple C++ state mirror for synchronous getters. All object changes still
 // flow through the JSON model and are applied by the renderer.
 auto getId=[&]()->int{size_t at=json.find("\"id\"");if(at==std::string::npos)return -1;at=json.find(':',at);return at==std::string::npos?-1:std::atoi(json.c_str()+at+1);};
 auto boolField=[&](const char* key,bool fallback)->bool{std::string k="\""+std::string(key)+"\"";size_t at=json.find(k);if(at==std::string::npos)return fallback;at=json.find(':',at);if(at==std::string::npos)return fallback;std::string sub=json.substr(at+1,16);return sub.find("true")!=std::string::npos;};
 if(op=="window.create"){std::lock_guard<std::mutex> lk(g_callbacksLock);g_locked.clear();g_lockAll=false;}
 if(op=="element.create"){int id=getId();if(id>0){std::lock_guard<std::mutex> lk(g_callbacksLock);g_locked[id]=boolField("Locked",false);}}
 if(op=="element.update"){int id=getId();if(id>0&&json.find("\"Locked\"")!=std::string::npos){std::lock_guard<std::mutex> lk(g_callbacksLock);g_locked[id]=boolField("Locked",false);}}
 if(op=="element.action"&&json.find("destroy")!=std::string::npos){std::lock_guard<std::mutex> lk(g_callbacksLock);g_locked.erase(getId());}
 if(op=="window.action"){if(json.find("lockAll")!=std::string::npos)g_lockAll=true;else if(json.find("unlockAll")!=std::string::npos)g_lockAll=false;}
 enqueueCommand(code,persistent!=0);
 // Persist theme metadata in C++ for synchronous getters.
 if(op=="theme.add"){
   auto k=json.find("\"Name\"");if(k!=std::string::npos){k=json.find(':',k);if(k!=std::string::npos){k=json.find('\"',k);if(k!=std::string::npos){auto end=json.find('\"',k+1);if(end!=std::string::npos){std::string n=json.substr(k+1,end-k-1);std::lock_guard<std::mutex> lk(g_queueLock);if(!n.empty()&&std::find(g_themes.begin(),g_themes.end(),n)==g_themes.end())g_themes.push_back(n);}}}}
 }
 return 1;
}
extern "C" __attribute__((visibility("default"))) void WindUI_On(int id,WindUI_Callback cb,void* userdata){
 std::lock_guard<std::mutex> lk(g_callbacksLock);
 if(cb)g_callbacks[id]={cb,userdata};else g_callbacks.erase(id);
}
extern "C" __attribute__((visibility("default"))) void WindUI_Off(int id){WindUI_On(id,nullptr,nullptr);}
extern "C" __attribute__((visibility("default"))) void WindUI_SetOverlaySize(int w,int h){
 g_windowWidth=std::max(300,std::min(1200,w));g_windowHeight=std::max(250,std::min(900,h));postRun(6);
}
extern "C" __attribute__((visibility("default"))) void WindUI_Close(){postRun(4);}
extern "C" __attribute__((visibility("default"))) const char* WindUI_GetCurrentTheme(){
 static thread_local std::string s;
 {std::lock_guard<std::mutex> lk(g_queueLock);s=g_currentTheme;}
 return s.c_str();
}
extern "C" __attribute__((visibility("default"))) const char* WindUI_GetThemesJSON(){
 static thread_local std::string s;
 std::lock_guard<std::mutex> lk(g_queueLock);
 s="[";for(const auto& t:g_themes){if(s.size()>1)s+=',';s+=jsonq(t);}s+="]";
 return s.c_str();
}

extern "C" __attribute__((visibility("default"))) void WindUI_SetUIScale(double value){g_uiScale=std::max(.6,std::min(1.4,value));}
extern "C" __attribute__((visibility("default"))) double WindUI_GetUIScale(){return g_uiScale.load();}
extern "C" __attribute__((visibility("default"))) void WindUI_SetTransparency(double value){g_transparency=std::max(0.0,std::min(.85,value));}
extern "C" __attribute__((visibility("default"))) double WindUI_GetTransparency(){return g_transparency.load();}
extern "C" __attribute__((visibility("default"))) void WindUI_GetWindowSize(int* width,int* height){if(width)*width=g_windowWidth.load();if(height)*height=g_windowHeight.load();}
extern "C" __attribute__((visibility("default"))) const char* WindUI_GetLockedIdsJSON(int locked){
 static thread_local std::string out;
 std::lock_guard<std::mutex> lk(g_callbacksLock);
 out="[";for(auto &it:g_locked){bool state=g_lockAll||it.second;if(state==(locked!=0)){if(out.size()>1)out+=',';out+=std::to_string(it.first);}}out+="]";return out.c_str();
}
extern "C" __attribute__((visibility("default"))) void WindUI_OnKeyValidation(int windowId,KeyValidator cb,void* userdata){
 std::lock_guard<std::mutex> lk(g_callbacksLock);
 if(cb)g_keyValidators[windowId]={cb,userdata};else g_keyValidators.erase(windowId);
}
extern "C" __attribute__((visibility("default"))) void WindUI_SetOpenButtonEnabled(int visible){g_bubbleEnabled=(visible!=0);postRun(5);}
