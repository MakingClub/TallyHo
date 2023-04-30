#include "../main.h"
#include "../config.h"
#include "EEPROM/ConfigData.h"
#include "LittleFS.h"

#include <ESP8266HTTPUpdateServer.h>
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

int status = WL_IDLE_STATUS;

const int SsidMaxLength = 24;
const int PassMaxLength = 24;
const int HostNameMaxLength = 24;
const int TallyNumberMaxValue = 64;

void tallyPageHandler();
void rootPageHandler();
void SaveHandler();
void UpdataHandler();

// WebServer Handler
void webInit(void) {
  LittleFS.begin();
  httpServer.on("/", HTTP_GET, rootPageHandler);
  httpServer.on("/save", HTTP_POST, SaveHandler);
  httpServer.on("/tally", HTTP_GET, tallyPageHandler);
  httpServer.on("/zend", []() {
    //httpServer.sendHeader("Connection", "close");
    httpServer.sendHeader("Cache-Control", "no-cache");
    httpServer.sendHeader("Access-Control-Allow-Origin", "*");

    if (Var.currentState != Var.oldState) {
        httpServer.send(200, "text/event-stream", "event: message\ndata: refresh" + String(Var.vmixcon) + "\nretry: 500\n\n");
        Var.oldState = Var.currentState;
    } else {
        httpServer.send(200, "text/event-stream", "event: message\ndata: norefresh\nretry: 500\n\n");
    }

    //Serial.print(F("| FreeHeap: "));
    //Serial.println(ESP.getFreeHeap(),DEC);
  });
  // handler for the /update form page
  httpServer.on("/otaupdate", HTTP_GET, UpdataHandler);  // OTA
  httpUpdater.setup(&httpServer, "/update", "tallyHo", "goodlife");
  httpServer.serveStatic("/", LittleFS, "/", "max-age=315360000");
  httpServer.begin();
}
// web close 
void webClose(void) {
  //httpServer.close();
}
// Web Loop
void webLoop(void) {
    httpServer.handleClient();
}
// Web status
uint8_t webStatus(void) {
  return 0;
}
// Handle http server Tally request
void tallyPageHandler() {
  String response_message = F("<!DOCTYPE html>");
  response_message += F("<html lang='cn'>");
  response_message += F("<head>");
  response_message += F("<title>vTally by VID-PRO - ") + String(setting.tallyName) + F("</title>");
  response_message += F("<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>");
  response_message += F("<meta charset='utf-8'>");
  response_message += F("<link rel='icon' type='image/x-icon' href='favicon.png'>");
  response_message += F("<link rel='stylesheet' href='styles.css'>");
  response_message += F("<style>body {width: 100%;height: 100%;padding: 0px;}</style>");
  response_message += F("</head>");

  response_message += F("<body class='bg-light'>");

  response_message += F("<table class='table'><tbody  style='border-radius: 0px 0px 10px 10px;background-color:#d5dadd;'>");

  response_message += F("<tr><th style='background-color:rgb(");

  if (Var.vmixcon == 0) {
    Var.currentState = '3';
  }
  if (Var.vmixcon == 1 && (Var.currentState != '0' && Var.currentState != '1' && Var.currentState != '2')) {
    Var.currentState = 4;
  }

  switch (Var.currentState)
  {
    case '0':
      response_message += String(setting.offred) + F(",") + String(setting.offgreen) + F(",") + String(setting.offblue); //off
      break;
    case '1':
      response_message += String(setting.prgred) + F(",") + String(setting.prggreen) + F(",") + String(setting.prgblue); //prg
      break;
    case '2':
      response_message += String(setting.prvred) + F(",") + String(setting.prvgreen) + F(",") + String(setting.prvblue); //prv
      break;
    case '3':
      response_message += F("255,0,0"); // no vMix Server
      break;
    case '4':
      response_message += F("255,255,0"); // no vMix Server
      break;
    default:
      response_message += String(setting.offred) + F(",") + String(setting.offgreen) + F(",") + String(setting.offblue); //default off
  }
  response_message += F(");'>&nbsp;</th><tr>");


  response_message += F("<tr><th style='text-align:center;background-color:rgb(");
  switch (Var.currentState)
  {
    case '0':
      response_message += String(setting.offred) + F(",") + String(setting.offgreen) + F(",") + String(setting.offblue); //off
      break;
    case '1':
      response_message += String(setting.prgred) + F(",") + String(setting.prggreen) + F(",") + String(setting.prgblue); //prg
      break;
    case '2':
      response_message += String(setting.prvred) + F(",") + String(setting.prvgreen) + F(",") + String(setting.prvblue); //prv
      break;
    case '3':
      response_message += F("255,0,0"); // no vMix Server
      break;
    case '4':
      response_message += F("255,255,0"); // no vMix Server
      break;
    default:
      response_message += String(setting.offred) + F(",") + String(setting.offgreen) + F(",") + String(setting.offblue); //default off
  }
  response_message += F(");color:");
  switch (Var.currentState)
  {
    case '0':
      response_message += F("#ffffff"); //off
      break;
    case '1':
      response_message += F("#ffffff"); //prg
      break;
    case '2':
      response_message += F("#ffffff"); //prv
      break;
    case '3':
      response_message += F("#ffffff"); // no vMix Server
      break;
    case '4':
      response_message += F("#ffffff"); // no vMix Server
      break;
    default:
      response_message += F("#ffffff"); //default off
  }
  response_message += F("'>");
  switch (Var.currentState)
  {
    case '0':
      response_message += F("关闭"); //off
      break;
    case '1':
      response_message += F("播出"); //prg
      break;
    case '2':
      response_message += F("预选"); //prv
      break;
    case '3':
      response_message += F("未发现vMix服务器"); // no vMix Server
      break;
    case '4':
      response_message += F("连接到vMix服务器,等待数据"); // no vMix Server
      break;
    default:
      response_message += F("关闭"); //default off
  }
  response_message += F("</th></tr>");


  response_message += F("<tr><th style='background-color:rgb(");
  switch (Var.currentState)
  {
    case '0':
      response_message += String(setting.offred) + F(",") + String(setting.offgreen) + F(",") + String(setting.offblue); //off
      break;
    case '1':
      response_message += String(setting.prgred) + F(",") + String(setting.prggreen) + F(",") + String(setting.prgblue); //prg
      break;
    case '2':
      response_message += String(setting.prvred) + F(",") + String(setting.prvgreen) + F(",") + String(setting.prvblue); //prv
      break;
    case '3':
      response_message += F("255,0,0"); // no vMix Server
      break;
    case '4':
      response_message += F("255,255,0"); // no vMix Server
      break;
    default:
      response_message += String(setting.offred) + F(",") + String(setting.offgreen) + F(",") + String(setting.offblue); //default off
  }
  response_message += F(");'>&nbsp;</th><tr>");

  response_message += F("<tr><th style='border-radius: 0px 0px 10px 10px;background-color:rgb(");
  switch (Var.currentState)
  {
    case '0':
      response_message += String(setting.offred) + F(",") + String(setting.offgreen) + F(",") + String(setting.offblue); //off
      break;
    case '1':
      response_message += String(setting.prgred) + F(",") + String(setting.prggreen) + F(",") + String(setting.prgblue); //prg
      break;
    case '2':
      response_message += String(setting.prvred) + F(",") + String(setting.prvgreen) + F(",") + String(setting.prvblue); //prv
      break;
    case '3':
      response_message += F("255,0,0"); // no vMix Server
      break;
    case '4':
      response_message += F("255,255,0"); // no vMix Server
      break;
    default:
      response_message += String(setting.offred) + F(",") + String(setting.offgreen) + F(",") + String(setting.offblue); //default off
  }
  response_message += F(");'>&nbsp;</th><tr>");

  response_message += F("</tbody></table>");

  response_message += F("</body></html>");

  httpServer.sendHeader("Connection", "close");
  httpServer.send(200, "text/html", String(response_message));

  Serial.print(F("| FreeHeap: "));
  Serial.println(ESP.getFreeHeap(),DEC);
}
// Handle http server root request
void rootPageHandler() {
  String response_message = F("<!DOCTYPE html>");
  response_message += F("<html lang='cn'>");
  response_message += F("<head>");
  response_message += F("<title>TallyHo by MakingClub - ") + String(setting.tallyName) + F("</title>");
  response_message += F("<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>");
  response_message += F("<meta charset='utf-8'>");
  response_message += F("<link rel='icon' type='image/x-icon' href='favicon.ico'>");
  response_message += F("<link rel='stylesheet' href='styles.css'>");
  response_message += F("<script src='jquery.slim.min.js'></script>");
  response_message += F("<script type='text/javascript'>");
  response_message += F("var ESPurl = 'http://") + WiFi.localIP().toString() + F("/zend';");
  response_message += F("if(typeof(EventSource) !== 'undefined') {");
  response_message += F("  var source = new EventSource(ESPurl);");
  response_message += F("  source.onmessage = function(event) {");
  response_message += F("    if(event.data == 'refresh0' || event.data == 'refresh1') {");
  response_message += F("      $('#tally').html(\"<object type='text/html' data='/tally' style='height:100%;width:100%;'></oject>\");");
  response_message += F("      if(event.data == 'refresh1') {");
  response_message += F("        $('#vmixstatus').html(\"连接\");");
  response_message += F("        $('#vmixstatus').css(\"background-color\",\"green\");");
  response_message += F("      }");
  response_message += F("      if(event.data == 'refresh0') {");
  response_message += F("        $('#vmixstatus').html(\"未连接\");");
  response_message += F("        $('#vmixstatus').css(\"background-color\",\"red\");");
  response_message += F("      }");
  response_message += F("    }");
  response_message += F("  }");
  response_message += F("} else {");
  response_message += F("  document.getElementById('tally').innerHTML('Your browser does not support EventSource!')");
  response_message += F("}");

  response_message += F("</script>");
  response_message += F("<style>body {width: 100%;height: 100%;padding: 25px;}</style>");

  response_message += F("<link rel='stylesheet' href='https://cdn.staticfile.org/font-awesome/4.7.0/css/font-awesome.css'>");
  response_message += F("</head>");

  response_message += F("<body class='bg-light'>");

  response_message += F("<h1 style='border-radius: 10px 10px 0px 0px;background-color:#0066ff;color:#FFFFFF;text-align:center;padding:5px;margin-bottom:0px;'><span style='vertical-align:text-top;'>vTally by </span><a href=https://www.calihc.de target=_new><img src='logo.png' style='vertical-align:bottom;'></a></h1>");

  response_message += F("<h1 style='border-radius:0px 0px 10px 10px;background-color:#c6cdd2;text-align:center;'>vTally ID: ") + String(setting.tallyNumber) + F("</h1>");

  response_message += F("<form action='/save' method='post' enctype='multipart/form-data' data-ajax='false'>");

  response_message += F("<div data-role='content' class='row' style='margin-bottom:.5rem;'>");

  response_message += F("<div class='col-md-6'>");
  response_message += F("<h2 style='border-radius: 10px 10px 0px 0px;margin-bottom:0px;background-color:#99c2ff;padding:2px;'>&nbsp; 网络/vMix/VISCA设置</h2>");

  response_message += F("<table class='table' style='border-radius: 0px 0px 10px 10px;'><tbody  style='border-radius: 0px 0px 10px 10px;background-color:#d5dadd;'><tr style='border-radius: 0px 0px 10px 10px;'><td style='border-radius: 0px 0px 10px 10px;'>");

  response_message += F("<div class='form-group row'>");
  response_message += F("<label for='ssid' class='col-sm-4 col-form-label'>WiFi 名称</label>");
  response_message += F("<div class='col-sm-8'>");
  response_message += F("<input id='ssid' class='form-control' type='text' size='64' maxlength='32' name='ssid' value='") + String(setting.ssid) + F("'>");
  response_message += F("</div></div>");

  response_message += F("<div class='form-group row'>");
  response_message += F("<label for='ssidpass' class='col-sm-4 col-form-label'>WiFi 密码</label>");
  response_message += F("<div class='col-sm-8'>");
  response_message += F("<input id='ssidpass' class='form-control' type='password' autocomplete='current-password' size='64' maxlength='32' name='ssidpass' value='") + String(setting.pass) + F("'>");
  response_message += F("</div></div>");

  response_message += F("<hr>");

  response_message += F("<div class='form-group row'>");
  response_message += F("<label for='hostname' class='col-sm-4 col-form-label'>vMix服务器IP/IP</label>");
  response_message += F("<div class='col-sm-8'>");
  response_message += F("<input id='hostname' class='form-control' type='text' size='64' maxlength='32' name='hostname' value='") + String(setting.hostName) + F("'>");
  response_message += F("</div></div>");

  response_message += F("<div class='form-group row'>");
  response_message += F("<label for='inputnumber' class='col-sm-4 col-form-label'>Tally灯序号(1-") + String(TallyNumberMaxValue) + F(")</label>");
  response_message += F("<div class='col-sm-8'>");
  response_message += F("<input id='inputnumber' class='form-control' type='number' size='64' min='0' max='") + String(TallyNumberMaxValue) + F("' name='inputnumber' value='") + String(setting.tallyNumber) + F("'>");
  response_message += F("</div></div>");

  response_message += F("<hr>");

  // response_message += F("<div class='form-group row'>");
  // response_message += F("<label for='inputnumber' class='col-sm-4 col-form-label'>VISCA Baudrate (9600)</label>");
  // response_message += F("<div class='col-sm-8'>");
  // response_message += F("<select id='viscabaud' name='viscabaud' class='form-control'>");
  // response_message += F("<option value='4800' ");
  // (String(setting.viscabaud) == "4800") ? response_message += "selected" : response_message += "";
  // response_message += F(">4800</option>");
  // response_message += F("<option value='9600' ");
  // (String(setting.viscabaud) == "9600") ? response_message += "selected" : response_message += "";
  // response_message += F(">9600</option>");
  // response_message += F("<option value='14400' ");
  // (String(setting.viscabaud) == "14400") ? response_message += "selected" : response_message += "";
  // response_message += F(">14400</option>");
  // response_message += F("<option value='19200' ");
  // (String(setting.viscabaud) == "19200") ? response_message += "selected" : response_message += "";
  // response_message += F(">19200</option>");
  // response_message += F("<option value='57600' ");
  // (String(setting.viscabaud) == "57600") ? response_message += "selected" : response_message += "";
  // response_message += F(">57600</option>");
  // response_message += F("<option value='115200' ");
  // (String(setting.viscabaud) == "115200") ? response_message += "selected" : response_message += "";
  // response_message += F(">115200</option>");
  // response_message += F("</select>");
  // //response_message += F("<input id='viscabaud' class='form-control' type='number' size='64' min='2400' max='115200' name='viscabaud' value='") + String(setting.viscabaud) + F("'>");
  // response_message += F("</div></div>");

  // response_message += F("<div class='form-group row'>");
  // response_message += F("<label for='viscaport' class='col-sm-4 col-form-label'>VISCA UDP Port (52381)</label>");
  // response_message += F("<div class='col-sm-8'>");
  // response_message += F("<input id='viscaport' class='form-control' type='number' size='64' min='1024' max='65554' name='viscaport' value='") + String(setting.viscaport) + F("'>");
  // response_message += F("</div></div>");
  response_message += F("<div class='form-group row'>");
  response_message += F("<label class='col-sm-4 col-form-label'>&nbsp;</label>");
  response_message += F("<div class='col-sm-8'>&nbsp;</div></div>");

  response_message += F("<div class='form-group row'>");
  response_message += F("<label class='col-sm-4 col-form-label'>&nbsp;</label>");
  response_message += F("<div class='col-sm-8'>&nbsp;</div></div>");

  response_message += F("<div class='form-group row' style='border-radius:0px 0px 0px 10px;background-color:c6cdd2;'>");
  response_message += F("<label for='save' class='col-sm-4 col-form-label'> </label>");
  response_message += F("<div class='col-sm-8' style='border-radius:0px 0px 10px 0px;text-align:center;'>");
  response_message += F("<input type='submit' value='保存' class='btn btn-primary'>");
  response_message += F("</div></div>");

  response_message += F("</td></tr></tbody></table>");

  response_message += F("</div>");

  response_message += F("<div class='col-md-6'>");
  response_message += F("<h2 style='border-radius: 10px 10px 0px 0px;margin-bottom:0px;background-color:#99c2ff;padding:2px;'>&nbsp;   vMix Tally设置</h2>");

  response_message += F("<table class='table' style='height:90%;border-radius: 0px 0px 10px 10px;'><tbody  style='border-radius: 0px 0px 10px 10px;background-color:#d5dadd;'><tr style='border-radius: 0px 0px 10px 10px;'><td style='border-radius: 0px 0px 10px 10px;'>");

  response_message += F("<div class='form-group row'>");
  response_message += F("<label for='prg' class='col-sm-4 col-form-label'>直播RGB颜色(0-254)</label>");
  response_message += F("<div class='col-sm-8'>");
  response_message += F("<div class='form-group row' style='margin-bottom:0px;'>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;R&nbsp;</div><div>");
  response_message += F("<input id='prgred' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='prgred' value='") + String(setting.prgred) + F("' onchange=\"document.getElementById('pprg').style.backgroundColor = 'rgb('+document.getElementById('prgred').value+','+document.getElementById('prggreen').value+','+document.getElementById('prgblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;G&nbsp;</div><div>");
  response_message += F("<input id='prggreen' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='prggreen' value='") + String(setting.prggreen) + F("'onchange=\"document.getElementById('pprg').style.backgroundColor = 'rgb('+document.getElementById('prgred').value+','+document.getElementById('prggreen').value+','+document.getElementById('prgblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;B&nbsp;</div><div>");
  response_message += F("<input id='prgblue' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='prgblue' value='") + String(setting.prgblue) + F("'onchange=\"document.getElementById('pprg').style.backgroundColor = 'rgb('+document.getElementById('prgred').value+','+document.getElementById('prggreen').value+','+document.getElementById('prgblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div>&nbsp;&nbsp;</div>");
  response_message += F("<div id='pprg' style='display: -webkit-flex;display: flex;align-items: center;color:#ffffff;background-color:rgb(") + String(setting.prgred) + F(",") + String(setting.prggreen) + F(",") + String(setting.prgblue) + F(")'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;直播&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>");
  response_message += F("</div></div></div>");

  response_message += F("<div class='form-group row'>");
  response_message += F("<label for='prv' class='col-sm-4 col-form-label'>预选RGB颜色(0-254)</label>");
  response_message += F("<div class='col-sm-8'>");
  response_message += F("<div class='form-group row' style='margin-bottom:0px;'>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;R&nbsp;</div><div>");
  response_message += F("<input id='prvred' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='prvred' value='") + String(setting.prvred) + F("' onchange=\"document.getElementById('pprv').style.backgroundColor = 'rgb('+document.getElementById('prvred').value+','+document.getElementById('prvgreen').value+','+document.getElementById('prvblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;G&nbsp;</div><div>");
  response_message += F("<input id='prvgreen' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='prvgreen' value='") + String(setting.prvgreen) + F("' onchange=\"document.getElementById('pprv').style.backgroundColor = 'rgb('+document.getElementById('prvred').value+','+document.getElementById('prvgreen').value+','+document.getElementById('prvblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;B&nbsp;</div><div>");
  response_message += F("<input id='prvblue' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='prvblue' value='") + String(setting.prvblue) + F("' onchange=\"document.getElementById('pprv').style.backgroundColor = 'rgb('+document.getElementById('prvred').value+','+document.getElementById('prvgreen').value+','+document.getElementById('prvblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div>&nbsp;&nbsp;</div>");
  response_message += F("<div id='pprv' style='display: -webkit-flex;display: flex;align-items: center;color:#ffffff;background-color:rgb(") + String(setting.prvred) + F(",") + String(setting.prvgreen) + F(",") + String(setting.prvblue) + F(")'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;预选&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>");
  response_message += F("</div></div></div>");

  response_message += F("<div class='form-group row'>");
  response_message += F("<label for='off' class='col-sm-4 col-form-label'>指示RGB颜色(0-254)</label>");
  response_message += F("<div class='col-sm-8'>");
  response_message += F("<div class='form-group row' style='margin-bottom:0px;'>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;R&nbsp;</div><div>");
  response_message += F("<input id='offred' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='infored' value='") + String(setting.infored) + F("' onchange=\"document.getElementById('poff').style.backgroundColor = 'rgb('+document.getElementById('offred').value+','+document.getElementById('offgreen').value+','+document.getElementById('offblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;G&nbsp;</div><div>");
  response_message += F("<input id='offgreen' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='infogreen' value='") + String(setting.infogreen) + F("' onchange=\"document.getElementById('poff').style.backgroundColor = 'rgb('+document.getElementById('offred').value+','+document.getElementById('offgreen').value+','+document.getElementById('offblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;B&nbsp;</div><div>");
  response_message += F("<input id='offblue' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='infoblue' value='") + String(setting.infoblue) + F("' onchange=\"document.getElementById('poff').style.backgroundColor = 'rgb('+document.getElementById('offred').value+','+document.getElementById('offgreen').value+','+document.getElementById('offblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div>&nbsp;&nbsp;</div>");
  response_message += F("<div id='poff' style='display: -webkit-flex;display: flex;align-items: center;color:#ffffff;background-color:rgb(") + String(setting.infored) + F(",") + String(setting.infogreen) + F(",") + String(setting.infoblue) + F(")'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;指示&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>");
  response_message += F("</div></div></div>");

  response_message += F("<div class='form-group row'>");
  response_message += F("<label for='off' class='col-sm-4 col-form-label'>关闭RGB颜色(0-254)</label>");
  response_message += F("<div class='col-sm-8'>");
  response_message += F("<div class='form-group row' style='margin-bottom:0px;'>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;R&nbsp;</div><div>");
  response_message += F("<input id='offred' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='offred' value='") + String(setting.offred) + F("' onchange=\"document.getElementById('poff').style.backgroundColor = 'rgb('+document.getElementById('offred').value+','+document.getElementById('offgreen').value+','+document.getElementById('offblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;G&nbsp;</div><div>");
  response_message += F("<input id='offgreen' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='offgreen' value='") + String(setting.offgreen) + F("' onchange=\"document.getElementById('poff').style.backgroundColor = 'rgb('+document.getElementById('offred').value+','+document.getElementById('offgreen').value+','+document.getElementById('offblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div style='display: -webkit-flex;display: flex;align-items: center'>&nbsp;B&nbsp;</div><div>");
  response_message += F("<input id='offblue' class='form-control' type='number' size='21' min='0' max='254' style='width:5em;' name='offblue' value='") + String(setting.offblue) + F("' onchange=\"document.getElementById('poff').style.backgroundColor = 'rgb('+document.getElementById('offred').value+','+document.getElementById('offgreen').value+','+document.getElementById('offblue').value+')'\">");
  response_message += F("</div>");
  response_message += F("<div>&nbsp;&nbsp;</div>");
  response_message += F("<div id='poff' style='display: -webkit-flex;display: flex;align-items: center;color:#ffffff;background-color:rgb(") + String(setting.offred) + F(",") + String(setting.offgreen) + F(",") + String(setting.offblue) + F(")'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关闭&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>");
  response_message += F("</div></div></div>");

  response_message += F("<hr>");

  response_message += F("<div class='form-group row' style='background-color=#dcdcdc'>");
  response_message += F("<label for='intensFull' class='col-sm-4 col-form-label'>预选/直播亮度(0-100)</label>");
  response_message += F("<div class='col-sm-8'>");
  response_message += F("<input id='intensFull' class='form-control' type='number' style='width:5em;' size='8' min='0' max='100' name='intensFull' value='") + String(setting.intensFull) + F("'>");
  response_message += F("</div></div>");

  response_message += F("<div class='form-group row'>");
  response_message += F("<label for='intensDim' class='col-sm-4 col-form-label'>关闭亮度(0-100)</label>");
  response_message += F("<div class='col-sm-8'>");
  response_message += F("<input id='intensDim' class='form-control' type='number' style='width:5em;' size='8' min='0' max='100' name='intensDim' value='") + String(setting.intensDim) + F("'>");
  response_message += F("</div></div>");

  response_message += F("<hr>");

  // response_message += F("<div class='form-group row'>");
  // response_message += F("<label class='col-sm-4 col-form-label'>&nbsp;</label>");
  // response_message += F("<div class='col-sm-8'>&nbsp;</div></div>");

  response_message += F("<div class='form-group row' style='background-color:c6cdd2;'>");
  response_message += F("<label for='save' class='col-sm-4 col-form-label'> </label>");
  response_message += F("<div class='col-sm-8' style='text-align:center;'>");
  response_message += F("<input type='submit' value='保存' class='btn btn-primary'>");
  response_message += F("</div></div>");

  response_message += F("</td></tr></tbody></table>");

  response_message += F("</div></div></div>");
  response_message += F("</form>");

  response_message += F("<div data-role='content' class='row'>");
  response_message += F("&nbsp;</div>");

  response_message += F("<div data-role='content' class='row'>");

  response_message += F("<div class='col-md-6'>");
  response_message += F("<h2 style='border-radius: 10px 10px 0px 0px;margin-bottom:0px;background-color:#99c2ff;padding:2px;'>&nbsp; 设备信息</h2>");
  response_message += F("<table class='table'><tbody  style='border-radius: 0px 0px 10px 10px;background-color:#d5dadd;'>");

  char ip[13];
  sprintf(ip, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  response_message += F("<tr><th>IP地址</th><td>") + String(ip) + F("</td><th>WiFi信号强度</th><td style='background-color:");

  String color = "#ff0000";

  if (Var.WiFiRSSI > -80) {
    color = "#ff0000";
  }
  else if (Var.WiFiRSSI > -67) {
    color = "#ffff00";
  }
  else if (Var.WiFiRSSI > -50) {
    color = "#00ff00";
  }

  response_message += color;

  response_message += F(";'>") + String(Var.WiFiRSSI) + F(" dBm</td></tr>");

  response_message += F("<tr><th>MAC</th><td>") + String(WiFi.macAddress()) + F("</td>");

  response_message += F("<th>WiFi AP</th>");
  if (Var.apEnabled)
  {
    sprintf(ip, "%d.%d.%d.%d", WiFi.softAPIP()[0], WiFi.softAPIP()[1], WiFi.softAPIP()[2], WiFi.softAPIP()[3]);
    response_message += F("<td style='background-color:green;color:white;'>运行中 (") + String(ip) + F(")");
  }
  else
  {
    response_message += F("<td style='background-color:yellow;'>未运行");
  }
  response_message += F("</td>");

  response_message += F("</tr>");

  response_message += F("<tr><th>设备名称</th><td>") + String(setting.tallyName) + F("</td>");
  response_message += F("<th>vMix状态</th><td id='vmixstatus'");
  if (Var.vmixcon == 1)
  {
    response_message += F("style='background-color:green;color:white;'>已连接");
  }
  else
  {
    response_message += F("style='background-color:red;color:white;'>未连接");
  }
  response_message += F("</td>");
  response_message += F("</tr>");

  response_message += F("<tr><th style='border-radius: 0px 0px 0px 10px;'>固件版本</th><td>")+ String(Vers) + F("</td>");
  response_message += F("<th>电池电量</th><td id='BatV'");
  if (Var.BatPrecent > 60) {
    response_message += F("style='background-color:green;color:white;'>")+ String(Var.BatPrecent);
  }
  else {
    response_message += F("style='background-color:yellow;'>")+ String(Var.BatPrecent);
  }
//   if (udpstate == 1)
//   {
//     response_message += F("style='border-radius: 0px 0px 10px 0px;background-color:green;color:white;'>Running");
//   }
//   else
//   {
//     response_message += F("style='border-radius: 0px 0px 10px 0px;background-color:red;color:white;'>Not Running");
//   }
  response_message += F("</td>");

  response_message += F("</tr>");

  response_message += F("</tbody></table>");
  response_message += F("</div>");



  response_message += F("<div class='col-md-6'>");
  response_message += F("<h2 style='border-radius: 10px 10px 0px 0px;margin-bottom:0px;background-color:#99c2ff;padding:2px;'>&nbsp; vMix Tally</h2>");

  response_message += F("<div id='tally' style='height:90%;'></div>");
  response_message += F("<script>document.getElementById('tally').innerHTML=\"<object type='text/html' data='/tally' style='height:100%;width:100%;'></object>\";</script>");

  response_message += F("</div>");



  response_message += F("</div>");

  response_message += F("<h4 style='border-radius: 10px 10px 10px 10px;background-color:#c8c8c8;text-align:center;margin-bottom:0px;'>vTally v") + String(Vers) + F(" &nbsp;&nbsp;&nbsp; &copy; 2022 by <a href=https://www.vid-pro.de target=_new>VID-PRO</a>");
  response_message += F("&nbsp;&nbsp;&nbsp; &copy; 2023 by <a href=https://space.bilibili.com/1369737196 target=_new>MAKING-CLUB</a>");
  response_message += F("&nbsp;&nbsp;&nbsp; <a href=https://space.bilibili.com/1369737196 target=_new><img src='bilibili.png' style='vertical-align:bottom;'></a>");
  response_message += F("</h4>");

  response_message += F("</body>");
  response_message += F("</html>");

  httpServer.sendHeader("Connection", "close");
  httpServer.send(200, "text/html", String(response_message));
}
// Updata Handler
void UpdataHandler() {
  Serial.print(F("| FreeHeap 2: "));
  Serial.println(ESP.getFreeHeap(),DEC);


  String response_message = F("<!DOCTYPE html>");
  response_message += F("<html lang=\"cn\">");
  response_message += F("<head>");
  response_message += F("<meta charset=\"UTF-8\">");
  response_message += F("<title>Tally-Ho固件升级</title>");
  response_message += F("<style type=\"text/css\">");
  response_message += F("* {");
  response_message += F("box-sizing: border-box;");
  response_message += F("}\n");

  response_message += F("body {");
  response_message += F("font-family: Roboto, sans-serif;");
  response_message += F("background: #555;\n");

  response_message += F("text-align: center;");
  response_message += F("min-width: 360px;");
  response_message += F("}\n");

  response_message += F(".button-send {");
  response_message += F("width: 100%;");
  // response_message += F("border: 0;");
  response_message += F("background: #FFF;");
  response_message += F("line-height: 23px;");
  response_message += F("font-weight: bold;");
  response_message += F("color: #555;");
  response_message += F("border-radius: 4px;");
  response_message += F("box-shadow: inset 0 -2px 3px rgba(0,0,0,.4), 0 2px 5px rgba(0,0,0,0.5);");
  response_message += F("}\n");

  response_message += F(".button-send:hover {");
  response_message += F("background-color: #dedede;");
  response_message += F("}\n");

  response_message += F(".button-send:active {");
  response_message += F("background-color: #dedede;");
  response_message += F("box-shadow: 1px 1px #666;");
  response_message += F("transform: translateY(4px);");
  response_message += F("}");
  response_message += F(".button-send:disabled {");
  response_message += F("background-color: #9c9c9c;");
  response_message += F("}\n");

  response_message += F(".container {");
  response_message += F("padding-top: 80px;");
  response_message += F("display: flex;");
  response_message += F("justify-content: center;");
  response_message += F("}\n");

  response_message += F(".form-container {");
  response_message += F("min-width: 380px;");
  response_message += F("}\n");

  response_message += F("progress {");
  response_message += F("display: block;");
  response_message += F("margin: 2em auto;");
  response_message += F("padding: 4px;");
  response_message += F("background: #444;");
  response_message += F("border-radius: 14px;");
  response_message += F("box-shadow: inset 0px 1px 1px rgba(0,0,0,0.5), 0px 1px 0px rgba(255,255,255,0.2);");
  response_message += F("}");
  response_message += F("progress::-moz-progress-bar {");
  response_message += F("border-radius: 12px;");
  response_message += F("background: #FFF;");
  response_message += F("box-shadow: inset 0 -2px 4px rgba(0,0,0,0.4), 0 2px 5px 0px rgba(0,0,0,0.3);\n");

  response_message += F("}");
  response_message += F("@media screen and (-webkit-min-device-pixel-ratio:0) {");
  response_message += F("progress {");
  response_message += F("height: 25px;");
  response_message += F("}");
  response_message += F("}");
  response_message += F("progress::-webkit-progress-bar {");
  response_message += F("background: transparent;");
  response_message += F("}");
  response_message += F("progress::-webkit-progress-value {");
  response_message += F("border-radius: 12px;");
  response_message += F("background: #FFF;");
  response_message += F("box-shadow: inset 0 -2px 4px rgba(0,0,0,0.4), 0 2px 5px 0px rgba(0,0,0,0.3);");
  response_message += F("}\n");

  response_message += F("h1 {");
  response_message += F("color: #eee;");
  response_message += F("font: 50px Helvetica, Arial, sans-serif;");
  response_message += F("text-shadow: 0px 1px black;");
  response_message += F("text-align: center;");
  response_message += F("-webkit-font-smoothing: antialiased;");
  response_message += F("}\n");

  response_message += F(".input-file {");
  response_message += F("padding: 4px 4px 4px 4px;");
  response_message += F("margin-top: 20px;");
  response_message += F("border-radius: 7px;");
  response_message += F("background-color: rgb(255 255 255);");
  response_message += F("}\n");

  response_message += F(".additional-info {");
  response_message += F("color: white;");
  response_message += F("height: 67px;");
  response_message += F("}\n");

  response_message += F("@-webkit-keyframes progress-animation {");
  response_message += F("0% {");
  response_message += F("background-position: 100%;");
  response_message += F("}");
  response_message += F("100% {");
  response_message += F("background-position: 0;");
  response_message += F("}");
  response_message += F("}\n");

  response_message += F("@keyframes progress-animation {");
  response_message += F("0% {");
  response_message += F("background-position: 100%;");
  response_message += F("}");
  response_message += F("100% {");
  response_message += F("background-position: 0;");
  response_message += F("}");
  response_message += F("}\n");

  response_message += F(".tile-container {");
  response_message += F("display: flex;");
  response_message += F("justify-content: center;");
  response_message += F("}\n");

  response_message += F(".title-image {");
  response_message += F("background-repeat: no-repeat;");
  response_message += F("background-position: center;");
  response_message += F("background-size: 40px;");
  response_message += F("width: 40px;");
  response_message += F("margin-right: 20px;");
  response_message += F("}\n");

  response_message += F(".footer-desc {");
  response_message += F("position: fixed;");
  response_message += F("display: flex;");
  response_message += F("bottom: 20px;");
  response_message += F("right: 20px;");
  response_message += F("align-items: center;");
  response_message += F("}\n");

  response_message += F(".footer-link {");
  response_message += F("font-size: x-small;");
  response_message += F("color: black;");
  // response_message += F("text-decoration: none;");
  response_message += F("padding-left: 10px;");
  response_message += F("}\n");

  response_message += F(".button-o {");
  response_message += F("cursor: pointer;");
  response_message += F("height: 25px;");
  response_message += F("font-size: 15px;");
  response_message += F("border: 1px solid rgba(0, 0, 0, 0.35);");
  response_message += F("width: 50%;");
  response_message += F("color: white;");
  response_message += F("background: black;");
  response_message += F("}\n");

  response_message += F(".button-o.selected {");
  response_message += F("background-color: white;");
  response_message += F("color: black;");
  response_message += F("}\n");

  response_message += F(".button-o.left {");
  response_message += F("border-radius: 14px 0px 0px 14px;");
  response_message += F("margin-right: -3px;");
  response_message += F("}\n");

  response_message += F(".button-o.right {");
  response_message += F("border-radius: 0px 14px 14px 0px;");
  response_message += F("margin-left: -3px;");
  response_message += F("}\n");

  response_message += F("#overlay {");
  response_message += F("position: fixed;");
  // response_message += F("display: none;");
  // response_message += F("width: 100%;");
  // response_message += F("height: 100%;");
  // response_message += F("top: 0;");
  // response_message += F("left: 0;");
  // response_message += F("right: 0;");
  // response_message += F("bottom: 0;");
  response_message += F("background-color: rgba(0, 0, 0, 0.06);");
  response_message += F("z-index: 2;");
  response_message += F("cursor: pointer;");
  response_message += F("}\n");

  response_message += F("</style>");
  response_message += F("</head>");
  response_message += F("<body>");
  response_message += F("<div id=\"overlay\"></div>");

  response_message += F("<div class=\"tile-container\">");
  response_message += F("<h1>固件更新</h1>");
  response_message += F("</div>");

  response_message += F("<div class=\"container\">");

  response_message += F("<form enctype=\"multipart/form-data\" class=\"form-container\" id=\"upload_form\" method=\"post\">");
  response_message += F("<div id=\"switch-container\">");
  response_message += F("<button class=\"button-o left selected\" id=\"firmware-button\" type=\"button\">固件</button>");
  response_message += F("<button class=\"button-o right\" id=\"filesystem-button\" type=\"button\">文件</button>");
  response_message += F("</div>");

  response_message += F("<input accept='.bin,.bin.gz' class=\"input-file\" id=\"file1\" name='firmware' type=\"file\"><br>");
  response_message += F("<progress id=\"progressBar\" max=\"100\" value=\"0\"></progress>");
  response_message += F("<div class=\"additional-info\">");
  response_message += F("<h3 id=\"status\">选择固件</h3>");
  response_message += F("<p id=\"loaded_n_total\"></p>");
  response_message += F("</div>");
  response_message += F("<hr/>");
  response_message += F("<button id=\"button-send\" class=\"button-send\" type=\"submit\" disabled>更新</button>");
  response_message += F("</form>\n");

  response_message += F("<script type=\"application/javascript\">\n");
  response_message += F("function stringToBoolean(string){\n");
  response_message += F("switch(string.toLowerCase().trim()){\n");
  response_message += F("case \"true\": case \"yes\": case \"1\": return true;\n");
  response_message += F("case \"false\": case \"no\": case \"0\": case null: return false;\n");
  response_message += F("default: return Boolean(string);\n");
  response_message += F("}\n");
  response_message += F("}\n");

  response_message += F("const urlParams = new URLSearchParams(window.location.search);\n");
  response_message += F("const onlyFirmware = urlParams.get('onlyFirmware');\n");

  response_message += F("if (onlyFirmware && stringToBoolean(onlyFirmware)===true){\n");
  response_message += F("_('switch-container').style.display = 'none';\n");
  response_message += F("}\n");

  response_message += F("function disableAll() {\n");
  response_message += F("document.getElementById(\"overlay\").style.display = \"block\";\n");
  response_message += F("}\n");

  response_message += F("function enableAll() {\n");
  response_message += F("document.getElementById(\"overlay\").style.display = \"none\";\n");
  response_message += F("}\n");

  response_message += F("function _(el) {\n");
  response_message += F("return document.getElementById(el);\n");
  response_message += F("}\n");

  response_message += F("function uploadFile() {\n");
  response_message += F("var file = _(\"file1\").files[0];\n");
  response_message += F("var formdata = new FormData();\n");
  response_message += F("formdata.append(_(\"file1\").name, file, file.name);\n");
  response_message += F("var ajax = new XMLHttpRequest();\n");
  response_message += F("ajax.upload.addEventListener(\"progress\", progressHandler, false);\n");
  response_message += F("ajax.addEventListener(\"load\", completeHandler, false);\n");
  response_message += F("ajax.addEventListener(\"loadstart\", startHandler, false);\n");
  response_message += F("ajax.addEventListener(\"error\", errorHandler, false);\n");
  response_message += F("ajax.addEventListener(\"abort\", abortHandler, false);\n");
  response_message += F("ajax.open(\"POST\", \"/update\");\n");
  response_message += F("ajax.setRequestHeader('Access-Control-Allow-Headers', '*');\n");
  response_message += F("ajax.setRequestHeader('Access-Control-Allow-Origin', '*');\n");

  response_message += F("ajax.send(formdata);\n");
  response_message += F("}\n");

  response_message += F("function progressHandler(event) {\n");
  response_message += F("_(\"loaded_n_total\").innerHTML = \"上传 \" + event.loaded + \" bytes，总共 \" + event.total + \" bytes\";\n");
  response_message += F("var percent = Math.round((event.loaded / event.total) * 100);\n");
  response_message += F("_(\"progressBar\").value = percent;\n");
  response_message += F("_(\"status\").innerHTML = percent + \"% 上传中... 请稍等\";\n");
  response_message += F("}\n");

  response_message += F("function completeHandler(event) {\n");
  response_message += F("enableAll();\n");
  response_message += F("_(\"status\").innerHTML = '更新成功！';\n");
  response_message += F("_(\"progressBar\").value = 0;\n");
  response_message += F("}\n");

  response_message += F("function startHandler(event) {\n");
  response_message += F("disableAll();\n");
  response_message += F("}\n");

  response_message += F("function errorHandler(event) {\n");
  response_message += F("enableAll();\n");
  response_message += F("_(\"status\").innerHTML = \"更新失败\";\n");
  response_message += F("}\n");

  response_message += F("function abortHandler(event) {\n");
  response_message += F("enableAll();\n");
  response_message += F("_(\"status\").innerHTML = \"更新中止\";\n");
  response_message += F("}\n");


  response_message += F("_('upload_form').addEventListener('submit', (e) => {\n");
  response_message += F("e.preventDefault();\n");
  response_message += F("uploadFile();\n");
  response_message += F("});\n");

  response_message += F("_('firmware-button').addEventListener('click',\n");
  response_message += F("function (e) {\n");
  response_message += F("e.target.classList.add('selected');\n");
  response_message += F("_('filesystem-button').classList.remove('selected');\n");
  response_message += F("_(\"file1\").name = 'firmware';\n");
  response_message += F("}\n");
  response_message += F(")\n");
  response_message += F("_('filesystem-button').addEventListener('click',\n");
  response_message += F("function (e) {\n");
  response_message += F("e.target.classList.add('selected');\n");
  response_message += F("_('firmware-button').classList.remove('selected');\n");
  response_message += F("_(\"file1\").name = 'filesystem';\n");
  response_message += F("}\n");
  response_message += F(")\n");
  response_message += F("_('file1').addEventListener('change', function(e){\n");
  response_message += F("var file = _(\"file1\").files[0];\n");
  response_message += F("if (file && file.name){\n");
  response_message += F("_('button-send').disabled = false;\n");
  response_message += F("_(\"progressBar\").value = 0;\n");
  response_message += F("}else{\n");
  response_message += F("_('button-send').disabled = true;\n");
  response_message += F("}\n");
  response_message += F("_('status').innerHTML = \"固件已加载!\";\n");
  response_message += F("_(\"loaded_n_total\").innerHTML = \"\";\n");
  response_message += F("});\n");
  response_message += F("</script>\n");
  response_message += F("</div>\n");
  response_message += F("</body>\n");
  response_message += F("</html>\n");

  httpServer.sendHeader("Connection", "close");
  httpServer.send(200, "text/html", String(response_message));

  Serial.print(F("| FreeHeap 3: "));
  Serial.println(ESP.getFreeHeap(),DEC);
}
// setting POST handler
void SaveHandler() {
  bool doRestart = false;

  httpServer.sendHeader("Location", String("/"), true);
  httpServer.send(302, "text/plain", "Redirected to: /");

  if (httpServer.hasArg("ssid"))
  {
    if (httpServer.arg("ssid").length() <= SsidMaxLength)
    {
      httpServer.arg("ssid").toCharArray(setting.ssid, SsidMaxLength);
      doRestart = true;
    }
  }

  if (httpServer.hasArg("ssidpass"))
  {
    if (httpServer.arg("ssidpass").length() <= PassMaxLength)
    {
      httpServer.arg("ssidpass").toCharArray(setting.pass, PassMaxLength);
      doRestart = true;
    }
  }

  if (httpServer.hasArg("hostname"))
  {
    if (httpServer.arg("hostname").length() <= HostNameMaxLength)
    {
      httpServer.arg("hostname").toCharArray(setting.hostName, HostNameMaxLength);
      doRestart = true;
    }
  }

  if (httpServer.hasArg("inputnumber"))
  {
    if (httpServer.arg("inputnumber").toInt() > 0 and httpServer.arg("inputnumber").toInt() <= TallyNumberMaxValue)
    {
      setting.tallyNumber = httpServer.arg("inputnumber").toInt();
      doRestart = true;
    }
  }

  if (httpServer.hasArg("intensFull"))
  {
    if (httpServer.arg("intensFull").toInt() >= 0 and httpServer.arg("intensFull").toInt() <= 100)
    {
      setting.intensFull = httpServer.arg("intensFull").toInt();
      doRestart = true;
    }
  }

  if (httpServer.hasArg("intensDim"))
  {
    if (httpServer.arg("intensDim").toInt() >= 0 and httpServer.arg("intensDim").toInt() <= 100)
    {
      setting.intensDim = httpServer.arg("intensDim").toInt();
      doRestart = true;
    }
  }

  if (httpServer.hasArg("prgred"))
  {
    if (httpServer.arg("prgred").toInt() >= 0 and httpServer.arg("prgred").toInt() < 255)
    {
      setting.prgred = httpServer.arg("prgred").toInt();
      doRestart = true;
    }
  }
  if (httpServer.hasArg("prggreen"))
  {
    if (httpServer.arg("prggreen").toInt() >= 0 and httpServer.arg("prggreen").toInt() < 255)
    {
      setting.prggreen = httpServer.arg("prggreen").toInt();
      doRestart = true;
    }
  }
  if (httpServer.hasArg("prgblue"))
  {
    if (httpServer.arg("prgblue").toInt() >= 0 and httpServer.arg("prgblue").toInt() < 255)
    {
      setting.prgblue = httpServer.arg("prgblue").toInt();
      doRestart = true;
    }
  }

  if (httpServer.hasArg("prvred"))
  {
    if (httpServer.arg("prvred").toInt() >= 0 and httpServer.arg("prvred").toInt() < 255)
    {
      setting.prvred = httpServer.arg("prvred").toInt();
      doRestart = true;
    }
  }
  if (httpServer.hasArg("prvgreen"))
  {
    if (httpServer.arg("prvgreen").toInt() >= 0 and httpServer.arg("prvgreen").toInt() < 255)
    {
      setting.prvgreen = httpServer.arg("prvgreen").toInt();
      doRestart = true;
    }
  }
  if (httpServer.hasArg("prvblue"))
  {
    if (httpServer.arg("prvblue").toInt() >= 0 and httpServer.arg("prvblue").toInt() < 255)
    {
      setting.prvblue = httpServer.arg("prvblue").toInt();
      doRestart = true;
    }
  }

  if (httpServer.hasArg("infored"))
  {
    if (httpServer.arg("infored").toInt() >= 0 and httpServer.arg("infored").toInt() < 255)
    {
      setting.offred = httpServer.arg("infored").toInt();
      doRestart = true;
    }
  }
  if (httpServer.hasArg("infogreen"))
  {
    if (httpServer.arg("infogreen").toInt() >= 0 and httpServer.arg("infogreen").toInt() < 255)
    {
      setting.offgreen = httpServer.arg("infogreen").toInt();
      doRestart = true;
    }
  }
  if (httpServer.hasArg("infoblue"))
  {
    if (httpServer.arg("infoblue").toInt() >= 0 and httpServer.arg("infoblue").toInt() < 255)
    {
      setting.offblue = httpServer.arg("infoblue").toInt();
      doRestart = true;
    }
  }

  if (httpServer.hasArg("offred"))
  {
    if (httpServer.arg("offred").toInt() >= 0 and httpServer.arg("offred").toInt() < 255)
    {
      setting.offred = httpServer.arg("offred").toInt();
      doRestart = true;
    }
  }
  if (httpServer.hasArg("offgreen"))
  {
    if (httpServer.arg("offgreen").toInt() >= 0 and httpServer.arg("offgreen").toInt() < 255)
    {
      setting.offgreen = httpServer.arg("offgreen").toInt();
      doRestart = true;
    }
  }
  if (httpServer.hasArg("offblue"))
  {
    if (httpServer.arg("offblue").toInt() >= 0 and httpServer.arg("offblue").toInt() < 255)
    {
      setting.offblue = httpServer.arg("offblue").toInt();
      doRestart = true;
    }
  }

  Serial.printf("Save Data");

  SavaConfig(setting);

  if (doRestart == true)
  {
    Restart();
  }
}
