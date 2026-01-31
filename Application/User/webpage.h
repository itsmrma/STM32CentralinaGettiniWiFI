#ifndef WEBPAGE_H
#define WEBPAGE_H

const char HTML_HEADER[] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html\r\n"
"Connection: close\r\n"
"Pragma: no-cache\r\n\r\n"
"<!DOCTYPE html><html lang='it'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>"
"<title>Smart Garden Pro</title>"
/* Link Material Icons */
"<link href='https://fonts.googleapis.com/icon?family=Material+Icons' rel='stylesheet'>"
"<style>"
":root { --bg: #f0f2f5; --card: #ffffff; --primary: #2196F3; --accent: #4CAF50; --warn: #FF9800; --danger: #f44336; --text: #2c3e50; }"
"body { font-family: 'Segoe UI', Helvetica, Arial, sans-serif; background: var(--bg); color: var(--text); padding: 10px; margin: 0; }"
".container { max-width: 600px; margin: 0 auto; }"
/* Header */
".header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 20px; background: linear-gradient(135deg, var(--primary), #1976D2); color: white; padding: 15px; border-radius: 12px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }"
".header h2 { margin: 0; font-size: 1.2em; display:flex; align-items:center; gap:10px; } #clk { font-family: monospace; font-size: 1.4em; }"
/* Cards */
".dashboard { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; margin-bottom: 20px; }"
".card { background: var(--card); padding: 15px; border-radius: 12px; text-align: center; box-shadow: 0 2px 4px rgba(0,0,0,0.05); }"
".card .material-icons { font-size: 2em; margin-bottom: 5px; color: var(--primary); display: block; }"
".value { font-size: 1.3em; font-weight: bold; color: var(--text); }"
".unit { font-size: 0.7em; color: #7f8c8d; }"
/* Status Bar */
".status-bar { background: var(--card); padding: 15px; border-radius: 12px; margin-bottom: 20px; display: flex; flex-direction: column; gap: 8px; border-left: 6px solid #ccc; box-shadow: 0 2px 4px rgba(0,0,0,0.05); }"
".badge { display: inline-block; padding: 4px 10px; border-radius: 20px; color: white; font-size: 0.85em; font-weight: bold; }"
".bg-ok { background: var(--accent); border-color: var(--accent); } .bg-stop { background: var(--danger); border-color: var(--danger); } .bg-wait { background: #95a5a6; border-color: #95a5a6; }"
/* Forms */
".panel { background: var(--card); padding: 20px; border-radius: 12px; box-shadow: 0 2px 10px rgba(0,0,0,0.05); margin-bottom: 20px; }"
".panel h3 { display: flex; align-items: center; gap: 10px; }"
".form-row { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; margin-bottom: 15px; }"
"label { display: block; font-weight: 600; margin-bottom: 5px; font-size: 0.9em; }"
"input[type=number] { width: 100%; padding: 10px; border: 1px solid #ddd; border-radius: 8px; box-sizing: border-box; font-size: 1em; }"
/* Day Selector */
".day-selector { display: flex; justify-content: space-between; margin-top: 5px; }"
".day-selector label { font-weight: normal; cursor: pointer; display: flex; flex-direction: column; align-items: center; font-size: 0.8em; }"
".day-selector input { margin-bottom: 3px; }"
/* Simulator */
".sim-panel { border: 2px dashed var(--warn); background: #fff8e1; }"
".toggle-wrap { display: flex; align-items: center; gap: 10px; margin-bottom: 10px; }"
/* Button */
"input[type=submit] { background: var(--primary); color: white; border: none; padding: 15px; width: 100%; border-radius: 8px; cursor: pointer; font-size: 1.1em; font-weight: bold; transition: background 0.3s; display: flex; align-items: center; justify-content: center; gap: 10px; }"
"input[type=submit]:hover { background: #1565C0; }"
/* Utility align */
".v-center { display: flex; align-items: center; }"
"</style>"
"<script>"
"var h,m,s;"
"function startClock(HH,MM,SS){ h=HH; m=MM; s=SS; setInterval(tick,1000); tick(); }"
"function tick(){"
" s++; if(s>59){s=0; m++; if(m>59){m=0; h++; if(h>23)h=0;}}"
" var t = (h<10?'0'+h:h) + ':' + (m<10?'0'+m:m) + ':' + (s<10?'0'+s:s);"
" document.getElementById('clk').innerText = t;"
"}"
"</script>"
"</head><body>"
"<div class='container'>"
"<div class='header'>"
" <div><h2><span class='material-icons'>yard</span> Smart Garden</h2><small>Control Panel</small></div>"
" <div id='clk'>--:--:--</div>"
"</div>";

const char HTML_FOOTER[] =
"<div style='text-align:center; color:#999; font-size:0.8em; margin-top:30px; margin-bottom:20px;'>"
"STM32 IoT Project &bull; Unimore 2026"
"</div></div></body></html>\r\n";

#endif
