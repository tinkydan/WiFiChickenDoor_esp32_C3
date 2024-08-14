void Update_Screen(bool finish) {
  bool Con=1;
 // Serial.print(LCD_Sleep_Start + LCD_Sleep_ms);
 // Serial.print(" Sensor:");
 //Serial.println();
  if (LCD_Sleep_Start + LCD_Sleep_ms > millis()) {
    // put your main code here, to run repeatedly:
    int Ctype_POS = 90;
    u8g2.firstPage();
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    u8g2.setDrawColor(0);
    //u8g2.drawBox(Ctype_POS, 20, 12, 8);
    u8g2.drawBox(0, 0, 128, 64);
    u8g2.setDrawColor(1);
    if (Time_Method == 0) {
      u8g2.drawGlyph(Ctype_POS, 8, 165); // Brightness
    }
    else if (Time_Method == 1) {
      u8g2.drawGlyph(Ctype_POS, 8, 121); // No Schedule
    }
    else if (Time_Method == 2) {
      u8g2.drawGlyph(Ctype_POS, 8, 269); // Clock Schedule
    }
    else if (Time_Method == 3) {
      u8g2.drawGlyph(Ctype_POS, 8, 175); // SunSet
    }
    // Open Logic


    // Door Status
    int Status_POS = 77;
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    u8g2.setDrawColor(0);
    u8g2.drawBox(Status_POS, 1, 12, 8);
    u8g2.setDrawColor(1);
    if (is_top && is_bot) {//Unplugged
      u8g2.drawGlyph(Status_POS, 8, 282);
    }
    else if (!is_bot && !is_top) {//In between
      u8g2.drawGlyph(Status_POS, 8, 240);
    }
    else if (is_top == 1) { //Top Unlocked
      u8g2.drawGlyph(Status_POS, 8, 203);
    }
    else if (is_bot == 1) { //Bottom Locked
      u8g2.drawGlyph(Status_POS, 8, 202);
    }


    // put your setup code here, to run once:
    int BAT_P = 105;
    int BAT_LEV_C;
    u8g2.drawFrame(BAT_P, 0, 12, 7);
    u8g2.drawBox(BAT_P + 12, 2, 2, 3);
    BAT_LEV_C = 7.142857 * BAT_LEV - 20;

    u8g2.drawBox(BAT_P + 1, 1, max(min(BAT_LEV_C + 1, 10), 0), 5);




    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    tt = WiFi.localIP().toString();
    //if (tt.indexOf(".") < 1) {
    if (tt=="0.0.0.0") {
      tt = WiFi.softAPIP().toString();
      //if (tt.indexOf(".") < 1) {
      if (tt=="192.168.4.1") {
        u8g2.drawGlyph(1, 8, 281);
        u8g2.setFont(u8g2_font_6x13_me);
        u8g2.drawStr(18, 20, "Connect to Wifi");
        u8g2.drawStr(19, 34, " AutoConnectAP");
        u8g2.drawStr(14, 48, "enter");
        u8g2.drawStr(48, 48, tt.c_str ());
        u8g2.drawStr(21, 62, "in web browser");
        Con=0;
      }
      else {
        u8g2.setFont(u8g2_font_6x13_me);
        u8g2.drawStr(0, 20, "reconnecting to Wifi");
        Con=0;
      }
    }
    else {

      u8g2.drawGlyph(1, 8, 248);
    }
    u8g2.setFont(u8g2_font_trixel_square_tn);
    u8g2.setFont(u8g2_font_timR08_tn);
    u8g2.drawStr(12, 8, tt.c_str ());
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    if (!digitalRead(0)){// if (In_Level > BAT_LEV) {
      u8g2.drawGlyph(121, 8, 96);
    }



    // u8g2.setMaxClipWindow();
    // u8g2.drawRFrame(13, 15, 102, 20, 3);
    // u8g2.drawRBox(14, 16, 2, 18, 1);
    //   u8g2.setFont(u8g2_font_6x13_me);
    // u8g2.drawStr(34, 52, "Updating...");


    if (finish) {
      // u8g2.drawStr(
      if (Con){
      u8g2.setFont(u8g2_font_VCR_OSD_mf);
      u8g2.drawStr(17, 27,  digitalClockDisplay().c_str());
      //u8g2.drawStr(5, 27 + 18,  digitalClockDisplay().c_str());
      u8g2.drawStr(5, 27 + 18,  digitalDateDisplay().c_str());
      u8g2.drawStr(20, 27 + 36,  String(TempF).c_str() );
      u8g2.drawGlyph(85, 27 + 36, 176);
      u8g2.drawStr(98, 27 + 36,  "F");}
      u8g2.nextPage();
    }
    u8g2.setFont(u8g2_font_6x13_me);

  }
  else {
     u8g2.firstPage();
        u8g2.setDrawColor(0);

    u8g2.drawBox(0, 0, 128, 64);
            u8g2.setDrawColor(1);
                u8g2.nextPage();
                ScreenSleep=1;
  }
}


void UpdateScreenLoading(int cur, int total) {
  //Serial.println(cur);
  //Serial.println(total);
  Update_Screen(0);
  u8g2.setDrawColor(1);
  u8g2.drawRFrame(10, 14, 102, 20, 3);
  int length_bar = double(cur) / double(total) * 100;
  // Serial.println(length_bar);
  if (length_bar >= 4) {
    u8g2.drawRBox(11, 15, max(length_bar, 4) , 18, min(length_bar - 3, 3));
  }
  u8g2.drawStr(35, 53, "Updating...");
  u8g2.nextPage();


}


void UpdateScreenComplete(bool goodupdate) {
  Update_Screen(0);
  if (goodupdate) {
    u8g2.drawStr(15, 40, "Finished Updating");
    u8g2.drawStr(25, 60, "Restarting...");
  }
  else {
    u8g2.drawStr(17, 40, "Failed Updating");
    u8g2.drawStr(25, 60, "Restarting...");
  }
  u8g2.nextPage();

}
