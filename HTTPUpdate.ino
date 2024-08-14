//String URL_fw_Version "https://raw.githubusercontent.com/tinkydan/WiFiChickenDoor/main/bin_version.txt";
//String URL_fw_Bin "https://raw.githubusercontent.com/tinkydan/WiFiChickenDoor/main/ChickenDoorFW.bin";


void UpdateProgressBar(int cur, int total) {
  u8g2.drawFrame(10, 45, 102, 12);
  u8g2.drawBox(11, 46, cur / total * 100, 10);

}



void FirmwareUpdate()
{
//  t_httpUpdate_return ret;
// // WiFiClientSecure client;
//  //client.setTrustAnchors(&cert);
//  //client.setInsecure();
////    client.setTrustAnchors(&cert);
//  if (!client.connect(host, httpsPort)) {
//    Serial.println("Connection failed");
//    return;
//  }
//  String MAC = WiFi.macAddress();
//  String collon = ":";
//  String blank = "";
//  MAC.replace(":", "");
//  Serial.println(String("GET ") + URL_fw_Version_C + MAC + ".txt HTTP/1.1\r\n" +
//                 "Host: " + host + "\r\n" +
//                 "User-Agent: BuildFailureDetectorESP8266\r\n" +
//                 "Connection: close\r\n\r\n");
//  //Checking for custom build based on the mac address
//  client.print(String("GET ") + URL_fw_Version_C + MAC + ".txt HTTP/1.1\r\n" +
//               "Host: " + host + "\r\n" +
//               "User-Agent: BuildFailureDetectorESP8266\r\n" +
//               "Connection: close\r\n\r\n");
//  bool Custom_FW = 0;
//  bool Good_FW = 0;
//  while (client.connected()) {
//    String line = client.readStringUntil('\n');
//    Serial.println(line);
//    if (line.indexOf("200 OK") > 0) Custom_FW = 1;
//    if (line == "\r") {
//      //Serial.println("Headers received");
//      break;
//    }
//  }
//  String payload = client.readStringUntil('\n');
//  if (Custom_FW) {
//    Serial.print("A custom firmware is avaliable version: ");
//    Serial.println(payload);
//  }
//  else {
//    client.flush();
//
//
//  //  client.setInsecure();
//    if (!client.connect(host, httpsPort)) {
//
//      Serial.println(client.connect(host, httpsPort));
//      Serial.println("Connection failed");
//      return;
//    }
//    Serial.print(String("GET ") + URL_fw_Version + " HTTP/1.1\r\n" +
//                 "Host: " + host + "\r\n" +
//                 "User-Agent: BuildFailureDetectorESP8266\r\n" +
//                 "Connection: close\r\n\r\n");
//    client.print(String("GET ") + URL_fw_Version + " HTTP/1.1\r\n" +
//                 "Host: " + host + "\r\n" +
//                 "User-Agent: BuildFailureDetectorESP8266\r\n" +
//                 "Connection: close\r\n\r\n");
//
//    while (client.connected()) {
//      String line = client.readStringUntil('\n');
//      Serial.println(line);
//      if (line.indexOf("200 OK") > 0) Good_FW = 1;
//      if (line == "\r") {
//        //Serial.println("Headers received");
//        break;
//      }
//    }
//    payload = client.readStringUntil('\n');
//    Serial.println(payload);
//  }
//
//  if (payload.equals(FirmwareVer) )
//  {
//    Serial.println("Device already on latest firmware version");
//  }
//  else if (Custom_FW || Good_FW)  {
//    Serial.println("New firmware detected");
////    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
//    UpdateScreenLoading(0, 10) ;
//    if (Custom_FW) {
//    ret = ESPhttpUpdate.update(client, URL_fw_Bin_C + MAC + ".bin");
//    }
//    else if (Good_FW) {
//    ret = ESPhttpUpdate.update(client, URL_fw_Bin);
//    }
//
//    switch (ret) {
//      case HTTP_UPDATE_FAILED:
//        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
//        break;
//
//      case HTTP_UPDATE_NO_UPDATES:
//        Serial.println("HTTP_UPDATE_NO_UPDATES");
//        break;
//
//      case HTTP_UPDATE_OK:
//        Serial.println("HTTP_UPDATE_OK");
//        break;
//    }
//  }
//  else {
//    Serial.println("Cannot Connect to Github");
//  }
}  //  client.setInsecure();
