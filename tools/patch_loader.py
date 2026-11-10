from pathlib import Path
p=Path(__file__).resolve().parent.parent/'src/loader.cpp'
s=p.read_text()
s=s.replace('#include <utility>','#include <utility>\n#include <map>\n#include <unordered_map>\n#include "../include/WindUI.h"')
s=s.replace('static std::vector<std::string> g_commands;', '''static std::vector<std::string> g_commands;
static std::vector<std::string> g_replay;
static bool g_needReplay=true;
static std::mutex g_callbacksLock;
static std::unordered_map<int,std::pair<WindUI_Callback,void*>> g_callbacks;
static std::atomic<int> g_windowWidth{740},g_windowHeight{490};
static std::string g_currentTheme="Dark";
static std::vector<std::string> g_themes={"Dark","Light","Rose","Plant","Red","Indigo","Sky","Violet","Amber","Emerald","Midnight","Crimson","Monokai Pro","Cotton Candy","Mellowsi","Rainbow"};
extern "C" void WindUI_InstallDemo();''')
s=s.replace('static void enqueue(const std::string& js){std::lock_guard<std::mutex> lk(g_queueLock);if(g_commands.size()<1000)g_commands.push_back(js);}', '''static void enqueue(const std::string& js){
    std::lock_guard<std::mutex> lk(g_queueLock);
    if(g_commands.size()<4000)g_commands.push_back(js);
}
static void enqueueCommand(const std::string& js,bool persistent){
    std::lock_guard<std::mutex> lk(g_queueLock);
    if(persistent && g_replay.size()<4000)g_replay.push_back(js);
    if(g_commands.size()<4000)g_commands.push_back(js);
}''')
start=s.index('static void flushCommands(JNIEnv* e){')
end=s.index('static void ref(',start)
s=s[:start]+'''static void flushCommands(JNIEnv* e){
    if(!g_webview||!g_webReady)return;
    std::vector<std::string> batch;
    bool replay=false;
    {std::lock_guard<std::mutex> lk(g_queueLock);
       replay=g_needReplay;
       if(replay){batch=g_replay;g_needReplay=false;}
       else batch.swap(g_commands);
       if(replay)g_commands.clear();
    }
    if(replay)js(e,"window.WindUIAPI&&WindUIAPI.reset();");
    for(const std::string& cmd:batch)js(e,cmd);
}
''' + s[end:]
s=s.replace('g_webReady=false;g_loadTicks=0;','g_webReady=false;g_loadTicks=0; {std::lock_guard<std::mutex> lk(g_queueLock);g_needReplay=true;}',1)
# second instance in show
s=s.replace('g_webReady=false;g_loadTicks=0;\n    jclass d=cls(e,"android/app/Dialog");', 'g_webReady=false;g_loadTicks=0;{std::lock_guard<std::mutex> lk(g_queueLock);g_needReplay=true;}\n    jclass d=cls(e,"android/app/Dialog");')
s=s.replace('std::min(dp(e,g_activity,740),maxw),std::min(dp(e,g_activity,490),maxh)', 'std::min(dp(e,g_activity,float(g_windowWidth.load())),maxw),std::min(dp(e,g_activity,float(g_windowHeight.load())),maxh)')
s=s.replace('if(type==2){detach(e);return;}','if(type==2){detach(e);return;}\n    if(type==4){hide(e);return;}') # hide declared before nativeRun
s=s.replace('if(g_userCallback&&id.size()<16&&value.size()<8192)g_userCallback(std::atoi(id.c_str()),value.c_str(),g_userData);', '''if(id.size()<16&&value.size()<8192){
            int numeric=std::atoi(id.c_str());WindUI_Callback cb=nullptr;void* data=nullptr;
            {std::lock_guard<std::mutex> lk(g_callbacksLock);
               auto it=g_callbacks.find(numeric);
               if(it!=g_callbacks.end()){cb=it->second.first;data=it->second.second;}
            }
            if(cb)cb(numeric,value.c_str(),data);
            if(g_userCallback)g_userCallback(numeric,value.c_str(),g_userData);
        }''')
s=s.replace('extern "C" __attribute__((visibility("default"))) bool PLMod_Enable(){g_enabled=true;return postRun(0);}', 'extern "C" __attribute__((visibility("default"))) bool PLMod_Enable(){g_enabled=true;WindUI_InstallDemo();return postRun(0);}')
# after footer
s+='''
// v2 native API. Commands and their authoritative replay log are C++ owned.
// The WebView only turns declarative, JSON-serialized state into interactive views.
extern "C" __attribute__((visibility("default"))) int WindUI_NextId(){return g_nextId.fetch_add(1);}
extern "C" __attribute__((visibility("default"))) int WindUI_Command(const char* method,const char* payload,int persistent){
 if(!method||!payload)return 0;
 std::string op(method),json(payload);
 if(op.size()>90||json.size()>262144||json.empty())return 0;
 for(char ch:op)if(!((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||ch=='.'))return 0;
 const std::string code="window.WindUIAPI&&WindUIAPI.dispatch("+jsonq(op)+",JSON.parse("+jsonq(json)+"));";
 enqueueCommand(code,persistent!=0);
 // Persist theme metadata in C++ for synchronous getters.
 if(op=="theme.add"){
   auto k=json.find("\\\"Name\\\"");if(k!=std::string::npos){k=json.find(':',k);if(k!=std::string::npos){k=json.find('\\\"',k);if(k!=std::string::npos){auto end=json.find('\\\"',k+1);if(end!=std::string::npos){std::string n=json.substr(k+1,end-k-1);std::lock_guard<std::mutex> lk(g_queueLock);if(!n.empty()&&std::find(g_themes.begin(),g_themes.end(),n)==g_themes.end())g_themes.push_back(n);}}}}
 }
 return 1;
}
extern "C" __attribute__((visibility("default"))) void WindUI_On(int id,WindUI_Callback cb,void* userdata){
 std::lock_guard<std::mutex> lk(g_callbacksLock);
 if(cb)g_callbacks[id]={cb,userdata};else g_callbacks.erase(id);
}
extern "C" __attribute__((visibility("default"))) void WindUI_Off(int id){WindUI_On(id,nullptr,nullptr);}
extern "C" __attribute__((visibility("default"))) void WindUI_SetOverlaySize(int w,int h){
 g_windowWidth=std::max(300,std::min(1200,w));g_windowHeight=std::max(250,std::min(900,h));
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
'''
# fix native v1 SetTheme update mirror
s=s.replace('void WindUI_SetTheme(const char* theme){if(theme)enqueue("window.WindUI&&WindUI.setTheme("+jsonq(theme)+");");}', 'void WindUI_SetTheme(const char* theme){if(theme){ {std::lock_guard<std::mutex> lk(g_queueLock);g_currentTheme=theme;} enqueueCommand("window.WindUIAPI&&WindUIAPI.dispatch(\\\"theme.set\\\",{Name:"+jsonq(theme)+"});",true);}}')
s=s.replace('void WindUI_SetTitle(const char* title){if(title)enqueue("window.WindUI&&WindUI.setTitle("+jsonq(title)+");");}', 'void WindUI_SetTitle(const char* title){if(title)enqueueCommand("window.WindUIAPI&&WindUIAPI.dispatch(\\\"window.update\\\",{changes:{Title:"+jsonq(title)+"}});",true);}')
p.write_text(s)
