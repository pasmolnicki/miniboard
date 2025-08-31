#pragma once

#include "config.h"
#include "template_parser.h"

char* getPage();
void initPage();

constexpr const char PAGE_HTML[] = R"_(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8" />
<meta name="viewport" content="width=device-width, initial-scale=1" />
<title>Miniboard Keypad Setup</title>
<style>
    :root {
     --bg:#111; 
     --card:#1e1e1e; 
     --accent:#4caf50; 
     --accent2:#1976d2; 
     --text:#eee; 
     --muted:#888; 
     --danger:#e53935; 
}
    * { 
        box-sizing:border-box; 
    }
    body { 
        margin:0; 
        font-family:  system-ui,-apple-system,Segoe UI,Roboto,Arial,sans-serif;
        background:linear-gradient(135deg,#20242a,#111); 
        color:var(--text); 
        min-height:100vh; 
        display:flex; 
        flex-direction:column; 
    }
    header { padding:1rem 1.25rem; text-align:center; }
    h1 { font-size:1.6rem; margin:0.2rem 0 0.6rem; font-weight:600; }
    p.subtitle { margin:0; color:var(--muted); font-size:1rem; }
    main { width:100%; max-width:900px; margin:0 auto 2rem; padding:0 1rem 2rem; flex:1; }
    .grid { display:grid; gap:1rem; grid-template-columns:repeat(auto-fit,minmax(180px,1fr)); margin-top:1rem; }
    .card { background:var(--card); border:1px solid #2c2f33; padding:1rem; border-radius:12px; position:relative; box-shadow:0 4px 12px -4px #000 inset, 0 2px 6px -2px #000; }
    .card h2 { margin:0 0 .75rem; font-size:1.1rem; font-weight:600; letter-spacing:.5px; text-transform:uppercase; color:var(--accent2); }
    label { font-size:.8rem; text-transform:uppercase; letter-spacing:1px; font-weight:600; color:var(--muted); display:block; margin-bottom:.35rem; }
    select, input[type=number] { width:100%; padding:.55rem .6rem; border-radius:8px; background:#121416; border:1px solid #2d3238; color:var(--text); font-size:.85rem; font-family:inherit; outline:none; transition:border .15s, background .15s; }
    select:focus, input[type=number]:focus { border-color:var(--accent2); background:#171b1f; }
    .row { display:flex; gap:.6rem; }
    .row > div { flex:1; }
    .klabel { position:absolute; top:.6rem; right:.75rem; background:#263238; color:#b0bec5; font-size:.6rem; font-weight:700; padding:.22rem .45rem; border-radius:6px; letter-spacing:.5px; }
    footer { padding:1rem 1.25rem; text-align:center; font-size:.8rem; color:var(--muted); }
    button { cursor:pointer; font-family:inherit; font-weight:600; letter-spacing:.5px; border-radius:10px; border:1px solid transparent; padding:.75rem 1.1rem; font-size:.8rem; display:inline-flex; align-items:center; gap:.5rem; }
    button.primary { background:var(--accent); color:#fff; }
    button.secondary { background:#263238; color:#cfd8dc; }
    button.outline { background:transparent; border-color:#37474f; color:#b0bec5; }
    button.danger { background:var(--danger); color:#fff; }
    button:active { transform:translateY(1px); }
    .actions { display:flex; flex-wrap:wrap; justify-content: center; gap:.7rem; margin-top:1.4rem; }
    .status { margin-top:1.25rem; text-align:center; font-size:.8rem; min-height:1.2rem; }
    .pill { display:inline-flex; align-items:center; background:#263238; color:#b0bec5; padding:.65rem .85rem; border-radius:999px; font-size:.8rem; font-weight:600; gap:.6rem; }
    .inline { display:inline; }
    .fade-enter { animation:fade .25s ease; }
    @keyframes fade { from { opacity:0; transform:translateY(-4px);} to { opacity:1; transform:translateY(0);} }
    .kbd-preview { margin-top:.4rem; display:grid; grid-template-columns:repeat(4,1fr); gap:.4rem; }
    .kbd-preview span { background:#162027; padding:.55rem .4rem; border-radius:8px; font-size:.85rem; text-align:center; border:1px solid #253038; }
    .battery { font-weight:600; color:var(--accent); }
    a { color:var(--accent2); text-decoration:none; }
    a:hover { text-decoration:underline; }
    .flex { display:flex; gap:.6rem; align-items:center; flex-wrap:wrap; }
    .mt { margin-top:1.2rem; }
    .small { font-size:.65rem; color:var(--muted); text-align:center; }
</style>
</head>
<body>
    <header>
        <h1>Miniboard Keypad Setup</h1>
        <p class="subtitle">Configure the 4 hardware buttons and flash settings to EEPROM</p>
    </header>
    <main>
        <div class="flex">
            <div class="pill fade-enter" id="batteryPill">Battery: <span class="battery" id="batteryLevel">--%</span></div>
        </div>
        <section class="grid" id="keyGrid"></section>
        <div class="kbd-preview" id="preview"></div>
        <div class="actions">
            <button class="secondary" onclick="loadInfo()">Refresh</button>
            <button class="primary" onclick="save()">Save & Reboot</button>
            <button class="outline" onclick="resetToDefaults()">Defaults</button>
        </div>
        <div id="status" class="status"></div>
        <p class="small mt">After saving the device will reboot and start in BLE keyboard mode using the new layout.</p>
    </main>
    <footer>Miniboard &middot; ESP32 BLE Keyboard Config</footer>
<script>
// Key code catalog (matches values expected by BleKeyboard library constants, NOT raw HID usages)
// Special keys use library's extended codes (e.g. KEY_RETURN=0xB0, arrows 0xD7-0xDA, F-keys 0xC1+)
// Printable characters: we rely on their ASCII codes directly (letters, digits, space, symbols)
const KEY_OPTIONS = [
    // Modifiers
    {v:0x80,l:'Left Ctrl'},{v:0x81,l:'Left Shift'},{v:0x82,l:'Left Alt'},{v:0x83,l:'Left GUI'},
    {v:0x84,l:'Right Ctrl'},{v:0x85,l:'Right Shift'},{v:0x86,l:'Right Alt'},{v:0x87,l:'Right GUI'},
    // Navigation / control
    {v:0xB0,l:'Enter'},{v:0xB1,l:'Esc'},{v:0xB2,l:'Backspace'},{v:0xB3,l:'Tab'},
    {v:0xD1,l:'Insert'},{v:0xD4,l:'Delete'},{v:0xD2,l:'Home'},{v:0xD5,l:'End'},
    {v:0xD3,l:'Page Up'},{v:0xD6,l:'Page Down'},
    {v:0xD7,l:'Right Arrow'},{v:0xD8,l:'Left Arrow'},{v:0xD9,l:'Down Arrow'},{v:0xDA,l:'Up Arrow'},
    {v:0xC1,l:'Caps Lock'},
    // Function keys
    {v:0xC2,l:'F1'},{v:0xC3,l:'F2'},{v:0xC4,l:'F3'},{v:0xC5,l:'F4'},{v:0xC6,l:'F5'},{v:0xC7,l:'F6'},
    {v:0xC8,l:'F7'},{v:0xC9,l:'F8'},{v:0xCA,l:'F9'},{v:0xCB,l:'F10'},{v:0xCC,l:'F11'},{v:0xCD,l:'F12'},
    {v:0xF0,l:'F13'},{v:0xF1,l:'F14'},{v:0xF2,l:'F15'},{v:0xF3,l:'F16'},{v:0xF4,l:'F17'},{v:0xF5,l:'F18'},
    {v:0xF6,l:'F19'},{v:0xF7,l:'F20'},{v:0xF8,l:'F21'},{v:0xF9,l:'F22'},{v:0xFA,l:'F23'},{v:0xFB,l:'F24'},
    // Keypad (numeric pad) special (if desired)
    {v:0xEA,l:'Num 0'},{v:0xE1,l:'Num 1'},{v:0xE2,l:'Num 2'},{v:0xE3,l:'Num 3'},{v:0xE4,l:'Num 4'},
    {v:0xE5,l:'Num 5'},{v:0xE6,l:'Num 6'},{v:0xE7,l:'Num 7'},{v:0xE8,l:'Num 8'},{v:0xE9,l:'Num 9'},
    {v:0xDC,l:'Num /'},{v:0xDD,l:'Num *'},{v:0xDE,l:'Num -'},{v:0xDF,l:'Num +'},
    {v:0xE0,l:'Num Enter'},{v:0xEB,l:'Num .'},{v:0xCE,l:'Print Screen'},
    // Printable ASCII (common subset)
    {v:' '.charCodeAt(0),l:'Space'},
    // Digits
    ...'0123456789'.split('').map(ch=>({v:ch.charCodeAt(0),l:ch})),
    // Uppercase letters (will send lowercase w/ shift as needed by library mapping)
    ...'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.split('').map(ch=>({v:ch.charCodeAt(0),l:ch})),
    // Symbols (basic)
    {v:'-'.charCodeAt(0),l:'-'},{v:'='.charCodeAt(0),l:'='},{v:'['.charCodeAt(0),l:'['},{v:']'.charCodeAt(0),l:']'},
    {v:';'.charCodeAt(0),l:';'},{v:"'".charCodeAt(0),l:"'"},{v:','.charCodeAt(0),l:','},{v:'.'.charCodeAt(0),l:'.'},
    {v:'/'.charCodeAt(0),l:'/'},{v:'`'.charCodeAt(0),l:'`'},
];

// Will parse these values
const defaultKeymap = [<%k0%>,<%k1%>,<%k2%>,<%k3%>];
let currentKeymap = [...defaultKeymap];
let batteryTimer = null;

function el(id){ 
    return document.getElementById(id); 
}
function status(msg,type='info'){
    el('status').innerHTML = `<span class="${type}">${msg}</span>`; 
}

function optionList(selected){
    return KEY_OPTIONS.map(o=>`<option value="${o.v}" ${o.v==selected?'selected':''}>${o.l}</option>`).join('');
}

function renderGrid(){
    const grid = el('keyGrid');
    grid.innerHTML = currentKeymap.map((v,i)=>{
        return `<div class="card fade-enter"><div class="klabel">BTN ${i+1}</div><h2>Button ${i+1}</h2><label>Key Code</label><select onchange=changeKey(${i},this.value)>${optionList(v)}</select></div>`
    }).join('');
    renderPreview();
}

function renderPreview(){
    el('preview').innerHTML = currentKeymap.map(v=>`<span>${labelFor(v)}</span>`).join('');
}

function labelFor(v){ const o = KEY_OPTIONS.find(k=>k.v==v); return o?o.l:('0x'+Number(v).toString(16)); }

function changeKey(index,value){ currentKeymap[index]=parseInt(value); renderPreview(); }

function loadInfo(){
    fetch('/info').then(r=>r.json()).then(d=>{
        if(Array.isArray(d.keymap)){ currentKeymap = d.keymap.slice(0,4); }
        el('batteryLevel').textContent = (d.battery_level!==undefined? d.battery_level+'%':'--');
        renderGrid();
        status('Info loaded');
    }).catch(e=>{ status('Failed to load info','error'); console.error(e); });
}

function save(){
    status('Saving...');
    fetch('/save',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({keymap:currentKeymap})})
        .then(r=>r.json()).then(d=>{ status('Saved. Rebooting...'); })
        .catch(e=>{ status('Save failed','error'); console.error(e); });
}

// Defaults mirror DEFAULT_SETTINGS in firmware (Down, Up, Backspace, Enter)
function resetToDefaults(){ 
    currentKeymap=[...defaultKeymap]; 
    renderGrid(); 
    status('Defaults loaded (not saved)');
}

function pollBattery(){ fetch('/info').then(r=>r.json()).then(d=>{ el('batteryLevel').textContent=d.battery_level+'%'; }); }

window.addEventListener('load',()=>{ loadInfo(); batteryTimer=setInterval(pollBattery,20000); });
</script>
</body>
</html>
)_";