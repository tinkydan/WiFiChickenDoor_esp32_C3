
void bindServerCallback(){
  wm.server->on("/param",handleRoute);
  wm.server->on("/action_page", handleForm);
  wm.server->on("/debug", handleDebug);
  wm.server->on("/info",handleInfo); // you can override wm!
  wm.server->on("/open",handleOpen);
  wm.server->on("/close",handleClose);
  wm.server->on("/TSU",HUploadTS);
  wm.server->on("/NTP",HUpdateNPTTime);
  wm.server->on("/BAD",BadNPTTime);

  
}
void handleForm() {

  if (currentMillis - EEPROM_Upload_LAST >= EEPROM_Upload_ms )
  { EEPROM_Upload_LAST = currentMillis;

    //| LATT | LONG | TIMZ | ARIS | ASET | UpLm | DnLm
    String LATT_S = wm.server->arg("LATT");
    String LONG_S = wm.server->arg("LONG");
    String TIMZ_S = wm.server->arg("TIMZ");
    Close_Time_String = wm.server->arg("CloseTime");
    Open_Time_String = wm.server->arg("OpenTime");
    String UpLm_S = wm.server->arg("UpLm");
    String DnLm_S = wm.server->arg("DnLm");
//    String T2FD_S = wm.server->arg("T2FD");
//    String FDTM_S = wm.server->arg("FDTM");
    String ApiK_S = wm.server->arg("ApiK");

    String StaF_S = wm.server->arg("StaF");
    String TmpF_S = wm.server->arg("TmpF");
    String BatF_S = wm.server->arg("BatF");
    String In_F_S = wm.server->arg("In_F");
    String DSEv_S = wm.server->arg("DSEv");




    
Serial.print("Open Time: ");
Serial.println(Open_Time_String);
Serial.print("Close Time: ");
Serial.println(Close_Time_String);

  int colon_pos=(Close_Time_String.indexOf(":"));
  if (colon_pos>0){    
    After_Sunset_hr=Close_Time_String.substring(0, colon_pos).toInt();
    After_Sunset=Close_Time_String.substring(colon_pos+1).toInt();
  }
  else{
    After_Sunset=Close_Time_String.toInt();
    After_Sunset_hr=0;
  }
    colon_pos=(Open_Time_String.indexOf(":"));
  if (colon_pos>0){    
    After_Sunrise_hr=Open_Time_String.substring(0, colon_pos).toInt();
    After_Sunrise=Open_Time_String.substring(colon_pos+1).toInt();
  }
  else{
    After_Sunrise=Open_Time_String.toInt();
    After_Sunrise_hr=0;
  }



    
    DOW[0]=(wm.server->arg("Monday").toInt());
    DOW[1]=(wm.server->arg("Tuesday").toInt());
    DOW[2]=(wm.server->arg("Wednesday").toInt());
    DOW[3]=(wm.server->arg("Thursday").toInt());
    DOW[4]=(wm.server->arg("Friday").toInt());
    DOW[5]=(wm.server->arg("Saturday").toInt());
    DOW[6]=(wm.server->arg("Sunday").toInt());

    control_method=(wm.server->arg("control_method").toInt());
    lowtemplockout=(wm.server->arg("LowTemp").toInt());
    lowtemplockout_val=(wm.server->arg("TempL").toFloat());
  
    LATT = LATT_S.toFloat();
    LONG = LONG_S.toFloat();
    Time_ZONE = TIMZ_S.toInt();
  //  After_Sunrise = ARIS_S.toInt();
  //  After_Sunset = ASET_S.toInt();
    up_limit = UpLm_S.toInt();
    down_limit = DnLm_S.toInt();
 //   Feed_time= T2FD_S.toInt();
 //   Feed_length = FDTM_S.toInt();

    strcpy(APIkey,ApiK_S.c_str());
    StateField=StaF_S.toInt();
    TempField=TmpF_S.toInt();
    BatField=BatF_S.toInt();
    InField=In_F_S.toInt();
    DSE=DSEv_S.toInt();


 
EEPROM.begin(EEPROM_SIZE);
    EEPROM.put( 0, LATT);
    EEPROM.put( 4, LONG);
    EEPROM.put( 8, Time_ZONE);
    EEPROM.put( 16, control_method);
    EEPROM.put( 17, lowtemplockout);
    EEPROM.put( 20, up_limit);
    EEPROM.put( 24, down_limit);
    EEPROM.put( 28, DOW);
    EEPROM.put( 36, lowtemplockout_val);
    EEPROM.put( 40, After_Sunrise);
    EEPROM.put( 44, After_Sunset);
    EEPROM.put( 48, After_Sunrise_hr);
    EEPROM.put( 52, After_Sunset_hr);
    EEPROM.put( 56, APIkey);
    EEPROM.put( 74, StateField);
    EEPROM.put( 75 , TempField);
    EEPROM.put( 76 , BatField);
    EEPROM.put( 77 , InField);
    EEPROM.put( 78 , DSE);
    EEPROM.commit();
    EEPROM.end();


    Serial.print("SETTING EEPROM || LATT: ");
    Serial.println(LATT);
    Serial.print("SETTING EEPROM || LONG: ");
    Serial.println(LONG);
    Serial.print("SETTING EEPROM || Time Zone: ");
    Serial.println(Time_ZONE);
    Serial.print("SETTING EEPROM || After Sunrise: ");
    Serial.println(After_Sunrise);
    Serial.print("SETTING EEPROM || After Sunset: ");
    Serial.println(After_Sunset);
    Serial.print("SETTING EEPROM || After Sunrise hr: ");
    Serial.println(After_Sunrise_hr);
    Serial.print("SETTING EEPROM || After Sunset hr: ");
    Serial.println(After_Sunset_hr);
    Serial.print("SETTING EEPROM || up limit: ");
    Serial.println(up_limit);
    Serial.print("SETTING EEPROM || down limit: ");
    Serial.println(down_limit);
    Serial.print("SETTING EEPROM || feed time: ");
    Serial.println(Feed_time);
    Serial.print("SETTING EEPROM || feed length: ");
    Serial.println(Feed_length);
    Serial.print("SETTING EEPROM ||API KEY: ");
    Serial.println(APIkey);
    Serial.print("SETTING EEPROM || State Field: ");
    Serial.println(StateField); 
    Serial.print("SETTING EEPROM || Temperature Field: ");
    Serial.println(TempField);
    Serial.print("SETTING EEPROM || Battery Voltage Field: ");
    Serial.println(BatField);
    Serial.print("SETTING EEPROM || Input Voltage Field: ");
    Serial.println(InField);
    Serial.print("SETTING EEPROM || Deep Sleep Settings: ");
    Serial.println(DSE);

    HTML2 ="";
    HTML2 += "<html><h3>Saved</h3>";
     HTML2 += "<a href=\"/\"><button class=\"button\">Return Home</button></a></html>";
  
 wm.server->send(200, "text/html", HTML2);
// timeClient.setTimeOffset(3600*Time_ZONE);
  }
}
void handleRoute(){
  Serial.println("[HTTP] handle route");
HTML2 ="";
HTML2 +="<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><style>* {  box-sizing: border-box;}.column {  float: left;  width: 33%;  padding: 10px;  height: 430px;"; 
HTML2 +=".row:after {  content: '';  display: table;  clear: both}";
HTML2 +="</style></head><body><h2>Automatic Door Settings</h2>";
  HTML2 += "<form action=\"/action_page\">";
  HTML2 += "<div class='row'> ";
HTML2 +="<div class='column' style='background-color:#aaa;'><h2>General Settings</h2>";



  HTML2 += "Lattitude:<br>";
  HTML2 += "<input type=\"text\" name=\"LATT\" value=";
  HTML2 += String(LATT);
  HTML2 += "><br>";
  HTML2 += "Longitude:<br>";
  HTML2 += "<input type=\"text\" name=\"LONG\" value=";
  HTML2 += String(LONG);
  HTML2 += "><br>";
  HTML2 += "Time Zome (number):<br>";
  HTML2 += "<input type=\"text\" name=\"TIMZ\" value=";
  HTML2 += String(Time_ZONE);
  HTML2 += "><br>";
  HTML2 += "Force threshold on Opening:<br>";
  HTML2 += "<input type=\"text\" name=\"UpLm\" value=";
  HTML2 += String(up_limit);
  HTML2 += "><br>";
  HTML2 += "Force threshold on Close:<br>";
  HTML2 += "<input type=\"text\" name=\"DnLm\" value=";
  HTML2 += String(down_limit);
  HTML2 += "><br>";
  HTML2 += "Thinkspeak API key:<br>";
  HTML2 += "<input type=\"text\" name=\"ApiK\" value=";
  HTML2 += String(APIkey);
  HTML2 += "><br>";
  HTML2 += "Channel Field:<br>";
  HTML2 += "<input type=\"text\" name=\"StaF\" value=";
  HTML2 += String(StateField);
    HTML2 += ">";
  HTML2 += "<input type=\"text\" name=\"TmpF\" value=";
  HTML2 += String(TempField);
    HTML2 += ">";
  HTML2 += "<input type=\"text\" name=\"BatF\" value=";
  HTML2 += String(BatField);
    HTML2 += ">";
  HTML2 += "<input type=\"text\" name=\"In_F\" value=";
  HTML2 += String(InField);
  HTML2 += "><br>";
  HTML2 += "Deep Sleep Setting:<br>";
  HTML2 += "<input type=\"text\" name=\"DSEv\" value=";
  HTML2 += String(DSE);
  HTML2 += "><br>";

HTML2 +="</div><div class='column' style='background-color:#bbb;'><h2>Control Settings</h2>";
HTML2 +="below enter the time for open and close of the door. For Sunrise/Sunset enter the number of minutes after civil twilight for the door to open or close. Negative minutes should be used for times before civil twilight<br> Note: civil twilight is used for better alignment with the observed darkness.This is especally critical for higher lattitude locations<br>";
HTML2 +="for scheduled times enter the time for open/close in 24 hour format hh:mm [for example 21:54]<br>";
HTML2 +="Open:&nbsp;&nbsp;<input name='OpenTime' label='Open' id='OpenTime' class='required' type='text' value='";
HTML2 +=Open_Time_String; 
HTML2 +="' /><br>Close:&nbsp; <input name='CloseTime' label='Close' id='CloseTime' class='required' type='text' value='";
HTML2 +=Close_Time_String;
HTML2 +="' /><br><br>Control Method<br><input type='radio' name='control_method' value='0' ";
if (control_method==0) HTML2 +="checked"; //if sunrise
HTML2 +="> Sunrise/Sunset (best method)<br><input type='radio' name='control_method' value='1'";
if (control_method==1) HTML2 +="checked";  //if Scheduled
HTML2 +="> Scheduled Times<br><input type='radio' name='control_method' value='2'";
if (control_method==2) HTML2 +="checked";  //if Brightness
HTML2 +="> Brightness<br><input type='radio' name='control_method' value='3'";
if (control_method==3) HTML2 +="checked";  //if Manual
HTML2 +="> Manual Only<br><input type='checkbox' id='LowTemp' name='LowTemp' value='1'";
if (lowtemplockout) HTML2 +="checked";  //if lowtemp
HTML2 +="><label for='LowTemp'>Low Temperture Lockout</label><br><input name='TempL' id='TempL' class='required' type='text' value='";
HTML2 +=String(lowtemplockout_val);//lowtemperture lockout value
HTML2 +="' />&nbsp;F</div>";
HTML2 +="<div class='column' style='background-color:#cccc;'><h2>Door Scheduling</h2>";
HTML2 +="<input type='checkbox' id='Monday' name='Monday' value='1'  ";
if (DOW[0]) HTML2 +="checked";
HTML2 +="><label for='Monday'> Monday   </label><br>";
HTML2 +="<input type='checkbox' id='Tuesday' name='Tuesday' value='1'  ";
if (DOW[1]) HTML2 +="checked";
HTML2 +="><label for='Tuesday'> Tuesday  </label><br>";
HTML2 +="<input type='checkbox' id='Wednesday' name='Wednesday' value='1'  ";
if (DOW[2]) HTML2 +="checked";
HTML2 +="><label for='Wednesday'> Wednesday</label><br>";
HTML2 +="<input type='checkbox' id='Thursday' name='Thursday' value='1' "; 
if (DOW[3]) HTML2 +="checked";
HTML2 +="><label for='Thursday'> Thursday </label><br>";
HTML2 +="<input type='checkbox' id='Friday' name='Friday' value='1'  ";
if (DOW[4]) HTML2 +="checked";
HTML2 +="><label for='Friday'> Friday   </label><br>";
HTML2 +="<input type='checkbox' id='Saturday' name='Saturday' value='1' "; 
if (DOW[5]) HTML2 +="checked";
HTML2 +="><label for='Saturday'> Saturday </label><br>";
HTML2 +="<input type='checkbox' id='Sunday' name='Sunday' value='1'  ";
if (DOW[6]) HTML2 +="checked";
HTML2 +="><label for='Sunday'> Sunday   </label><br> </div></div>";
  HTML2 += "<br><br>";
  HTML2 += "<input type=\"submit\" value=\"Submit\">";
  HTML2 += "</form> </body></html>";
 wm.server->send(200, "text/html", HTML2);
}

void SendHeaders(){
    HTML2 +="<!DOCTYPE HTML><html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><meta name=\"format-detection\" content=\"telephone=no\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"><style>.c,body{text-align:center;font-family:verdana}div,input{padding:5px;font-size:1em;margin:5px 0;box-sizing:border-box;}input,button,.msg{border-radius:.3rem;width: 100%},input[type=radio]{width: auto}button,input[type='button'],input[type='submit']{cursor:pointer;border:0;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%}input[type='file']{border:1px solid #1fa3ec}.wrap {text-align:left;display:inline-block;min-width:260px;max-width:500px}a{color:#000;font-weight:700;text-decoration:none}a:hover{color:#1fa3ec;text-decoration:underline}.q{height:16px;margin:0;padding:0 5px;text-align:right;min-width:38px;float:right}.q.q-0:after{background-position-x:0}.q.q-1:after{background-position-x:-16px}.q.q-2:after{background-position-x:-32px}.q.q-3:after{background-position-x:-48px}.q.q-4:after{background-position-x:-64px}.q.l:before{background-position-x:-80px;padding-right:5px}.ql .q{float:left}.q:after,.q:before{content:'';width:16px;height:16px;display:inline-block;background-repeat:no-repeat;background-position: 16px 0;background-image:url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGAAAAAQCAMAAADeZIrLAAAAJFBMVEX///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADHJj5lAAAAC3RSTlMAIjN3iJmqu8zd7vF8pzcAAABsSURBVHja7Y1BCsAwCASNSVo3/v+/BUEiXnIoXkoX5jAQMxTHzK9cVSnvDxwD8bFx8PhZ9q8FmghXBhqA1faxk92PsxvRc2CCCFdhQCbRkLoAQ3q/wWUBqG35ZxtVzW4Ed6LngPyBU2CobdIDQ5oPWI5nCUwAAAAASUVORK5CYII=');}@media (-webkit-min-device-pixel-ratio: 2),(min-resolution: 192dpi){.q:before,.q:after {background-image:url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAALwAAAAgCAMAAACfM+KhAAAALVBMVEX///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAOrOgAAAADnRSTlMAESIzRGZ3iJmqu8zd7gKjCLQAAACmSURBVHgB7dDBCoMwEEXRmKlVY3L//3NLhyzqIqSUggy8uxnhCR5Mo8xLt+14aZ7wwgsvvPA/ofv9+44334UXXngvb6XsFhO/VoC2RsSv9J7x8BnYLW+AjT56ud/uePMdb7IP8Bsc/e7h8Cfk912ghsNXWPpDC4hvN+D1560A1QPORyh84VKLjjdvfPFm++i9EWq0348XXnjhhT+4dIbCW+WjZim9AKk4UZMnnCEuAAAAAElFTkSuQmCC');background-size: 95px 16px;}}.msg{padding:20px;margin:20px 0;border:1px solid #eee;border-left-width:5px;border-left-color:#777}.msg h4{margin-top:0;margin-bottom:5px}.msg.P{border-left-color:#1fa3ec}.msg.P h4{color:#1fa3ec}.msg.D{border-left-color:#dc3630}.msg.D h4{color:#dc3630}.msg.S{border-left-color: #5cb85c}.msg.S h4{color: #5cb85c}dt{font-weight:bold}dd{margin:0;padding:0 0 0.5em 0;min-height:12px}td{vertical-align: top;}.h{display:none}button.D{background-color:#dc3630}body.invert,body.invert a,body.invert h1 {background-color:#060606;color:#fff;}body.invert .msg{color:#fff;background-color:#282828;border-top:1px solid #555;border-right:1px solid #555;border-bottom:1px solid #555;}body.invert .q[role=img]{-webkit-filter:invert(1);filter:invert(1);}input:disabled {opacity: 0.5;}</style>";
}

void SendTail(){
HTML2 +="<br><hr><br><br><form action=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/erase\" method=\"get\"><button class=\"D\">Erase WiFi Config</button></form><br><h3>Available Pages</h3><hr><table class=\"table\"><thead><tr><th>Page</th><th>Function</th></tr></thead><tbody><tr><td><a href=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/\">/</a></td><td>Menu page.</td></tr><tr><td><a href=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/wifi\">/wifi</a></td><td>Show WiFi scan results and enter WiFi configuration.(/0wifi noscan)</td></tr><tr><td><a href=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/wifisave\">/wifisave</a></td><td>Save WiFi configuration information and configure device. Needs variables supplied.</td></tr><tr><td><a href=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/param\">/param</a></td><td>Parameter page</td></tr><tr><td><a href=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/info\">/info</a></td><td>Information page</td></tr><tr><td><a href=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/u\">/u</a></td><td>OTA Update</td></tr><tr><td><a href=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/close\">/close</a></td><td>Close the captiveportal popup,configportal will remain active</td></tr><tr><td>/exit</td><td>Exit Config Portal, configportal will close</td></tr><tr><td>/restart</td><td>Reboot the device</td></tr><tr><td>/erase</td><td>Erase WiFi configuration and reboot Device. Device will not reconnect to a network until new WiFi configuration data is entered.</td></tr></tbody></table><p>More information about WiFiManager at <a href=\"https://github.com/tzapu/WiFiManager\">https://github.com/tzapu/WiFiManager</a>.</p></div></body></html>";

}
void handleInfo(){
  Serial.println("Serving Info");

    HTML2 ="";
    SendHeaders();
  if (WiFi.isConnected()) {
HTML2 += "<title>Info</title></head><body class=\"{c}\"><div class=\"wrap\"><div class=\"msg S\"><strong>Connected</strong> to ";
HTML2 += "<br><em><small>with IP ";
HTML2 += WiFi.localIP().toString();
HTML2 +="</small></em></div>";
}
if (1) {
HTML2 +="<h3>esp8266</h3><hr><dl><dt>Uptime</dt><dd>";
HTML2 +=String(floor(millis() / 1000 / 60),0);
HTML2 +="Mins ";
HTML2 +=String(((millis() / 1000 / 60)-floor(millis() / 1000 / 60))*60,0);
HTML2 +=" Secs</dd><dt>Temperture</dt><dd>";
HTML2 +=TempF;
HTML2 +=" F</dd><dt>Chip ID</dt><dd>";
HTML2 +=WIFI_getChipId();
HTML2 +="</dd><dt>Flash Chip ID</dt><dd>";
//HTML2 +=ESP.getFlashChipId();
HTML2 +="</dd><dt>Flash Size</dt><dd>";
//HTML2 +=String(ESP.getFlashChipSize());
HTML2 +=" bytes</dd><dt>Real Flash Size</dt><dd>";
//HTML2 +=String(ESP.getFlashChipRealSize());
HTML2 +=" bytes</dd><dt>SDK Version</dt><dd>";
//HTML2 +=system_get_sdk_version();
//HTML2 +="</dd><dt>Core Version</dt><dd>";
////HTML2 +=ESP.getCoreVersion();
//HTML2 +="</dd><dt>Boot Version</dt><dd>";
//HTML2 +=String(system_get_boot_version());
//HTML2 +="</dd><dt>Wifi Chicken Door Version</dt><dd>";
//HTML2 +=String(FirmwareVer);
//HTML2 +="</dd><dt>CPU Frequency</dt><dd>";
//HTML2 +=ESP.getCpuFreqMHz();
//HTML2 +="</dd><dt>Memory - Free Heap</dt><dd>",
//HTML2 +=String(ESP.getFreeHeap());
//HTML2 +=" bytes available</dd><dt>Memory - Sketch Size</dt><dd>Used / Total bytes<br>";
//HTML2 +=String(ESP.getSketchSize());
//HTML2 +=" / ";
//HTML2 +=String(ESP.getSketchSize()+ESP.getFreeSketchSpace());
//HTML2 +="<br><progress value=\"";
//HTML2 +=String(ESP.getSketchSize());
//HTML2 +="\" max=\"";
//HTML2 +=String(ESP.getSketchSize()+ESP.getFreeSketchSpace());
//HTML2 +="\"></progress></dd>";
//HTML2 +="<br><h3>WiFi</h3><hr><dt>Connected</dt><dd>";
if (WiFi.isConnected()){
HTML2 +="Yes";}
else {
HTML2 +="No";}
//HTML2 +="</dd><dt>Station SSID</dt><dd>";
//HTML2 +=WiFi_SSID();
HTML2 +="</dd><dt>Station IP</dt><dd>";
HTML2 +=WiFi.localIP().toString();
HTML2 +="</dd><dt>Station Gateway</dt><dd>";
HTML2 +=WiFi.gatewayIP().toString();
HTML2 +="</dd><dt>Station Subnet</dt><dd>";
HTML2 +=WiFi.subnetMask().toString();
HTML2 +="</dd><dt>DNS Server</dt><dd>";
HTML2 +=WiFi.dnsIP().toString();
HTML2 +="</dd><dt>Hostname</dt><dd>";
//HTML2 +=WiFi.hostname();
HTML2 +="</dd><dt>Station MAC</dt><dd>";
HTML2 +=WiFi.macAddress();
HTML2 +="</dd><dt>Autoconnect</dt><dd>";
if (WiFi.getAutoConnect()) {
HTML2 +="Enabled";}
else {
HTML2 +="Disabled";}
HTML2 +="</dd><dt>Access Point SSID</dt><dd>";
HTML2 +=WiFi.softAPSSID();
HTML2 +="</dd>";
HTML2 +="</dl><form action=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/update\" method=\"get\"><br><button>Update</button><br></form><form action=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/debug\" method=\"get\"><button>Debug Page</button></form>";
SendTail();

 // wm.server->send(200, "text/html", "<!DOCTYPE html><html lang=\"en\"><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><meta name=\"format-detection\" content=\"telephone=no\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"><style>.c,body{text-align:center;font-family:verdana}div,input{padding:5px;font-size:1em;margin:5px 0;box-sizing:border-box;}input,button,.msg{border-radius:.3rem;width: 100%},input[type=radio]{width: auto}button,input[type='button'],input[type='submit']{cursor:pointer;border:0;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%}input[type='file']{border:1px solid #1fa3ec}.wrap {text-align:left;display:inline-block;min-width:260px;max-width:500px}a{color:#000;font-weight:700;text-decoration:none}a:hover{color:#1fa3ec;text-decoration:underline}.q{height:16px;margin:0;padding:0 5px;text-align:right;min-width:38px;float:right}.q.q-0:after{background-position-x:0}.q.q-1:after{background-position-x:-16px}.q.q-2:after{background-position-x:-32px}.q.q-3:after{background-position-x:-48px}.q.q-4:after{background-position-x:-64px}.q.l:before{background-position-x:-80px;padding-right:5px}.ql .q{float:left}.q:after,.q:before{content:'';width:16px;height:16px;display:inline-block;background-repeat:no-repeat;background-position: 16px 0;background-image:url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGAAAAAQCAMAAADeZIrLAAAAJFBMVEX///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADHJj5lAAAAC3RSTlMAIjN3iJmqu8zd7vF8pzcAAABsSURBVHja7Y1BCsAwCASNSVo3/v+/BUEiXnIoXkoX5jAQMxTHzK9cVSnvDxwD8bFx8PhZ9q8FmghXBhqA1faxk92PsxvRc2CCCFdhQCbRkLoAQ3q/wWUBqG35ZxtVzW4Ed6LngPyBU2CobdIDQ5oPWI5nCUwAAAAASUVORK5CYII=');}@media (-webkit-min-device-pixel-ratio: 2),(min-resolution: 192dpi){.q:before,.q:after {background-image:url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAALwAAAAgCAMAAACfM+KhAAAALVBMVEX///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAOrOgAAAADnRSTlMAESIzRGZ3iJmqu8zd7gKjCLQAAACmSURBVHgB7dDBCoMwEEXRmKlVY3L//3NLhyzqIqSUggy8uxnhCR5Mo8xLt+14aZ7wwgsvvPA/ofv9+44334UXXngvb6XsFhO/VoC2RsSv9J7x8BnYLW+AjT56ud/uePMdb7IP8Bsc/e7h8Cfk912ghsNXWPpDC4hvN+D1560A1QPORyh84VKLjjdvfPFm++i9EWq0348XXnjhhT+4dIbCW+WjZim9AKk4UZMnnCEuAAAAAElFTkSuQmCC');background-size: 95px 16px;}}.msg{padding:20px;margin:20px 0;border:1px solid #eee;border-left-width:5px;border-left-color:#777}.msg h4{margin-top:0;margin-bottom:5px}.msg.P{border-left-color:#1fa3ec}.msg.P h4{color:#1fa3ec}.msg.D{border-left-color:#dc3630}.msg.D h4{color:#dc3630}.msg.S{border-left-color: #5cb85c}.msg.S h4{color: #5cb85c}dt{font-weight:bold}dd{margin:0;padding:0 0 0.5em 0;min-height:12px}td{vertical-align: top;}.h{display:none}button.D{background-color:#dc3630}body.invert,body.invert a,body.invert h1 {background-color:#060606;color:#fff;}body.invert .msg{color:#fff;background-color:#282828;border-top:1px solid #555;border-right:1px solid #555;border-bottom:1px solid #555;}body.invert .q[role=img]{-webkit-filter:invert(1);filter:invert(1);}input:disabled {opacity: 0.5;}</style>");

 wm.server->send(200, "text/html", HTML2);
 HTML2 ="";
}
}


void handleDebug(){
  Serial.println("Serving Info");

    HTML2 ="";
    SendHeaders();
  if (WiFi.isConnected()) {
HTML2 += "<title>Info</title></head><body class=\"{c}\"><div class=\"wrap\"><div class=\"msg S\"><strong>Connected</strong> to ";
HTML2 += "<br><em><small>with IP ";
HTML2 += WiFi.localIP().toString();
HTML2 +="</small></em></div>";
}

HTML2 +="<h3>";
HTML2 +=digitalClockDisplay()  + " " + digitalDateDisplay();
HTML2 +="</h3><hr><dl><dt>Sunrise / Sunset </dt><dd>";
HTML2 +=String(MESunrise);
Serial.print("MESunrise: ");
Serial.println(MESunrise);
HTML2 +="Mins / ";
Serial.print("MESunsete: ");
Serial.println(MESunset);
HTML2 +=String(MESunset);
HTML2 +="Mins</dd><dt>Current Mins</dt><dd>";
HTML2 +=String(CUR_time);
HTML2 +="</dd><dt>After Sunrise</dt><dd>";
HTML2 +=String(After_Sunrise_hr) + ":" + String(After_Sunrise);
HTML2 +="</dd><dt>After Sunset</dt><dd>";
HTML2 +=String(After_Sunset_hr) + ":" + String(After_Sunset);
Serial.print("After Sunset: ");
Serial.println(After_Sunset);
HTML2 +="</dd><dt>Second Drift</dt><dd>";
HTML2 +=String(second_err_T);
HTML2 +="</dd><dt>Top Trigger / Bottom Trigger</dt><dd>";
HTML2 +=String(is_top) + " / " + String(is_bot);
if (!is_top && !is_bot) {//Unplugged
     HTML2 +="<br> Unplugged";
}
  else if (is_top==2 && is_bot==2){
    HTML2 +="<br> Unplugged";
    }
    else if (is_bot==1 && is_top==1) {//In between
     HTML2 +="<br> In between";
    }
    else if (is_top == 1) { //Top Unlocked
      HTML2 +="<br> Top (Opened)";
    }
    else if (is_bot == 1) { //Bottom Locked
      HTML2 +="<br> Bottom (Closed)";
    }

  HTML2 +=" </dd><dt>Open Sensor / Close Sensor</dt><dd>";
  HTML2 +=sensor[is_top] + " / " + sensor[is_bot];
HTML2 +=" </dd><dt>Position Sensor</dt><dd>";
HTML2 +=String(sensorValue);
HTML2 +="</dd><dt>Uprequest / Downrequest / Stoprequest / Jamed / Overlimit</dt><dd>";     
HTML2 +=String(UPREQUEST) + " / " + String(DOWNREQUEST) + " / " + String(STOPREQUEST) + " / " + String(JAMED) + " / " + String(OVER_LIM);
HTML2 +="</dd><dt>Going Up / Going Down</dt><dd>";
HTML2 +=String(going_up) + " / " + String(going_down);//request_direc
HTML2 +="</dd><dt>requested direction (1 close, 2 up)</dt><dd>";
HTML2 +=String(request_direc);   
HTML2 +="</dd><dt>ADC Reading</dt><dd>";
HTML2 +=String(val_01) + " / " + String(val_2) + " / " + String(val_3);  
HTML2 +="</dd><dt>CPU Frequency</dt><dd>";



HTML2 +="</dd><dt>Clock Updated</dt><dd>";
HTML2 +=String(gps_time_updated);

HTML2 +="</dd><dt>Memory - Free Heap</dt><dd>",
HTML2 +=String(ESP.getFreeHeap());
HTML2 +=" bytes available</dd><dt>Memory - Sketch Size</dt><dd>Used / Total bytes<br>";
HTML2 +=String(ESP.getSketchSize());
HTML2 +=" / ";
HTML2 +=String(ESP.getSketchSize()+ESP.getFreeSketchSpace());
HTML2 +="<br><progress value=\"";
HTML2 +=String(ESP.getSketchSize());
HTML2 +="\" max=\"";
HTML2 +=String(ESP.getSketchSize()+ESP.getFreeSketchSpace());
HTML2 +="\"></progress></dd>";
HTML2 +="<br><h3>WiFi</h3><hr><dt>Connected</dt><dd>";
if (WiFi.isConnected()){
HTML2 +="Yes";}
else {
HTML2 +="No";}
//HTML2 +="</dd><dt>Station SSID</dt><dd>";
//HTML2 +=WiFi_SSID();
HTML2 +="</dd><dt>Station IP</dt><dd>";
HTML2 +=WiFi.localIP().toString();
HTML2 +="</dd><dt>Station Gateway</dt><dd>";
HTML2 +=WiFi.gatewayIP().toString();
HTML2 +="</dd><dt>Station Subnet</dt><dd>";
HTML2 +=WiFi.subnetMask().toString();
HTML2 +="</dd><dt>DNS Server</dt><dd>";
HTML2 +=WiFi.dnsIP().toString();
HTML2 +="</dd><dt>Hostname</dt><dd>";
//HTML2 +=WiFi.hostname();
HTML2 +="</dd><dt>Station MAC</dt><dd>";
HTML2 +=WiFi.macAddress();
HTML2 +="</dd><dt>Autoconnect</dt><dd>";
if (WiFi.getAutoConnect()) {
HTML2 +="Enabled";}
else {
HTML2 +="Disabled";}
HTML2 +="</dd><dt>Access Point SSID</dt><dd>";
HTML2 +=WiFi.softAPSSID();
HTML2 +="</dd>";
HTML2 +="</dl><form action=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/update\" method=\"get\"><br><button>Update</button><br><br></form><form action=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/open\" method=\"get\"><button>Open Door</button></form>";
HTML2 +="<br><br></form><form action=\"http://";
HTML2 +=WiFi.localIP().toString();
HTML2 +="/close\" method=\"get\"><button>Close Door</button></form>";
SendTail();

 // wm.server->send(200, "text/html", "<!DOCTYPE html><html lang=\"en\"><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><meta name=\"format-detection\" content=\"telephone=no\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"><style>.c,body{text-align:center;font-family:verdana}div,input{padding:5px;font-size:1em;margin:5px 0;box-sizing:border-box;}input,button,.msg{border-radius:.3rem;width: 100%},input[type=radio]{width: auto}button,input[type='button'],input[type='submit']{cursor:pointer;border:0;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%}input[type='file']{border:1px solid #1fa3ec}.wrap {text-align:left;display:inline-block;min-width:260px;max-width:500px}a{color:#000;font-weight:700;text-decoration:none}a:hover{color:#1fa3ec;text-decoration:underline}.q{height:16px;margin:0;padding:0 5px;text-align:right;min-width:38px;float:right}.q.q-0:after{background-position-x:0}.q.q-1:after{background-position-x:-16px}.q.q-2:after{background-position-x:-32px}.q.q-3:after{background-position-x:-48px}.q.q-4:after{background-position-x:-64px}.q.l:before{background-position-x:-80px;padding-right:5px}.ql .q{float:left}.q:after,.q:before{content:'';width:16px;height:16px;display:inline-block;background-repeat:no-repeat;background-position: 16px 0;background-image:url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGAAAAAQCAMAAADeZIrLAAAAJFBMVEX///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADHJj5lAAAAC3RSTlMAIjN3iJmqu8zd7vF8pzcAAABsSURBVHja7Y1BCsAwCASNSVo3/v+/BUEiXnIoXkoX5jAQMxTHzK9cVSnvDxwD8bFx8PhZ9q8FmghXBhqA1faxk92PsxvRc2CCCFdhQCbRkLoAQ3q/wWUBqG35ZxtVzW4Ed6LngPyBU2CobdIDQ5oPWI5nCUwAAAAASUVORK5CYII=');}@media (-webkit-min-device-pixel-ratio: 2),(min-resolution: 192dpi){.q:before,.q:after {background-image:url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAALwAAAAgCAMAAACfM+KhAAAALVBMVEX///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAOrOgAAAADnRSTlMAESIzRGZ3iJmqu8zd7gKjCLQAAACmSURBVHgB7dDBCoMwEEXRmKlVY3L//3NLhyzqIqSUggy8uxnhCR5Mo8xLt+14aZ7wwgsvvPA/ofv9+44334UXXngvb6XsFhO/VoC2RsSv9J7x8BnYLW+AjT56ud/uePMdb7IP8Bsc/e7h8Cfk912ghsNXWPpDC4hvN+D1560A1QPORyh84VKLjjdvfPFm++i9EWq0348XXnjhhT+4dIbCW+WjZim9AKk4UZMnnCEuAAAAAElFTkSuQmCC');background-size: 95px 16px;}}.msg{padding:20px;margin:20px 0;border:1px solid #eee;border-left-width:5px;border-left-color:#777}.msg h4{margin-top:0;margin-bottom:5px}.msg.P{border-left-color:#1fa3ec}.msg.P h4{color:#1fa3ec}.msg.D{border-left-color:#dc3630}.msg.D h4{color:#dc3630}.msg.S{border-left-color: #5cb85c}.msg.S h4{color: #5cb85c}dt{font-weight:bold}dd{margin:0;padding:0 0 0.5em 0;min-height:12px}td{vertical-align: top;}.h{display:none}button.D{background-color:#dc3630}body.invert,body.invert a,body.invert h1 {background-color:#060606;color:#fff;}body.invert .msg{color:#fff;background-color:#282828;border-top:1px solid #555;border-right:1px solid #555;border-bottom:1px solid #555;}body.invert .q[role=img]{-webkit-filter:invert(1);filter:invert(1);}input:disabled {opacity: 0.5;}</style>");

 wm.server->send(200, "text/html", HTML2);
 HTML2 ="";

}


void handleOpen(){
  UPREQUEST=1;
  wm.server->send(200, "text/html", "Opening");
}
void handleClose(){
  DOWNREQUEST=1;
  wm.server->send(200, "text/html", "Closing");
}


void HUploadTS(){
  UploadTS();
  wm.server->send(200, "text/html", "Updating TS");
}
void HUpdateNPTTime(){
 UpdateNPTTime();
  wm.server->send(200, "text/html", "Updating NPTTime");
}
void BadNPTTime(){
 setTime(10, 4, 2, 2,3, 2029);
 wm.server->send(200, "text/html", "Testing NPTTime");}
