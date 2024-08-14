void setTime_R(int hr,int mins,int sec,int dy, int mnth, int yr){
  setTime(hr,mins,sec,dy,mnth,yr);
  hr_set=hr;min_set=mins;sec_set=sec;day_set=dy;month_set=mnth;year_set=yr;
  DEBUG_PRINT("Clock Updated to: " + String(yr) + " Month: "+ String(mnth) + " Day: " + String(dy) + " Hour: " + String(hr) + " Min: " + String(mins) + " Sec: " + String(sec));
}
 

String digitalClockDisplay() {
  // digital clock display of the time
  String Time_NOW;
  Time_NOW = String(hour());
  Time_NOW += String(":");
  Time_NOW += printDigits(minute());
  Time_NOW += String(":");
  Time_NOW += printDigits(second());
  // Time_NOW += " ";

  //  Time_NOW += String(month());
  return Time_NOW;
}

String digitalDateDisplay() {
  String Time_NOW;
  Time_NOW += printDigits(month());
  Time_NOW += String("/");
  // Time_NOW += String(day());
  Time_NOW += printDigits(day());
  Time_NOW += String("/");

  Time_NOW += String(year()); \
  return Time_NOW;

}
String printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  String Digs = "";
  if (digits < 10) {
    Digs += '0';
  }
  Digs += String(digits);
  return Digs;
}


void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

void UpdateNPTTime() {
  int time_close = 0;
  tcount = 0;
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo) && tcount < 5) {
    tcount += 1;

  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");



  if (tcount < 5 && (TIME_SET_BY != 1)) { // Will not adjust time from NPT if GPS time was set

    ajH =   hour() - Time_ZONE;
    int Da=0;;
    if (ajH < 0) {
      ajH += 24;
      Da = -1;
    }
    if (ajH > 23) {
      ajH += -24;
      Da = 1;
    }

//  tmConvert_t(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min,timeinfo.tm_sec)
DEBUG_PRINT("EPOC for NPT " + String(int(mktime(&timeinfo)))) ;
    int64_t second_err_new=int64_t(tmConvert_t(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min,timeinfo.tm_sec)) - int64_t(tmConvert_t(year(), month(), day()+ Da,  ajH, minute(), second()));
    DEBUG_PRINT("Above") ;
    if (abs(second_err_new) > 300) { // Checks if the clock is off by more than a reasonable amount

      //Checks to see if the last time the time was off but was the same as now
      DEBUG_PRINT("Benchmarking to previous time") ;
      if (abs(int64_t((tmConvert_t(year_lst, month_lst, day_lst, hr_lst, min_lst, sec_lst) + (Time_NPT_ms / 1000))) - int64_t(tmConvert_t(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min,timeinfo.tm_sec))) > 30) {
        NPT_ATTEMPTs = 0 ;
        DEBUG_PRINT("Last time stored not consisten with current NPT");
      }
      else {
        NPT_ATTEMPTs ++ ;
        DEBUG_PRINT("Number of NPT reset attempts " + String(NPT_ATTEMPTs));
      }
      // Save questionable time
      hr_lst = timeinfo.tm_hour; min_lst = timeinfo.tm_min; sec_lst = timeinfo.tm_sec; day_lst = timeinfo.tm_mday; month_lst = timeinfo.tm_mon + 1; year_lst = timeinfo.tm_year + 1900;
    }
    else {
      time_close = 1;
    }

    int NPT_UPDATE_VALID_ATTEMPT = 3;
      if (second_err_new==0) {
        DEBUG_PRINT("Updated not required time already correct");
        time_close=0;// Time is exact so no update required
      }
    if ((NPT_UPDATE_VALID_ATTEMPT < NPT_ATTEMPTs) || time_close || (TIME_SET_BY == 0)) {
      NPT_ATTEMPTs = 0;

      DEBUG_PRINT("Updated time from NTP");

      if (time_close){
        DEBUG_PRINT("Time adjusted sligtly by " + String(second_err_new) + " seconds");
        second_err_T=second_err_T+second_err_new;
      }
      else if (TIME_SET_BY == 0) {
        DEBUG_PRINT("Time Previously Unset");
      }
      else {
        DEBUG_PRINT("Time not correct for multiple update resinking");
      }
      //        DEBUG_PRINT(timeClient.getFormattedTime().c_str());
      TIME_SET_BY = 2; // Indicates the time was set by the NPT protocol
      // setTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds(), timeClient.getDay(), timeClient.getMonth(), timeClient.getYear());
      DEBUG_PRINT("Hour Set: " + String(timeinfo.tm_hour) + " Min Set: " + String(timeinfo.tm_min) + " Second Set: " + String(timeinfo.tm_sec));
      DEBUG_PRINT("Day Set: " + String(timeinfo.tm_mday) + " Month Set: " + String(timeinfo.tm_mon + 1) + " Year Set: " + String(timeinfo.tm_year - 100 + 2000));
      ajH = Time_ZONE + timeinfo.tm_hour;
      int Da=0;
      if (ajH < 0) {
        ajH += 24;
        Da = -1;
      }
      if (ajH > 23) {
        ajH += -24;
        Da = 1;
      }
      setTime_R(ajH, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday + Da, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
      DEBUG_PRINT("Setting time by the time zone...");
      DEBUG_PRINT(digitalClockDisplay().c_str());
      DEBUG_PRINT(digitalDateDisplay().c_str());

    }
  }

}

time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss) {
  DEBUG_PRINT("EPOC for Year: " + String(YYYY) + " Month: "+ String(MM) + " Day: " + String(DD) + " Hour: " + String(hh) + " Min: " + String(mm) + " Sec: " + String(ss));
  tmElements_t tm;
  tm.Year = YYYY - 1970;
  tm.Month = MM - 1;
  tm.Day = DD;
  tm.Hour = hh;
  tm.Minute = mm;
  tm.Second = ss;
  DEBUG_PRINT(makeTime(tm));
  return makeTime(tm);
}

/*
  int DEBUGSerial=0;
  void initalizeNTP() {

  if (DEBUGSerial >= 2) {
    Serial.print("IP number assigned by DHCP is ");
    Serial.println(WiFi.localIP());
    Serial.println("Starting UDP");
    Serial.print("Local port: ");
    Serial.println(Udp.localPort());
    Serial.println("waiting for sync");
  }

  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(3600);
  }


  void digitalClockDisplay()
  {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
  }

  void printDigits(int digits)
  {
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
  }

  /*-------- NTP code ----------*/
/*
  const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

  time_t getNtpTime()
  {
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  if (DEBUGSerial >= 2) {
    Serial.println("Transmit NTP Request");
  }
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  if (DEBUGSerial >= 2) {
    Serial.print(ntpServerName);
    Serial.print(": ");
    Serial.println(ntpServerIP);
  }

  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      if (DEBUGSerial >= 2) {
        Serial.println("Receive NTP Response");
      }
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  if (DEBUGSerial >= 2) {
    Serial.println("No NTP Response :-(");
  }
  return 0; // return 0 if unable to get the time
  }

  // send an NTP request to the time server at the given address
  void sendNTPpacket(IPAddress &address)
  {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
  }
*/
