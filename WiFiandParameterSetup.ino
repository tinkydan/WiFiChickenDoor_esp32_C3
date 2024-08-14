void WiFiManegerSetup(){


  
  WiFiManagerParameter custom_html(); // only custom html
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "", 40);
  WiFiManagerParameter custom_token("api_token", "api token", "", 16);
  

  const char _customHtml_checkbox[] = "type=\"checkbox\""; 
  WiFiManagerParameter custom_checkbox("checkbox", "my checkbox", "T", 2, _customHtml_checkbox, WFM_LABEL_AFTER);



}

void SaveCustomParams (){
  

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

}
