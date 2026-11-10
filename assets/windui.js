(()=>{'use strict';
const $=id=>document.getElementById(id);
const esc=x=>String(x==null?'':x).replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
const hexOk=s=>typeof s==='string'&&/^#[0-9a-fA-F]{6}([0-9a-fA-F]{2})?$/.test(s);
const parseColor=(c,fallback)=>hexOk(c)?c:fallback;
const clamp=(n,a,b)=>Math.max(a,Math.min(b,Number(n)||0));
function rgba(hex,a){const h=hex.replace('#','');return 'rgba('+parseInt(h.slice(0,2),16)+','+parseInt(h.slice(2,4),16)+','+parseInt(h.slice(4,6),16)+','+clamp(a,0,1)+')';}
const icons={wind:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><path d="M3 8h11a3 3 0 1 0-3-3M2 12h17a3 3 0 1 1-3 3M4 16h8a3 3 0 1 1-3 3"/></svg>',home:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="m3 10 9-7 9 7v10H3zM9 20v-7h6v7"/></svg>',settings:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="3"/><path d="m19 13 2-1-2-1-.7-3 1-2-2-2-2 1-3-.7-1-2-1 2-3 .7-2-1-2 2 1 2-.7 3-2 1 2 1 .7 3-1 2 2 2 2-1 3 .7 1 2 1-2 3-.7 2 1 2-2-1-2z"/></svg>',layers:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="m12 3 9 5-9 5-9-5 9-5zm-9 9 9 5 9-5m-18 5 9 5 9-5"/></svg>',check:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="m4 12 5 5L20 6"/></svg>',sun:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="4"/><path d="M12 2v2m0 16v2M4.9 4.9l1.4 1.4m11.4 11.4 1.4 1.4M2 12h2m16 0h2M4.9 19.1l1.4-1.4M17.7 6.3l1.4-1.4"/></svg>',moon:'<svg viewBox="0 0 24 24" fill="currentColor"><path d="M20 15.1A8 8 0 0 1 9 4a8 8 0 1 0 11 11.1Z"/></svg>',volume:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M4 9h4l5-4v14l-5-4H4zM17 9q4 3 0 6"/></svg>',bell:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M18 8a6 6 0 0 0-12 0c0 7-3 7-3 9h18c0-2-3-2-3-9m-8 12h4"/></svg>',bird:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M18 7h3l-3 3a7 7 0 1 1-10-6c3-1 5 1 6 3l4 0zM8 14h.01"/></svg>',palette:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M12 3a9 9 0 1 0 0 18h2a2 2 0 0 0 1-4c-1-2 2-2 4-4a8 8 0 0 0-7-10z"/><circle cx="7" cy="11" r="1"/><circle cx="11" cy="7" r="1"/><circle cx="17" cy="10" r="1"/></svg>'};
// Common WindUI/Lucide names used by the documented API examples. Embedded
// vector paths are drawn locally and never fetched from Roblox/network URLs.
Object.assign(icons,{
 'door-open':'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><path d="M13 20H4V4h9v16zm0-16 7-1v17l-7-2V4zM7 12h.01M13 12h.01"/></svg>',
 'alert-circle':'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><path d="M12 7v6m0 4h.01"/></svg>',
 'check-circle':'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><path d="m7 12 3 3 6-6"/></svg>',
 'trash-2':'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M3 6h18M8 6V4h8v2m3 0-1 15H6L5 6m5 4v7m4-7v7"/></svg>',
 'arrow-right':'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><path d="M5 12h14m-6-6 6 6-6 6"/></svg>',
 'alert':'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="m12 3 10 18H2L12 3zm0 7v5m0 3h.01"/></svg>',
 'x':'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="m5 5 14 14M19 5 5 19"/></svg>',
 'bell-ring':'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M18 8a6 6 0 0 0-12 0c0 7-3 7-3 9h18c0-2-3-2-3-9m-8 12h4M2 4l2 2m18-2-2 2"/></svg>'
});
function icon(x){let k=String(x||'layers').replace(/^lucide:/,'');return icons[k]||'<span class="icon-fallback">'+esc((k[0]||'◆').toUpperCase())+'</span>';}
const builtin={Dark:['#101010','#121215','#18181b','#222226','#39393e','#ffffff','#a1a1aa','#52525b'],Light:['#ededee','#f1f2f5','#ffffff','#f2f2f4','#d1d1d5','#18181b','#707078','#487ce8'],Rose:['#200b15','#1b0b13','#311924','#3b222d','#59343e','#ffeafa','#bd9fac','#f05f88'],Plant:['#0c1811','#0a160f','#12271b','#1b3022','#294736','#dcf8e3','#8ba998','#34bf79'],Red:['#1c0e10','#1b0a0d','#2d171b','#362025','#513036','#fbe9ea','#c1a5a7','#ed454e'],Indigo:['#111127','#0d0e25','#1c1f3e','#24274c','#34365b','#e9eaff','#a8abd1','#6768ec'],Sky:['#081d25','#09202a','#13313d','#1a3a48','#2c4c58','#e4f9ff','#a3c5d2','#18b1cf'],Violet:['#180c2c','#160b2b','#271c43','#30204b','#473060','#f2eaff','#afa4c8','#9254ec'],Amber:['#1f170e','#1b140c','#322719','#3b2c1b','#51422a','#fbeed6','#b8ad91','#d3a64c'],Emerald:['#071d17','#061912','#112a20','#18392b','#2b5140','#e1fff3','#9cbcae','#00a77f'],Midnight:['#0a1022','#091124','#121b36','#1a2541','#303955','#eff2ff','#9da9c5','#245dd5'],Crimson:['#240d16','#1e0b12','#34151f','#3f1d2a','#56313a','#ffeaf0','#c6a0ad','#d64069'],'Monokai Pro':['#18171e','#15141a','#23222b','#2d2a34','#44404c','#faf9f3','#b7b4c1','#a2cf70'],'Cotton Candy':['#282136','#231d30','#3b304d','#463d57','#61546c','#fff4fb','#d1bed7','#f395c3'],Mellowsi:['#201812','#1e1711','#30281b','#3f3020','#534332','#ffefe2','#c4b1a1','#cfaa63'],Rainbow:['#141820','#131921','#242732','#2a2d3b','#454759','#eff6ff','#aeb6d3','#45b6f3']};
let state;
const blank=()=>({window:{id:0,Title:'WindUI',Author:'Native UI library',Icon:'wind',Theme:'Dark',HideSearchBar:false},nodes:{},tabs:[],nav:[],active:null,modalDefs:{},popupDefs:{},flags:{},theme:'Dark',themes:{},notificationLower:false,toastTimers:[],modal:null,popup:null,selectedTab:null,lockedAll:false,openDropdown:null});
let rAF=0;const refresh=()=>{if(!rAF)rAF=requestAnimationFrame(()=>{rAF=0;render()})};
function send(id,value){try{const f=document.createElement('iframe');f.style.display='none';f.setAttribute('aria-hidden','true');f.src='windui://event?id='+encodeURIComponent(String(id))+'&value='+encodeURIComponent(typeof value==='string'?value:JSON.stringify(value));document.body.appendChild(f);setTimeout(()=>f.remove(),700)}catch(e){}}
function closeNative(){send(-Number(state.window.id)*10-2,'closed');const f=document.createElement('iframe');f.style.display='none';f.src='windui://close';document.body.appendChild(f);setTimeout(()=>f.remove(),400)}
function applyTheme(name){const t=state.themes[name]||{},c=builtin[name]||builtin.Dark;state.theme=name;
const background=parseColor(t.WindowBackground||t.Background,c[0]),sidebar=parseColor(t.PanelBackground,c[1]);
const surface=parseColor(t.ElementBackground,c[2]),button=parseColor(t.Button||t.Toggle,c[3]),border=parseColor(t.Outline||t.TabBorder,c[4]),text=parseColor(t.Text||t.ElementTitle,c[5]),muted=parseColor(t.Placeholder||t.ElementDesc,c[6]),accent=parseColor(t.Accent||t.Primary||t.Slider,c[7]);
const values={'--bg':background,'--sidebar':t.PanelBackgroundTransparency!=null?rgba(sidebar,Math.min(.96,1-Number(t.PanelBackgroundTransparency))):sidebar,'--surface':t.ElementBackgroundTransparency!=null?rgba(surface,1-Number(t.ElementBackgroundTransparency)):surface,'--surface2':c[3],'--button':button,'--border':t.TabBorderTransparency!=null?rgba(border,1-Number(t.TabBorderTransparency)):border,'--text':text,'--muted':muted,'--accent':accent,'--accentfg':name==='Light'?'#ffffff':'#ffffff','--radius':(Number(state.window.Radius)||16)+'px','--glow':rgba(accent,.2)};
for(const k in values)document.documentElement.style.setProperty(k,values[k]);
const extra={'--topbar-title':parseColor(t.WindowTopbarTitle,text),'--element-title':parseColor(t.ElementTitle,text),
'--element-desc':parseColor(t.ElementDesc,muted),'--tab-text':parseColor(t.TabText,muted),
'--dialog-title':parseColor(t.DialogTitle,text),'--dialog-content':parseColor(t.DialogContent,muted),
'--popup-title':parseColor(t.PopupTitle,text),'--popup-content':parseColor(t.PopupContent,muted),
'--notification-title':parseColor(t.NotificationTitle,text),'--notification-content':parseColor(t.NotificationContent,muted),
'--slider-thumb':parseColor(t.SliderThumb,'#d7d8df'),'--toggle-bg':parseColor(t.Toggle,button),
'--section-icon':parseColor(t.SectionIcon,muted),'--checkbox-color':parseColor(t.Checkbox,accent),
'--tab-active':rgba(parseColor(t.TabBackgroundActive,t.TabBackground||text),1-clamp(t.TabBackgroundActiveTransparency??.9,0,1)),
'--tab-hover':rgba(parseColor(t.TabBackgroundHover,text),1-clamp(t.TabBackgroundHoverTransparency??.96,0,1)),
'--tab-icon':parseColor(t.TabIcon,muted),
'--toggle-bar':parseColor(t.ToggleBar,'#ffffff'),
'--slider-icon':parseColor(t.SliderIcon,muted),
'--dialog-bg':parseColor(t.DialogBackground,background),
'--popup-bg':parseColor(t.PopupBackground,background),
'--notification-bg':parseColor(t.Notification,background),
'--notification-border':rgba(parseColor(t.NotificationBorder,border),1-clamp(t.NotificationBorderTransparency??.75,0,1)),
'--search-bg':parseColor(t.WindowSearchBarBackground,surface),
'--search-border':rgba(parseColor(t.SearchBarBorder,border),1-clamp(t.SearchBarBorderTransparency??.75,0,1)),
'--label-bg':rgba(parseColor(t.LabelBackground,surface),1-clamp(t.LabelBackgroundTransparency??.9,0,1))};
for(const k in extra)document.documentElement.style.setProperty(k,extra[k]);
document.documentElement.style.setProperty('--sidebar-bg',name==='Rainbow'?'rgba(0,0,0,.12)':values['--sidebar']);document.documentElement.style.setProperty('--window-gradient',name==='Rainbow'?'linear-gradient(180deg,#f0134d 0%,#ffb52c 25%,#e9ff13 40%,#01ec69 59%,#087ffa 78%,#3b2df4 100%)':'none');
if(name==='Rainbow'){document.documentElement.style.setProperty('--sidebar','rgba(0,0,0,.12)');document.documentElement.style.setProperty('--text','#fff');document.documentElement.style.setProperty('--surface','rgba(255,255,255,.1)');document.documentElement.style.setProperty('--button','rgba(255,255,255,.14)');document.documentElement.style.setProperty('--border','rgba(255,255,255,.15)');}
}
function configFor(id){return state.nodes[id]?.config||{};}
function newNode(id,parent,type,config){if(!id)return;state.nodes[id]={id,parent,type,config:{...(config||{})},children:[],open:config?.Opened!==false,visible:true};const parentNode=state.nodes[parent];if(parentNode)parentNode.children.push(id);}
function dispatch(op,p={}){try{switch(op){
case 'window.create': {const themes=state.themes;const flags=state.flags;state=blank();state.themes=themes;state.flags=flags;state.window={...state.window,...p.config,id:p.id};state.theme=state.window.Theme||'Dark';applyTheme(state.theme);break;}
case 'window.update': Object.assign(state.window,p.changes||{});if(p.changes?.Theme)applyTheme(p.changes.Theme);break;
case 'window.action': windowAction(p.action);break;
case 'theme.add':if(p.Name){state.themes[p.Name]={...p};if(state.theme===p.Name)applyTheme(p.Name);}break;
case 'theme.set':if(p.Name){applyTheme(p.Name);state.window.Theme=p.Name;}break;
case 'tab.create':newNode(p.id,p.parent,'tab',p.config);state.tabs.push(p.id);if(state.active==null)state.active=p.id;break;
case 'navsection.create':newNode(p.id,p.parent,'navsection',p.config);state.nav.push(p.id);break;
case 'navsection.divider':state.nav.push('divider:'+p.id);break;
case 'container.create':newNode(p.id,p.parent,p.type,p.config);break;
case 'container.update':if(state.nodes[p.id])Object.assign(state.nodes[p.id].config,p.changes||{});break;
case 'container.action':
 if(p.action==='select'&&state.nodes[p.id]?.type==='tab'){state.active=p.id;state.openDropdown=null;}
 else if(p.action==='scrollTo')requestAnimationFrame(()=>document.querySelector('[data-element="'+p.target+'"]')?.scrollIntoView({behavior:'smooth',block:'nearest'}));
 else if(p.action==='open'&&state.nodes[p.id])state.nodes[p.id].open=true;
 else if(p.action==='close'&&state.nodes[p.id])state.nodes[p.id].open=false;
 else if(p.action==='destroy')destroyNode(p.id);
 break;
case 'element.create':newNode(p.id,p.parent,p.type,p.config);{const key=p.config?.Flag;if(key&&Object.hasOwn(state.flags,key))state.nodes[p.id].config.Value=state.flags[key];}break;
case 'element.update':if(state.nodes[p.id]){
   const c=state.nodes[p.id].config;Object.assign(c,p.changes||{});
   if(c.Value&&typeof c.Value==='object')for(const k of ['Min','Max','Step'])if(p.changes&&k in p.changes)c.Value[k]=p.changes[k];
 }break;
case 'element.action':if(p.action==='destroy')destroyNode(p.id);
 else if(p.action==='toggle'){const n=state.nodes[p.id];if(n)changeValue(p.id,!n.config.Value);}
 else if(p.action==='highlight'){requestAnimationFrame(()=>{let e=document.querySelector('[data-element="'+p.id+'"]');if(e){e.classList.add('highlight');setTimeout(()=>e.classList.remove('highlight'),700)}});}
 else if(p.action==='select'){const node=state.nodes[p.id];if(node&&node.type==='tab'){state.active=node.id;state.openDropdown=null;}}break;
case 'modal.create':state.modalDefs[p.id]={type:p.type,config:p.config||{},buttons:p.buttons||[]};break;
case 'modal.action':if(p.action==='show'){if(state.modalDefs[p.id]?.type==='popup'){state.popup=p.id;state.modal=null;}else{state.modal=p.id;state.popup=null;}}else if(p.action==='close'){if(state.modal===p.id)state.modal=null;if(state.popup===p.id)state.popup=null;}break;
case 'notify':notify(p);break;
case 'tag.create':state.window.tag=p.config||{};break;
case 'topbarButton.create':(state.window.topbarButtons??=[]).push({...p.config,id:p.id});break;
case 'library.update':
 if(p.NotificationLower!==undefined)state.notificationLower=!!p.NotificationLower;
 if(p.Font)document.body.style.fontFamily=String(p.Font)+',sans-serif';
 if(p.Language)state.language=p.Language;
 if(p.Localization)state.localization=p.Localization;
 if(p.Acrylic!==undefined)state.window.Acrylic=!!p.Acrylic;
 if(Array.isArray(p.Gradient)&&p.Gradient.length){const stops=p.Gradient.filter(x=>hexOk(x));if(stops.length)document.documentElement.style.setProperty('--window-gradient','linear-gradient(125deg,'+stops.join(',')+')');}
 break;
case 'key.validate':if(p.allowed){state.window.keyUnlocked=true;state.modal=null;try{if(state.window.KeySystem?.SaveKey)localStorage.setItem('windui.key.'+state.window.Folder,'1')}catch(e){}}else{notify({Title:'Invalid key',Content:'Please try another key',Duration:3})}break;
case 'user.update':state.window.User={...(state.window.User||{}),...(p.changes||{})};break;
case 'config.action':{
 const key='windui.flags.'+(state.window.Folder||'Default')+'.'+(p.name||'Default');
 try{if(p.action==='save')localStorage.setItem(key,JSON.stringify(state.flags));
 else if(p.action==='load'){state.flags={...state.flags,...JSON.parse(localStorage.getItem(key)||'{}')};for(const n of Object.values(state.nodes))if(n.config?.Flag&&n.config.Flag in state.flags)n.config.Value=state.flags[n.config.Flag];}
 else if(p.action==='delete')localStorage.removeItem(key);}catch(e){console.warn('Config storage',e)}break;
} 
}refresh();return true;}catch(err){document.title='WindUI runtime error';console.error('WindUI error',op,err);return false;}}
function destroyNode(id){let n=state.nodes[id];if(!n)return;for(const c of [...n.children])destroyNode(c);if(state.nodes[n.parent])state.nodes[n.parent].children=state.nodes[n.parent].children.filter(x=>x!==id);state.tabs=state.tabs.filter(x=>x!==id);delete state.nodes[id];if(state.active===id)state.active=state.tabs[0]||null;}
function windowAction(action){if(action==='open')send(-state.window.id*10-1,'opened');if(action==='close')closeNative();if(action==='destroy'){send(-state.window.id*10-3,'destroyed');closeNative();state=blank();}if(action==='toggle')closeNative();if(action==='lockAll')state.lockedAll=true;if(action==='unlockAll')state.lockedAll=false;if(action==='fullscreen')$('window').classList.toggle('fullscreen');if(action==='center')$('window').style.transform='';}
function notify(p){if(state.window.IgnoreAlerts)return;const toast=document.createElement('div');toast.className='toast-note';toast.innerHTML='<div class="toast-icon">'+icon(p.Icon||'bell')+'</div><div><b>'+esc(p.Title||'Notification')+'</b><small>'+esc(p.Content||'')+'</small></div>';toast.classList.add('active');$('notify-layer').appendChild(toast);const duration=clamp(p.Duration==null?3:p.Duration,.5,120);setTimeout(()=>{toast.classList.remove('active');setTimeout(()=>toast.remove(),300)},duration*1000);}
function rowStart(c,id){return '<div class="row-head"><div class="row-title">'+esc(c.Title||'')+'</div>'+(c.Desc?'<div class="row-desc">'+esc(c.Desc)+'</div>':'')+'</div>';}
function optionValue(o){return typeof o==='object'?String(o.Value??o.Title??''):String(o);}
function optionTitle(o){return typeof o==='object'?String(o.Title??o.Value??''):String(o);}
function dropdownValues(v){return Array.isArray(v)?v.map(x=>String(x)):v==null?[]:[String(v)];}
function dropdownLabel(c,v){const values=dropdownValues(v);if(!values.length)return String(c.Placeholder||'Select...');if(c.Multi)return values.join(', ');return values[0];}
function isDisabled(n){if(state.lockedAll||n.config.Locked||n.config.Disabled)return true;for(let x=state.nodes[n.parent];x;x=state.nodes[x.parent])if(x.config.Locked)return true;return false;}
function renderElement(n){const c=n.config||{},id=n.id,disabled=isDisabled(n)?' disabled':'',title=esc(c.Title||''),desc=esc(c.Desc||'');let ctrl='',t=n.type;
const v=(c.Value&&typeof c.Value==='object'&&'Default'in c.Value)?c.Value.Default:(c.Value!==undefined?c.Value:(c.Default!==undefined?c.Default:null));
switch(t){
case 'divider':return '<div class="separator" data-element="'+id+'"></div>';
case 'space':return '<div class="spacing" style="height:'+clamp(c.Columns||1,1,7)*10+'px" data-element="'+id+'"></div>';
case 'paragraph':return '<div class="paragraph" data-element="'+id+'"><div class="paragraph-body">'+(c.Image?'<span class="paragraph-icon">'+icon(c.Image)+'</span>':'')+'<div><b>'+title+'</b><p>'+desc+'</p></div></div>'+(Array.isArray(c.Buttons)?'<div class="buttons-flex">'+c.Buttons.map(b=>'<button class="pill" data-control="'+id+'" data-value="'+esc(b.Title||'click')+'">'+esc(b.Title||'Action')+'</button>').join('')+'</div>':'')+'</div>';
case 'code':return '<div class="code-block" data-element="'+id+'">'+(c.Title?'<div class="row-title">'+title+'</div>':'')+'<pre>'+esc(c.Code||'')+'</pre><button class="pill copy-code" data-copy="'+id+'">Copy</button></div>';
case 'image':return '<div class="paragraph" data-element="'+id+'"><b>'+title+'</b><div class="image-placeholder">'+(c.Image&&/^https:\/\//.test(c.Image)?'<img src="'+esc(c.Image)+'" alt="'+title+'"/>':icon(c.Icon||'layers'))+'</div>'+desc+'</div>';
case 'button':ctrl='<button class="pill '+(c.Variant==='Destructive'?'destructive':'primary')+'" data-control="'+id+'" data-value="click"'+disabled+'>'+(c.Icon?icon(c.Icon):'')+esc(c.ButtonText||c.Title||'Run')+'</button>';break;
case 'toggle':ctrl='<button role="switch" aria-checked="'+!!v+'" class="toggle '+(v?'on':'')+'" data-toggle="'+id+'" '+disabled+'></button>';break;
case 'checkbox':ctrl='<button role="checkbox" aria-checked="'+!!v+'" class="checkbox-btn '+(v?'on':'')+'" data-toggle="'+id+'" '+disabled+'>'+((!!v)?icon('check'):'')+'</button>';break;
case 'slider':{const range=c.Value&&typeof c.Value==='object'?c.Value:c;const mn=Number(range.Min??0),mx=Number(range.Max??100),step=Number(range.Step??c.Step??1),val=clamp(v==null?50:v,mn,mx);return '<div class="slider-block" data-element="'+id+'"><div class="range-head"><div>'+title+(desc?'<div class="row-desc">'+desc+'</div>':'')+'</div><span id="slider-v-'+id+'">'+val+'</span></div><div class="slider-shell"><span class="range-icon">'+icon('moon')+'</span><input type="range" data-slider="'+id+'" min="'+mn+'" max="'+mx+'" value="'+val+'" step="'+(step>0?step:1)+'" '+disabled+'/><span class="range-icon">'+icon('sun')+'</span></div></div>';}
case 'input':ctrl='<input type="'+(c.Password?'password':c.Numeric?'number':'text')+'" class="text-input" aria-label="'+title+'" value="'+esc(v||'')+'" placeholder="'+esc(c.Placeholder||'Type here...')+'" data-input="'+id+'" '+disabled+'/>';break;
case 'dropdown':{const options=Array.isArray(c.Values)?c.Values:[];const selected=dropdownValues(v);const open=state.openDropdown===id;const items=options.map((o,idx)=>{const value=optionValue(o),label=optionTitle(o),active=selected.includes(value);return '<button class="dropdown-option '+(active?'selected':'')+'" data-option="'+id+'" data-option-index="'+idx+'" '+disabled+'><span>'+esc(label)+'</span>'+(active?'<span class="dropdown-check">'+icon('check')+'</span>':'')+'</button>';}).join('');return '<div class="dropdown-block" data-element="'+id+'"><div class="row dropdown-row'+(open?' open':'')+'">'+rowStart(c,id)+'<button class="dropdown-trigger" data-dropdown="'+id+'" '+disabled+'><span class="dropdown-text">'+esc(dropdownLabel(c,v))+'</span><span class="dropdown-caret">⌄</span></button></div>'+(open?'<div class="dropdown-panel">'+items+'</div>':'')+'</div>'; }
case 'colorpicker':ctrl='<input type="color" class="color-input" data-color="'+id+'" value="'+parseColor(String(v||c.Default||'#ffffff'),'#ffffff')+'" '+disabled+'/>';break;
case 'keybind':ctrl='<button class="pill" data-keybind="'+id+'" '+disabled+'>'+esc(String(v||'Bind key'))+'</button>';break;
case 'progressbar':{let val=clamp(v??c.Progress??0,0,100);return '<div class="slider-block" data-element="'+id+'"><div class="range-head"><span>'+title+'</span><span>'+val+'%</span></div><div class="progress-track"><div class="progress-bar" style="width:'+val+'%"></div></div></div>';}
case 'label':return '<div class="group-label" data-element="'+id+'">'+title+'</div>';
default:ctrl='<span class="row-desc">'+esc(t)+'</span>';}
return '<div class="row" data-element="'+id+'">'+rowStart(c,id)+ctrl+'</div>';
}
function renderNode(id){let n=state.nodes[id];if(!n||n.visible===false)return '';if(n.type==='tab'||n.type==='navsection')return n.children.map(renderNode).join('');
if(n.type==='section'){return '<section class="section-block" data-element="'+n.id+'"><button class="section-title" data-section="'+n.id+'"><span>'+icon(n.config.Icon||'layers')+' '+esc(n.config.Title||'Section')+'</span><span>'+(n.open?'⌃':'⌄')+'</span></button>'+(n.open?'<div class="section-body">'+n.children.map(renderNode).join('')+'</div>':'')+'</section>';}
if(['group','hstack','vstack'].includes(n.type))return '<div class="layout '+(n.type==='vstack'?'layout-v':'layout-h')+'" data-element="'+n.id+'">'+n.children.map(renderNode).join('')+'</div>';
return renderElement(n);}
function navList(){const q=($('search')?.value||'').toLowerCase();let html='';const addTab=id=>{const n=state.nodes[id];if(!n||n.type!=='tab')return '';const c=n.config;const t=String(c.Title||'Tab');if(!t.toLowerCase().includes(q))return '';return '<button class="nav '+(state.active===id?'active':'')+' '+(n.config.Locked?'nav-locked':'')+'" data-tab="'+id+'" '+(n.config.Locked?'disabled':'')+'>'+icon(c.Icon||'layers')+'<span>'+esc(t)+'</span>'+(c.Border?'<small>•</small>':'')+'</button>';};
for(const nav of state.nav){if(typeof nav==='string'){html+='<div class="separator"></div>';continue;}const n=state.nodes[nav];if(!n)continue;html+='<div class="nav-header">'+esc(n.config.Title||'Navigation')+'</div>';for(const id of n.children){html+=addTab(id);}}
const standalone=state.tabs.filter(id=>!state.nodes[state.nodes[id]?.parent]||state.nodes[state.nodes[id]?.parent]?.type!=='navsection');if(standalone.length){html+='<div class="nav-header">Navigation</div>';for(const id of standalone)html+=addTab(id);}
return html||'<div class="nav-header">No tabs available</div>';}
function renderModal(){const layer=$('modal-layer');if(!state.modal){layer.classList.remove('visible');layer.innerHTML='';return;}let def=state.modalDefs[state.modal];if(!def){state.modal=null;return;}const c=def.config||{};layer.classList.add('visible');layer.innerHTML='<div class="modal-box" role="dialog" aria-modal="true"><div class="modal-icon">'+icon(c.Icon||'bell')+'</div><h2>'+esc(c.Title||'Dialog')+'</h2><p>'+esc(c.Content||'')+'</p><div class="modal-actions">'+(def.buttons.length?def.buttons.map(b=>'<button class="pill '+(b.Variant==='Destructive'?'destructive':b.Variant==='Secondary'?'':'primary')+'" data-modal-button="'+b.id+'">'+esc(b.Title||'Button')+'</button>').join(''):'<button class="pill primary" data-modal-close="1">Close</button>')+'</div></div>';}
function renderPopup(){let layer=$('popup-layer');if(!state.popup){layer.innerHTML='';return;}const d=state.modalDefs[state.popup];if(!d)return;const c=d.config;layer.innerHTML='<div class="popup-box"><button class="popup-close" data-popup-close="1">×</button><div class="modal-icon">'+icon(c.Icon||'bell')+'</div><h3>'+esc(c.Title||'Popup')+'</h3><p>'+esc(c.Content||'')+'</p><div class="modal-actions">'+d.buttons.map(b=>'<button class="pill '+(b.Variant==='Secondary'?'':'primary')+'" data-modal-button="'+b.id+'">'+esc(b.Title)+'</button>').join('')+'</div></div>';}
function lockKeyScreen(){const k=state.window.KeySystem;if(!k||state.window.keyUnlocked)return '';try{if(k.SaveKey&&localStorage.getItem('windui.key.'+state.window.Folder)==='1'){state.window.keyUnlocked=true;return '';}}catch(e){}return '<div class="key-overlay"><div class="modal-box"><h2>Access key</h2><p>'+esc(k.Note||'Enter your access key')+'</p><input class="text-input" placeholder="Key" id="key-input"/><div class="modal-actions"><button class="pill primary" data-key-submit="1">Verify</button></div></div></div>';}
function render(){const w=state.window||{};const wrapper=$('window');if(!wrapper)return;applyTheme(state.theme||'Dark');wrapper.classList.toggle('acrylic',!!w.Acrylic);wrapper.style.borderRadius=(Number(w.Radius)||16)+'px';wrapper.style.setProperty('--sidebar-width',clamp(w.SideBarWidth||190,105,310)+'px');wrapper.style.zoom=clamp(w.UIScale||1,.6,1.4);wrapper.style.opacity=1-clamp(w.BackgroundTransparency||0,0,.85);if(w.Background&&typeof w.Background==='string'&&/^https:\/\//.test(w.Background)){wrapper.style.backgroundImage='linear-gradient(#0008,#0008),url("'+w.Background.replace(/"/g,'')+'")';}else wrapper.style.backgroundImage='var(--window-gradient)';
$('brand').textContent=w.Title||'WindUI';$('by').textContent=w.Author||'Native UI library';$('brand-icon').innerHTML=icon(w.Icon||'wind');$('search').style.display=w.HideSearchBar?'none':'';$('sidebar-foot').innerHTML=(w.User?.Enabled?'<button id="user-button" class="user-button">'+(w.User.Anonymous?'◉ Anonymous':'◉ User')+'</button>':'')+esc(w.Footer?.Title||'WindUI • C++ native API v2.0.1');
$('tabs').innerHTML=navList();let tab=state.nodes[state.active];$('tab-title').textContent=tab?.config?.Title||w.Title||'WindUI';
$('notify-layer').style.top=state.notificationLower?'auto':'56px';$('notify-layer').style.bottom=state.notificationLower?'20px':'auto';
$('content').innerHTML=tab?((tab.config.ShowTabTitle?'<h2 class="title">'+esc(tab.config.Title||'Tab')+'</h2>':'')+(tab.config.Desc?'<p class="sub">'+esc(tab.config.Desc)+'</p>':'')+(tab.children.length?tab.children.map(renderNode).join(''):'<div class="empty"><div>'+icon(tab.config.CustomEmptyPage?.Icon||'layers')+'</div><h3>'+esc(tab.config.CustomEmptyPage?.Title||'This tab is empty')+'</h3><p>'+esc(tab.config.CustomEmptyPage?.Desc||'Add controls from C++ to populate this page.')+'</p></div>')):'<div class="empty"><div>'+icon('wind')+'</div><h3>WindUI Native Library</h3><p>Create a Window and Tab from C++ to start.</p></div>';
if($('user-button'))$('user-button').onclick=()=>send(-state.window.id*10-5,'click');
$('tag').textContent=w.tag?.Title||'';$('tag').style.display=w.tag?.Title?'inline-flex':'none';
$('top-extra').innerHTML=(w.topbarButtons||[]).filter(b=>!w.DisabledTopbarButtons?.[b.Title]).map(b=>'<button class="i-btn" data-control="'+b.id+'" data-value="click" title="'+esc(b.Title||'Button')+'">'+icon(b.Icon||'layers')+'</button>').join('');
renderModal();renderPopup();const key=lockKeyScreen();$('key-layer').innerHTML=key;
wire();}
function changeValue(id,value){let n=state.nodes[id];if(!n)return;n.config.Value=value;if(n.config.Flag){state.flags[n.config.Flag]=value;try{localStorage.setItem('windui.native.flags',JSON.stringify(state.flags))}catch(e){}}send(id,value);}
function wire(){document.querySelectorAll('[data-tab]').forEach(n=>n.onclick=()=>{state.active=Number(n.dataset.tab);state.openDropdown=null;render();$('content').scrollTop=0;});
document.querySelectorAll('[data-section]').forEach(n=>n.onclick=()=>{let s=state.nodes[Number(n.dataset.section)];s.open=!s.open;render()});
document.querySelectorAll('[data-toggle]').forEach(n=>n.onclick=()=>{let id=Number(n.dataset.toggle);changeValue(id,!state.nodes[id].config.Value);render();});
document.querySelectorAll('[data-control]').forEach(n=>n.onclick=()=>send(Number(n.dataset.control),n.dataset.value||'click'));
document.querySelectorAll('[data-input]').forEach(n=>n.onchange=()=>changeValue(Number(n.dataset.input),n.value));
document.querySelectorAll('[data-slider]').forEach(n=>{n.oninput=()=>{let label=$('slider-v-'+n.dataset.slider);if(label)label.textContent=n.value;const p=(n.value-n.min)/(n.max-n.min)*100;n.style.background='linear-gradient(to right,var(--accent) '+p+'%,var(--button) '+p+'%)';};n.onchange=()=>changeValue(Number(n.dataset.slider),Number(n.value));n.oninput();});
document.querySelectorAll('[data-dropdown]').forEach(n=>n.onclick=()=>{const id=Number(n.dataset.dropdown);state.openDropdown=state.openDropdown===id?null:id;render();});
document.querySelectorAll('[data-option]').forEach(n=>n.onclick=()=>{const id=Number(n.dataset.option),idx=Number(n.dataset.optionIndex),node=state.nodes[id],options=Array.isArray(node?.config?.Values)?node.config.Values:[];const raw=options[idx];const value=optionValue(raw);if(node?.config?.Multi){const current=dropdownValues(node.config.Value);const next=current.includes(value)?current.filter(x=>x!==value):current.concat(value);changeValue(id,next);state.openDropdown=id;}else{changeValue(id,value);state.openDropdown=null;}render();});
document.querySelectorAll('[data-color]').forEach(n=>n.onchange=()=>changeValue(Number(n.dataset.color),n.value));
document.querySelectorAll('[data-keybind]').forEach(n=>n.onclick=()=>{n.textContent='Press any key…';const cb=e=>{changeValue(Number(n.dataset.keybind),e.key);n.textContent=e.key;document.removeEventListener('keydown',cb)};document.addEventListener('keydown',cb);});
document.querySelectorAll('[data-copy]').forEach(n=>n.onclick=()=>{const c=configFor(Number(n.dataset.copy));try{navigator.clipboard.writeText(String(c.Code||''))}catch(e){}send(Number(n.dataset.copy),'copy');notify({Title:'Copied',Content:'Code copied to clipboard',Duration:2})});
document.querySelectorAll('[data-modal-button]').forEach(n=>n.onclick=()=>{send(Number(n.dataset.modalButton),'click');state.modal=null;state.popup=null;renderModal();renderPopup();});
document.querySelectorAll('[data-modal-close]').forEach(n=>n.onclick=()=>{state.modal=null;renderModal()});
document.querySelectorAll('[data-popup-close]').forEach(n=>n.onclick=()=>{state.popup=null;renderPopup()});
document.querySelectorAll('[data-key-submit]').forEach(n=>n.onclick=()=>{let v=$('key-input')?.value||'';if(!v)return;const k=state.window.KeySystem||{};if(Array.isArray(k.Key)&&k.Key.includes(v)){dispatch('key.validate',{allowed:true});}else send(-state.window.id*10-4,v);});
}
window.WindUIAPI={dispatch,reset(){let themes=state.themes;let flags=state.flags;state=blank();state.themes=themes;state.flags=flags;refresh()},getState(){return state},getThemes(){return Object.keys(builtin).concat(Object.keys(state.themes))}};
// Backwards-compatible JavaScript facade for native extensions using ABI v1.
const legacyTabNames={};let legacySeq=500000;
window.WindUI={
setTitle:Title=>dispatch('window.update',{changes:{Title}}),setTheme:Name=>dispatch('theme.set',{Name}),getThemes:()=>window.WindUIAPI.getThemes(),
addTab:(title,icon)=>{let id=++legacySeq;legacyTabNames[title]=id;dispatch('tab.create',{id,parent:state.window.id,config:{Title:title,Icon:icon}});return id;},
addElement:(tab,type,title,desc,id,value,min,max,step,options)=>{let parent=legacyTabNames[tab]||state.tabs.find(t=>state.nodes[t]?.config?.Title===tab);if(!parent)return false;let config={Title:title,Desc:desc,Value:value,Min:min,Max:max,Step:step,Values:options};if(type==='slider')config.Value={Min:min,Max:max,Default:value,Step:step};return dispatch('element.create',{id,parent,type,config});},
notify:(title,content)=>dispatch('notify',{Title:title,Content:content,Duration:3}),dialog:(title,content)=>{let id=++legacySeq;dispatch('modal.create',{id,type:'dialog',config:{Title:title,Content:content},buttons:[]});dispatch('modal.action',{id,action:'show'})},
close:closeNative,minimize:closeNative,resetScroll:()=>{$('content').scrollTop=0},selectTab:id=>dispatch('container.action',{id,action:'select'})};
$('search').addEventListener('input',()=>{$('tabs').innerHTML=navList();document.querySelectorAll('[data-tab]').forEach(n=>n.onclick=()=>{state.active=Number(n.dataset.tab);render()})});
$('win-close').addEventListener('click',closeNative);$('win-minimize').addEventListener('click',closeNative);$('win-max').addEventListener('click',()=>$('window').classList.toggle('fullscreen'));
try{state=blank();state.flags=JSON.parse(localStorage.getItem('windui.native.flags')||'{}');}catch(e){state=blank()};applyTheme(state.theme);render();
setTimeout(()=>{const f=document.createElement('iframe');f.style.display='none';f.src='windui://ready';document.body.appendChild(f);setTimeout(()=>f.remove(),1200);},70);
})();
