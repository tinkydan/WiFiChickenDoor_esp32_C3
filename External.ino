


void UploadTS() {
  if (APIkey != "________________") {
    Link = "GET /update?api_key=" + String(APIkey);
    if (StateField != 0) {
      Link += "&field" + String(StateField) + "=" + Get_State(2);
    }
    if (TempField != 0&&  TempF) {
      Link += "&field" + String(TempField) + "=" + String(TempF);
    }
    if (TempField != 0) {
      Link += "&field" + String(BatField) + "=" + String(BAT_LEV);
    }
    if (TempField != 0) {
      Link += "&field" + String(InField) + "=" + String(In_Level);
    }
    Link = Link + " HTTP/1.1\r\n" + "Host: " + String(host) + "\r\n" + "Connection: close\r\n\r\n";
    Serial.print(Link);
    if (client.connect(serverTS, 80)) {
      client.print(Link);
      Serial.print(Link);
    }
  }
}


String Get_State(int format) {

  String Cstate, ints;
  if (JAMED > 0) {
    ints = String(3 + JAMED);
    if (format == 0) {
      Cstate = "Check for jamed door\r\n";
      Cstate += "Door exceded limits " +  DIRC_S[JAMED];
    }
    else if (format == 1) {
      Cstate = "Jamed " +  DIRC_S[JAMED] ;
    }
  }
  else if (( is_top  == HIGH) && ( is_bot  == HIGH)) {
    ints = String(3);
    if (format == 0) {
      Cstate = "Sensor Unpluged";
    }
    else if (format == 1) {

      Cstate = "SENSOR NOT    CONNECTED";
    }
  }
  else if ( is_top == HIGH) {
    ints = String(2);
    Cstate = "UP (Open)";
  }
  else if (is_bot == HIGH) {
    Cstate = "DOWN (Closed)";
    ints = String(1);
  }
  else {
    ints = String(0);
    if (format == 0) {
      Cstate = "UNKNOWN position";
    }
    else if (format == 1) {
      Cstate = "BETWEEN       SENSORS";
    }
  }
  if (format == 2) {
    return ints;
  }
  else {
    return Cstate;
  }
}
