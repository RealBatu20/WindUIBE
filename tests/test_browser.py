#!/usr/bin/env python3
"""Offline renderer smoke test. Browser !== Minecraft/Android runtime."""
import json
from pathlib import Path
from playwright.sync_api import sync_playwright
ROOT=Path(__file__).resolve().parents[1]
commands=[json.loads(x) for x in (ROOT/'tests/demo_commands.jsonl').read_text().splitlines() if x]
html=(ROOT/'assets/windui.html').read_text()
js=(ROOT/'assets/windui.js').read_text()
i=html.index('<script>');j=html.index('</script>',i)
html=html[:i]+'<script>\n'+js+'\n'+html[j:]
with sync_playwright() as pw:
 browser=pw.chromium.launch(headless=True,executable_path='/usr/bin/chromium',args=['--no-sandbox','--disable-dev-shm-usage'])
 page=browser.new_page(viewport={'width':950,'height':700},device_scale_factor=1)
 errors=[]
 page.on('pageerror',lambda e:errors.append(str(e)))
 page.set_content(html,wait_until='domcontentloaded',timeout=25000)
 for cmd in commands:
  if not page.evaluate('(m)=>WindUIAPI.dispatch(m.op,m.payload)',cmd):raise AssertionError(cmd)
 page.wait_for_timeout(240)
 state=page.evaluate('WindUIAPI.getState()')
 assert len(state['tabs'])==4, (len(state['tabs']),errors)
 assert page.locator('[data-tab]').count()==4,page.locator('#tabs').inner_text()
 assert len(state['nodes'])>=27
 assert page.locator('.row,[data-element]').count()>3
 # Toggle and slider are interactive in renderer; C++ receives events in Android.
 toggle=page.locator('[data-toggle]').first
 before=toggle.get_attribute('aria-checked')
 toggle.click()
 assert page.locator('[data-toggle]').first.get_attribute('aria-checked')!=before
 page.locator('[data-slider]').first.fill('75')
 assert page.locator('[data-slider]').first.input_value()=='75'
 # Parent section collapse/expand.
 section=page.locator('[data-section]').first
 section.click(); assert page.locator('[data-toggle]').count()==0
 section.click(); assert page.locator('[data-toggle]').count()>0
 # Selection, in-panel dropdowns, nested input.
 page.locator('[data-tab]').filter(has_text='Elements').click()
 assert page.locator('[data-input]').count()>0
 page.locator('[data-input]').first.fill('native-user')
 page.locator('[data-input]').first.dispatch_event('change')
 page.locator('[data-dropdown]').first.click()
 page.locator('[data-option]').filter(has_text='Advanced').first.click()
 assert 'Advanced' in page.locator('[data-dropdown]').first.text_content()
 # Modal vs popup: backdrop layers and button dismissal.
 dialogs=[v for k,v in state['modalDefs'].items() if v['type']=='dialog']
 popups=[v for k,v in state['modalDefs'].items() if v['type']=='popup']
 assert dialogs and popups
 did=next(k for k,v in state['modalDefs'].items() if v['type']=='dialog')
 pid=next(k for k,v in state['modalDefs'].items() if v['type']=='popup')
 page.evaluate('(id)=>WindUIAPI.dispatch("modal.action",{id,action:"show"})',int(did))
 page.locator('.modal-box').first.wait_for(timeout=3000)
 assert page.locator('.modal-box').count()>0
 page.locator('[data-modal-button]').first.click()
 assert page.locator('.modal-box').count()==0
 page.evaluate('(id)=>WindUIAPI.dispatch("modal.action",{id,action:"show"})',int(pid))
 page.locator('.popup-box').first.wait_for(timeout=3000)
 assert page.locator('.popup-box').count()>0
 page.locator('[data-popup-close]').first.click()
 assert page.locator('.popup-box').count()==0
 page.evaluate('WindUIAPI.dispatch("theme.set",{Name:"Rainbow"})')
 assert page.evaluate('WindUIAPI.getState().theme')=='Rainbow'
 assert "linear-gradient" in page.evaluate('document.documentElement.style.getPropertyValue("--window-gradient")')
 page.evaluate('WindUIAPI.dispatch("notify",{Title:"Ready",Content:"Native C++",Duration:2})')
 page.locator('.toast-note').first.wait_for(timeout=3000)
 assert page.locator('.toast-note').count()>0
 page.screenshot(path=str(ROOT/'tests/preview_v2.png'))
 assert not errors,errors
 print('PASS: 4 C++-defined tabs, '+str(len(state['nodes']))+' nodes, toggle, slider, section, input, dropdown, dialog, popup, theme, toast; 0 JS errors')
 browser.close()
