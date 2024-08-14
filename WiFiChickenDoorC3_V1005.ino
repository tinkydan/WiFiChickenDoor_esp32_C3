#include <math.h>

// Improvements needed
// -> Add infomation about door status to the info page (info is handled in the Wifimanger.ccp function so a forked library would nedd to be used
// -> fill outthe setting options for deep sleep timeouts and other, also include a thingspeak channel in the settings page


/////////////////////////////////////////////////////////////////////////////////////////
///////////// LIBRARYS USED IN THE FOLLOWING CODE ///////////////////////////////////////
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

// include MDNS
#ifdef ESP8266
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <ESPmDNS.h>
#endif

WiFiManagerParameter custom_field; // global param ( for non blocking w params )

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#include <NTPClient.h>


#include <HardwareSerial.h>
#include <TinyGPS.h>

#include <Dusk2Dawn.h>

//#include <NTPtimeESP.h>

#include <TimeLib.h>
#include <Ticker.h>


#include <WiFi.h>
#include <mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


#include <EEPROM.h>
#include <WebServer.h>

#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>


//Needed for webupdate
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
#include <WiFiClientSecure.h>

#include <time.h>



Ticker blinker;
// WIfi Maneger
WiFiManager wm;

unsigned int  timeout   = 120; // seconds to run for
unsigned int  startTime = millis();
bool portalRunning      = false;
bool startAP            = true; // start AP and webserver if true, else start only webserver

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;


// Included for NPT time
//#include <WiFiUdp.h>
//#include <HTTPClient.h>
//#include <HTTPUpdate.h>
//#include <WiFiClientSecure.h>

//const char* host = "ChickenDoor";
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 7, /* data=*/ 6, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 6, /* data=*/ 7);

IPAddress IP_SERVER(192, 168, 1, 1);

TinyGPS gps;
HardwareSerial SerialGPS(0);



#define ONE_WIRE_BUS 8
#define I2C_SCL 6
#define I2C_SCA 7

#define MPB_SET 9
#define MPB_MENU 8


#define MOTOR_IN1 4
#define MOTOR_IN2 5

#define ACC_EN 10

#define ACC_GPS 10
#define TX_GPS 20
#define RX_GPS 21
#define BUTTON_PIN_BITMASK 0x8004

#define Vin 2
#define Bat 3
#define Cur 0
#define V_Sense 1

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
//ConfigStruct configs


//WiFiUDP ntpUDP;
// initialized to a time offset of 10 hours

//NTPClient timeClient(ntpUDP, "us.pool.ntp.org", 0, 60000);

//NTPClient timeClient(ntpUDP);
//NTPClient::NTPClient(UDP& udp, const char* poolServerName, long timeOffset, unsigned long updateInterval) {
///////////////////  THINGSPEAK SETUP ////////////////////////////////////////////

//NTPtime NTPch("us.pool.ntp.org");
const char* serverTS = "api.thingspeak.com";
const char* hostTS = "api.thingspeak.com";
char APIkey[17] = "________________";
byte StateField = 1;
byte TempField = 2;
byte BatField = 3;
byte InField = 4;


/////////////////// SYSTEM SETTINGS //////////////////////////////////////
unsigned long RTC_UPDATE_ms = 1000;
unsigned long RTC_LAST = 0;
unsigned long Time_Record_ms = 1000; // GPS reading
unsigned long Time_Record_LAST = 0;
unsigned long Time_NPT_ms = 6000000; // NPT read /every 100mins
unsigned long Time_NPT_LAST = 0;
unsigned long DATA_Upload_ms = 1800000; // Every 30 mins
unsigned long DATA_Upload_LAST = 0;
unsigned long DOOR_CNTR_ms = 200;
unsigned long DOOR_CNTR_LAST = 0;
unsigned long Temp_ms = 5000;
unsigned long Temp_LAST = 0;
unsigned long ACC_timout_ms = 1800000; // Attempt to get GPS time for 30 mins
unsigned long ACC_LAST = 0;
unsigned long JAM_REV = 0;

unsigned long LCD_LOCK_ms = 8000;
unsigned long LCD_LOCK_LAST = 0;

unsigned long EEPROM_Upload_ms = 2000; //Min time between EEPROM Updates
unsigned long EEPROM_Upload_LAST = 0;

unsigned long LCD_Sleep_ms = 200000;//3000000; //50 Mins
unsigned long LCD_Sleep_Start = 0;

unsigned long   currentMillis = 0;
///////////////////// Time Keeping Variables ////////////////////////////

byte SetWait = 0;
int debounce_ms = 15;
byte MenuWait = 0;
bool ScreenSleep = 0;
int NPT_ATTEMPTs = 0;
int NPT_UPDATE_VALID_ATTEMPT = 3; // will not update time if changing by more than 5mins unless validated this many times
int hr_lst, min_lst, sec_lst, day_lst, month_lst, year_lst; // stop the last NTP that didnt conform with the current set time
int hr_set, min_set, sec_set, day_set, month_set, year_set; // Timestamp from when the closck was set last
int second_err_T=0;


///////////////////// Variable Definition ///////////////////////////////

#define LCD_C     LOW
#define LCD_D     HIGH
// used in this example to print variables every 10 seconds
unsigned long printEntry;
String deviceName = "ChickenDoor";
String chipId;

Ticker SetCallback;

Ticker MenuCallback;


const int numReadings = 1000;
float readings_V[numReadings];
int16_t readings_FB[numReadings];

byte aHour = 12;
String AM;

double sensorValue;
int holdI = 0;
bool IP = HIGH;
int MESunrise = 0;
int MESunset = 0;
byte Ayear = 0;
bool first = HIGH;
int second_cur, minute_cur, hour_cur, date_cur, month_cur, year_cur, temperature;
int iter = 0;
int CUR_time = 0;
int LAST_COM = 0;
bool blink_test = 0;
bool Century = false;
bool h12;
bool PM;
int OVER_LIM = 0;
bool going_up = 0;
bool going_down = 0;
int son = 500;
int JAMED = 0;
float BAT_LEV = 0;
float In_Level = 0;
int Correction_cnt = 0;
String ACT_S[2] = {"inactive", "active"};
String DIRC_S[3] = {"stoped", "going down", "going up"};
String sensor[3] = {"Trigged", "Not Trigged", "Unpluged"};
int REQtime;
String HTML, HTML2, Link;
bool DFirst = 1;
float TempF = 0;
float TempH;
int tcount;
int Time_Method = 3;
int TIME_SET_BY = 0;

int Feed_time = 0;
int Feed_length = 0;
int eepS;

unsigned long age;
int Year;
byte Month, Day, Hour, Minute, Second;
int ajH;

byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

float flat, flon;
String tt;
int Move_Complete = 1;
int gps_time_updated = 0;
int interTIME = 0;
int interTIMElast = 0;
int Time_ZONE = -5;
float LATT = 43.;
float LONG = -70.;
int After_Sunrise = 5 ;
int After_Sunset = 20;
int After_Sunrise_hr = 0 ;
int After_Sunset_hr = 0;
int up_limit = 800;
int down_limit = 300;
Ticker secondTick;
unsigned long _updateInterval = (3600 * 1000);

volatile bool is_active = 0;
int Top = 0;
int Bot = 0;
byte is_top = 0;
byte is_bot = 0;
int DOWNREQUEST = 0;
int UPREQUEST = 0;
bool STOPREQUEST = 0;
int FB_READ;
int request_direc = 0; //1 - up requested 2 - Down Requested
byte DSE = 0; // 0 -
byte DSE_TV = 0;
uint64_t sleepT;

int16_t val_01;
float volts_01;
int16_t val_2;
int16_t val_3;

byte control_method = 0;
bool lowtemplockout = 0;
float lowtemplockout_val = 10;
bool DOW[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
String Close_Time_String = "10";
String Open_Time_String = "10";

String FirmwareVer = {
  "1.008"
};
#define URL_fw_Version_C "https://raw.githubusercontent.com/tinkydan/WiFiChickenDoorCustom/main/bin_version_"
#define URL_fw_Bin_C "https://raw.githubusercontent.com/tinkydan/WiFiChickenDoorCustom/main/ChickenDoorFW"
#define URL_fw_Version "https://raw.githubusercontent.com/tinkydan/WiFiChickenDoor/main/bin_version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/tinkydan/WiFiChickenDoor/main/ChickenDoorFW.bin"
const int httpsPort = 443;
const char* host = "raw.githubusercontent.com";

#define EEPROM_SIZE 128
#define uS_TO_S_FACTOR 1000000ULL


float f = 0;

//NTPtime NTPch("ch.pool.ntp.org");

WiFiClient client;

#define DEBUG_NTPClient
#define DEBUG
#define DEBUG_1SEC


#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.println (x)
#else
#define DEBUG_PRINT(x)
#endif

#ifdef DEBUG_1SEC
#define DEBUG_PRINT_1SEC(x)  Serial.println (x)
#else
#define DEBUG_PRINT_1SEC(x)
#endif


static const unsigned char COOP [] U8X8_PROGMEM = {
  // 'PCB_pcb copy_2021-09-24 (2), 91x64px
  0x00, 0x00, 0x00, 0x02, 0xbc, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12,
  0xcc, 0x01, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xf0, 0x0b, 0x00, 0x00,
  0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0xf8, 0x03, 0x00, 0x00, 0x41, 0x56, 0x00, 0x00,
  0x00, 0x80, 0x00, 0x00, 0xfe, 0x37, 0x00, 0x00, 0x20, 0x44, 0x00, 0x00, 0x08, 0x90, 0x00, 0x00,
  0xff, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0xc8, 0xff, 0x2d, 0x00, 0x0f,
  0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x7f, 0x08, 0x20, 0x3e, 0x00, 0x1c, 0x00, 0x00,
  0xc0, 0x30, 0x00, 0xf8, 0x3f, 0x58, 0x60, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x04, 0x44, 0x00, 0xfe,
  0x3f, 0x10, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x31, 0x00, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x63, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xfe, 0xff, 0x43, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0xff,
  0xff, 0xc7, 0x00, 0x08, 0x08, 0x18, 0x00, 0x00, 0x00, 0x30, 0x80, 0xff, 0xff, 0x8f, 0x45, 0x00,
  0x38, 0x00, 0x00, 0x00, 0x02, 0x30, 0x80, 0xff, 0xff, 0x1f, 0x01, 0x00, 0x30, 0x06, 0x00, 0x00,
  0x03, 0x00, 0xc0, 0xff, 0xff, 0x1f, 0x02, 0xc0, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff,
  0xff, 0x3f, 0x26, 0x80, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x7f, 0x4c, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xd8, 0x00, 0x00, 0x80, 0x01, 0x00,
  0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x31, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
  0xff, 0xff, 0x61, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0xc0, 0x7f, 0xfe, 0xff, 0xff, 0xc3, 0x04,
  0x00, 0x18, 0x00, 0x00, 0x00, 0xf0, 0xde, 0xff, 0xff, 0xff, 0x8f, 0x0b, 0x00, 0x04, 0x00, 0x00,
  0x00, 0x3c, 0xc7, 0xff, 0xff, 0xff, 0x1f, 0x27, 0x00, 0x01, 0x02, 0x00, 0x00, 0xc7, 0xc3, 0xff,
  0xff, 0xff, 0x3f, 0x4d, 0xf9, 0x00, 0x02, 0x00, 0x00, 0xc0, 0xf8, 0xff, 0xff, 0xff, 0xbf, 0xb8,
  0x1f, 0x00, 0x02, 0x00, 0x00, 0x00, 0xf8, 0x7f, 0x00, 0x80, 0xff, 0x60, 0x00, 0x00, 0x06, 0x80,
  0x00, 0x00, 0xf8, 0x7f, 0x01, 0x80, 0xff, 0x01, 0xc0, 0xc0, 0x06, 0x80, 0x00, 0x00, 0xfc, 0x7f,
  0x0b, 0xcc, 0xff, 0xe1, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x7f, 0x7f, 0xe0, 0xff, 0x01,
  0xfe, 0xff, 0xf1, 0x1f, 0x00, 0x00, 0xfe, 0x3f, 0x5e, 0xe0, 0xff, 0x09, 0x00, 0xc2, 0x53, 0x00,
  0x00, 0x00, 0xfe, 0x3f, 0x08, 0xc0, 0xff, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x3f,
  0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x3f, 0x00, 0x80, 0xff, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xff, 0x3f, 0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x20, 0xff, 0x3f, 0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xff, 0x7f,
  0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x01,
  0x00, 0x00, 0x00, 0x40, 0x00, 0xe0, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0x08, 0x40,
  0x24, 0xe0, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x01, 0x00, 0x40, 0x00, 0x00, 0x20, 0x60, 0xff, 0x7f,
  0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x01,
  0x86, 0x1b, 0x00, 0x00, 0x00, 0x60, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x31, 0xf8, 0x4b, 0xc0, 0x03,
  0x00, 0x60, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x31, 0xf0, 0x08, 0x02, 0x3f, 0x00, 0x70, 0xff, 0x7f,
  0x00, 0x80, 0xff, 0x01, 0x70, 0x00, 0x0a, 0x19, 0x00, 0x60, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x01,
  0xe0, 0x07, 0xea, 0x01, 0x00, 0xf0, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0xf0, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0x08, 0x00, 0x02, 0xe0, 0xff, 0x3f,
  0x00, 0x80, 0xff, 0x9d, 0xfc, 0x1e, 0x08, 0x40, 0x00, 0xe0, 0xff, 0x3f, 0x00, 0x80, 0xff, 0xfd,
  0xff, 0x7f, 0x3a, 0x60, 0x00, 0xe0, 0xff, 0x3f, 0x00, 0x80, 0xff, 0xfd, 0xff, 0xff, 0x7a, 0x60,
  0x00, 0xe0, 0xff, 0x7f, 0x00, 0x80, 0xff, 0xed, 0xf7, 0xff, 0xfb, 0xe1, 0x00, 0xe0, 0xff, 0x7f,
  0x00, 0x80, 0xff, 0x05, 0x40, 0xf4, 0x7a, 0x7d, 0x00, 0xe0, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x01,
  0xe0, 0x0f, 0x08, 0x78, 0x00, 0xc0, 0xff, 0x7f, 0x00, 0xfe, 0xff, 0x01, 0x00, 0x04, 0x00, 0x30,
  0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xdb, 0xff,
  0xff, 0xc1, 0x08, 0x18, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xb9, 0xfe, 0xf9, 0xf7, 0xee, 0xf8, 0x7f,
  0x00, 0x00, 0xfc, 0xff, 0x1f, 0x18, 0xfc, 0xf9, 0x07, 0x00, 0xe8, 0xff, 0x00, 0x00, 0xf8, 0x13,
  0x00, 0x08, 0xf8, 0xe1, 0x07, 0x00, 0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 0x00, 0x08, 0xf8, 0xe1,
  0x07, 0x00, 0x04, 0xfc, 0x00, 0x00, 0x38, 0x00, 0x60, 0x08, 0xf8, 0x61, 0x00, 0x00, 0x04, 0xff,
  0x00, 0x18, 0x38, 0x00, 0x60, 0x18, 0xfc, 0x01, 0x00, 0x00, 0x0c, 0xfc, 0x00, 0x00, 0x38, 0x00,
  0x00, 0x18, 0xfc, 0x01, 0x04, 0x00, 0x1c, 0xc4, 0x00, 0x00, 0x78, 0x00, 0x00, 0x78, 0xf8, 0x00,
  0x00, 0x00, 0xfc, 0x07, 0x00, 0x00, 0x78, 0x00, 0x00, 0xf8, 0xc0, 0x00, 0x00, 0x00, 0xfc, 0x07,
  0x00, 0x00, 0x78, 0x00, 0x00, 0xf8, 0xd0, 0x30, 0x00, 0x00, 0xf4, 0x07, 0x00, 0x00, 0x78, 0x00,
  0x00, 0x58, 0xc8, 0x00, 0x00, 0x00, 0xe4, 0x43, 0x00, 0x00, 0x78, 0x00, 0x00, 0x10, 0xc8, 0x00,
  0x00, 0x00, 0xc4, 0x02, 0x00, 0x00, 0x32, 0x06, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x38, 0x02
};
extern const unsigned char caCert[] PROGMEM;
extern const unsigned int caCertLen;
// Initialize IotAppStory
//AsyncWebServer serverV(80);

//

void setup() {

  WiFiManagerParameter custom_html("<p>This Is Custom HTML</p>"); // only custom html
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  

  // add all your parameters here
  wm.addParameter(&custom_html);
  wm.setWebServerCallback(bindServerCallback);
  // WiFi Maneger Setup
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  //WiFi.mode(WIFI_AP_STA);
  // put your setup code here, to run once:

  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, RX_GPS, TX_GPS);
  Serial.println("Starting EEPROM");
  EEPROM.begin(EEPROM_SIZE);
  //EEPROM
  if (1 == 0) {
    Serial.println("Setting EEPROM");
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
    EEPROM.put( 56, APIkey); //17 end 74
    EEPROM.put( 74, StateField);
    EEPROM.put( 75 , TempField);
    EEPROM.put( 76 , BatField);
    EEPROM.put( 77 , InField);
    EEPROM.put( 78 , DSE);
  }
  EEPROM.get( 0, LATT);
  EEPROM.get( 4, LONG);
  EEPROM.get( 8, Time_ZONE);
  EEPROM.get( 16, control_method);
  EEPROM.get( 17, lowtemplockout);
  EEPROM.get( 20, up_limit);
  EEPROM.get( 24, down_limit);
  EEPROM.get( 28, DOW);
  EEPROM.get( 36, lowtemplockout_val);
  EEPROM.get( 40, After_Sunrise);
  EEPROM.get( 44, After_Sunset);
  EEPROM.get( 48, After_Sunrise_hr);
  EEPROM.get( 52, After_Sunset_hr);
  EEPROM.get( 56, APIkey); //17 end 74
  EEPROM.get( 74, StateField);
  EEPROM.get( 75 , TempField);
  EEPROM.get( 76 , BatField);
  EEPROM.get( 77 , InField);
  EEPROM.get( 78 , DSE);

  //Dusk2Dawn ME(LATT, LONG, Time_ZONE);
  if (After_Sunrise_hr > 0) {
    Open_Time_String = String(After_Sunrise_hr) + ":" + String(After_Sunrise);
  }
  else {
    Open_Time_String = String(After_Sunrise);
  }
  if (After_Sunrise_hr > 0) {
    Close_Time_String = String(After_Sunset_hr) + ":" + String(After_Sunset);
  }
  else {
    Close_Time_String = String(After_Sunset);
  }

  //reset settings - wipe credentials for testing
  //wm.resetSettings();
  // wm.addParameter(&custom_mqtt_server);
  wm.setConfigPortalBlocking(false);
  wm.setCaptivePortalEnable(true);
  // wm.setSaveParamsCallback(saveParamsCallback);
  std::vector<const char *> menu = {"wifi", "wifinoscan", "info", "param", "sep", "erase", "restart"};
  wm.setMenu(menu); // custom menu, pass vector
  //automatically connect using saved credentials if they exist
  //If connection fails it starts an access point with the specified name
  if (wm.autoConnect("AutoConnectAP")) {
    Serial.println("connected... :)");
  }
  else {
    Serial.println("Configportal running");
  }
  DEBUG_PRINT("Connecting to time server");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  tcount = 0;
  UpdateNPTTime();

  //timeClient.setTimeOffset(3600 * Time_ZONE);
  //timeClient.begin();

  u8g2.begin();
  Serial.println(u8g2.getDisplayWidth());
  Serial.println(u8g2.getDisplayHeight());


  //  u8g2.clearDisplay(); //for Clearing the display
  //
  //  //         u8g2.setCursor(0, 15);
  //    //u8g2.println ("V1.1");


  u8g2.firstPage();
  u8g2.setFont(u8g2_font_6x13_me);
  //u8g2.drawStr(0, 0, "V1.1");
  u8g2.drawXBM(32, 0, 96, 72, COOP); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  u8g2.drawStr(0, 0, "V1.1");

  u8g2.nextPage();



  //  //u8g2.display();
  delay(500);

  // Wire.begin();
  //Update_Screen(1);

  delay(1000);
  Serial.println("Initilizing");

  char sz[32];


  if ((DSE == 1) || ((DSE == 2) && (BAT_LEV < 3.4)) || ((DSE == 3) && (In_Level < 1))) {
    // Deep sleep enabled
    DSE_TV = 1;
  }






  //  esp_task_wdt_init(60, true);
  // esp_task_wdt_add(NULL);
  //ESPhttpUpdate.onStart(update_started);
  // ESPhttpUpdate.onEnd(update_finished);
  // ESPhttpUpdate.onProgress(update_progress);
  // ESPhttpUpdate.onError(update_error);

  DEBUG_PRINT("Starting Web Portal");
  wm.setSaveConfigCallback(restart_portal);
  wm.startWebPortal();
  DEBUG_PRINT("Configuring Pins");
  pinMode(MPB_SET, INPUT);
  pinMode(MPB_MENU, INPUT);
  // WiFi.softAP("AutoConnectAP");
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(ACC_EN, OUTPUT);
  digitalWrite(ACC_EN, HIGH);
  
     TempH = sensors.getTempCByIndex(0) * 9 / 5 + 32;
    if (TempH > -150 & TempH != 0) {
      TempF = TempH;
       DEBUG_PRINT("Temperature: " + String(TempF));
    }


}


void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
  UpdateScreenComplete(1);
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
  UpdateScreenLoading(cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
  UpdateScreenComplete(0);
}

void restart_portal() {
  blinker.attach(5, restart_portal_delayed); //Use attach_ms if you need time in ms
}

void restart_portal_delayed() {
  wm.startWebPortal();
  // WiFi.softAP("AutoConnectAP");
  Serial.println("Config portal running");
  Serial.println(WiFi.softAPIP());
  blinker.detach();
}


void MenuRecheck() {
  MenuWait = 2;
  if (!digitalRead(MPB_MENU)) {
    Serial.println("Menu Callback Button");
    LCD_Sleep_Start = millis();
  }
  else {
    Update_Screen(1);
  }
  ScreenSleep = 0;
  MenuCallback.detach();
}

void SetRecheck() {
  SetWait = 2;
  if (!digitalRead(MPB_SET)) {
    Serial.println("Set Callback Button");
    LCD_Sleep_Start = millis();
  }
  else {
    Update_Screen(1);
  }
  ScreenSleep = 0;
  SetCallback.detach();
}
void loop() {

  // put your main code here, to run repeatedly:
  if (!is_active) {
    wm.process();
    // Check for Updates and things
  }
  //  esp_task_wdt_reset(); // Resets if board locks up

  currentMillis = millis();

  // Get status of the door
  analogSetAttenuation(ADC_2_5db);// Setting for 1.1V range
  sensorValue = (analogRead(V_Sense) + analogRead(V_Sense) + analogRead(V_Sense) + analogRead(V_Sense) + analogRead(V_Sense) + analogRead(V_Sense) + analogRead(V_Sense) + analogRead(V_Sense) + analogRead(V_Sense) + analogRead(V_Sense)) / 10;
  analogSetAttenuation(ADC_11db);// Default 2.6V
  if (sensorValue < 545) {
    is_top = 0;
    is_bot = 0;
  }
  else if (sensorValue < 1110) {
    is_top = 0;
    is_bot = 2;// UNPLUGGED
  }
  else if (sensorValue < 1363) {
    is_top = 2;// UNPLUGGED
    is_bot = 0;
  }
  else if (sensorValue < 1700) {
    is_top = 1;
    is_bot = 0;
  }
  else if (sensorValue < 2077) {
    is_top = 0;
    is_bot = 1;
  }
  else if (sensorValue < 2471) {
    is_top = 2;
    is_bot = 2;// UNPLUGGED
  }
  else if (sensorValue < 2856) {
    is_top = 1;
    is_bot = 2;// UNPLUGGED
  }
  else if (sensorValue < 3225) {
    is_top = 2;
    is_bot = 1;// UNPLUGGED
  }
  else {
    is_top = 0;
    is_bot = 0;
  }


  if ((!digitalRead(MPB_SET)) && (!SetWait)) {
    SetWait = 1;
    SetCallback.attach_ms(debounce_ms, SetRecheck); //Use attach_ms if you need time in ms
  }
  else if (digitalRead(MPB_SET) && (SetWait == 2)) {
    SetWait = 0;
  }

  if ((!digitalRead(MPB_MENU)) && (!MenuWait)) {
    MenuWait = 1;
    MenuCallback.attach_ms(debounce_ms, MenuRecheck); //Use attach_ms if you need time in ms
  }
  else if (digitalRead(MPB_MENU) && (MenuWait == 2)) {
    MenuWait = 0;
  }


  CUR_time = minute() + 60 * hour();
  if ((LAST_COM > 0) && (CUR_time < 30)) { //Resets last time commanded at midnight
    LAST_COM = 0;
  }


  if (blink_test) {
    if (!is_active) {

      if (is_bot) {
        delay(4000);
        UPREQUEST = 1;
        DEBUG_PRINT("Test Mode: At Bottom Reversing");
      }
      else if (is_top) {
        delay(4000);
        DOWNREQUEST = 1;
        DEBUG_PRINT("Test Mode: At Top Reversing");
      }
      else if (!is_top && !is_bot) {
        delay(4000);
        DEBUG_PRINT("Test Mode: Disconnected Going Up");
        UPREQUEST = 1;
      }
    }
  }


  if (((((CUR_time)) - After_Sunset) > MESunset) && ((((CUR_time)) - After_Sunset) < (MESunset + 15)) ) {
    if (is_bot != 1 && !OVER_LIM && ((LAST_COM + 1) < CUR_time)) {
      LAST_COM = CUR_time;
      DOWNREQUEST = 1;
      tcount = 0;
      DEBUG_PRINT("Sun has set Closing");
      UpdateNPTTime();
    }
  }
  else if ( (((CUR_time) - After_Sunrise) > MESunrise) && (((CUR_time) - After_Sunrise) < (MESunrise + 15))) {
    if (is_top != 1 && !OVER_LIM && ((LAST_COM + 1) < CUR_time)) {
      LAST_COM = CUR_time; // Makes sure this is only called once every min
      UPREQUEST = 1;
      DEBUG_PRINT("Sun has risen Opening");
      UpdateNPTTime();
      first = HIGH; // Reseting the GPS bool so that GPS time will be used to update clock
    }
  }


  if (is_top == 1 && going_up && (is_top != is_bot)) { // Not sure it shoud not just stop if sensor is unpluged
    if (request_direc == 1) {
      STOPREQUEST = 1;
      DEBUG_PRINT("Is fully opened motor stopping");
    }
    if (request_direc == 2) {
      DOWNREQUEST = 2;
      DEBUG_PRINT("Is fully open but close requested motor reversing");
    }
  }

  if (is_bot == 1 && going_down  && (is_top != is_bot)) { // Not sure it shoud not just stop if sensor is unpluged
    if (request_direc == 2) {
      STOPREQUEST = 1;
      DEBUG_PRINT("Is fully closed motor stopping");
    }
    if (request_direc == 1) {
      UPREQUEST = 2;
      DEBUG_PRINT("Is fully closed but open requested motor reversing");
    }
  }


  //Wire.begin(5,4);
  FB_READ =  (analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur)) / 10;
  // Wire.begin();

  if ((((FB_READ > up_limit) && (going_up)) || ((FB_READ > down_limit) && (going_down))) && (millis() + 9000 >= JAM_REV )) {
    delay(500);
    DEBUG_PRINT("Motor Feedback of " + String(FB_READ) + " Exceeded the limit of " + String(up_limit));
    FB_READ =  (analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur) + analogRead(Cur)) / 10;
    DEBUG_PRINT("After a delay the Motor Feedback of " + String(FB_READ) + " Still exceeded the limit of " + String(up_limit));
    if (FB_READ > down_limit) {
      OVER_LIM++;
      if (going_down && (OVER_LIM > 3)) {
        STOPREQUEST = 1;
        JAMED = 1;
        DEBUG_PRINT("Jamed going down after reattempting, door stopped");

      }
      if (going_up && (OVER_LIM > 4)) {
        STOPREQUEST = 1;
        JAMED = 2;
        DEBUG_PRINT("Jamed going Up after reattempting, door stopped");

      }
      else if (going_down) {
        UPREQUEST = 2;
        DEBUG_PRINT("Jamed sending up");
      }
      else if (going_up) {
        DOWNREQUEST = 2;
        DEBUG_PRINT("Jamed sending down");
      }
      JAM_REV = millis() + 10000;
    }
  }

  // reveses the motor after a jam if other position has not been reached


  if ((OVER_LIM > 0) && (JAM_REV > 0) && (millis() >= JAM_REV )) {
    JAM_REV = 0;
    if (going_down && (request_direc == 1)) {
      UPREQUEST = 2;
      DEBUG_PRINT("Motor reversing, attempting to open again previously jammed when opening");
    }
    else if (going_up && (request_direc == 2)) {
      DOWNREQUEST = 2;
      DEBUG_PRINT("Motor reversing, attempting to close again previously jammed when closing");
    }
  }

  //////////////////////// Excecute Close or open Commands ///////////////////////////
  if (DOWNREQUEST >= 1)
  {
    ACC_LAST = currentMillis;
    Serial.println("Going Down");
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    digitalWrite(ACC_EN, HIGH);
    //    digitalWrite(BL, HIGH);
    is_active = HIGH;
    going_down = HIGH;
    going_up = LOW;
    if (DOWNREQUEST == 1) {
      OVER_LIM = 0;
      JAMED = 0;
      DOWNREQUEST = 0;
      request_direc = 2;
    }
    if (DOWNREQUEST == 2) {
      delay(100);
      DOWNREQUEST = -1;
    }


  }
  if (UPREQUEST >= 1)
  { ACC_LAST = currentMillis;
    Serial.println("Going Up");
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
    digitalWrite(ACC_EN, HIGH);

    is_active = HIGH;
    going_up = HIGH;
    going_down = LOW;
    if (UPREQUEST == 1) {
      OVER_LIM = 0;
      JAMED = 0;
      UPREQUEST = 0;
      request_direc = 1;
    }
    if (UPREQUEST == 2) {
      delay(100);
      UPREQUEST = -1;
    }

  }


  if (STOPREQUEST == 1)
  { STOPREQUEST = 0;
    request_direc = 0;
    Serial.println("Stopping");
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
    Move_Complete = 1;
    going_up = LOW;
    going_down = LOW;
    OVER_LIM = 0;
    is_active = 0;

  }



  ////////////////////////// RTC UPDATE & SUN TIMES ///////////////////////////////////  /
  if (currentMillis - RTC_LAST >= RTC_UPDATE_ms )
  { RTC_LAST = currentMillis;
  DEBUG_PRINT_1SEC("Entering Clock/Screen Update");
    DEBUG_PRINT_1SEC(sensorValue);
    Update_Screen(1);
    //    print_time();
    DEBUG_PRINT_1SEC("IP Adress: " + WiFi.localIP().toString());
    tt = WiFi.localIP().toString();


    //Civil twilight sun down and rise times
    Dusk2Dawn ME(LATT, LONG, Time_ZONE);
    second_cur = second();
    minute_cur = minute();
    hour_cur = hour();
    date_cur = day();
    month_cur = month();
    year_cur = year();

    //
    MESunrise  = ME.sunrise(year_cur, month_cur, date_cur, false);//Calculating Civil Twilight times
    MESunset   = ME.sunset(year_cur, month_cur, date_cur, false);




  }
  ////////////////////////////////////////////////////////////////////////////
  if (currentMillis - Temp_LAST >= Temp_ms)
  { Temp_LAST = currentMillis;
     TempH = sensors.getTempCByIndex(0) * 9 / 5 + 32;
    if (TempH > -150 & TempH != 0) {
      TempF = TempH;

    }
     sensors.requestTemperatures();
  }
  ////////////////////////// DOOR CONTROL ////////////////////////////////////
  if (currentMillis - DOOR_CNTR_LAST >= DOOR_CNTR_ms)
  { DOOR_CNTR_LAST = currentMillis;
    if (hour_cur == 23 || hour_cur == 15) {
      //ESP.deepSleep(4e8);
    }
    // DEEP SLEEP
    //


    //ADS.setGain(1);
    val_2 = (analogRead(Vin) + analogRead(Vin) + analogRead(Vin) + analogRead(Vin) + analogRead(Vin) + analogRead(Vin) + analogRead(Vin) + analogRead(Vin) + analogRead(Vin) + analogRead(Vin)) / 10;
    val_3 = (analogRead(Bat) + analogRead(Bat) + analogRead(Bat) + analogRead(Bat) + analogRead(Bat) + analogRead(Bat) + analogRead(Bat) + analogRead(Bat) + analogRead(Bat) + analogRead(Bat)) / 10;


DEBUG_PRINT_1SEC(" C Sense V: " + String(sensorValue) );
DEBUG_PRINT_1SEC(" Vin int: " + String(val_2) + "   Bat int: " + String(val_3));
    BAT_LEV = (double(val_3) / 2715 * 4.08); //*0.780794+0.847166;
    // BAT_LEV = map(BAT_LEV, 2.45, 4.32, 2.76, 4.22);
    In_Level = val_2 * 5.03 / 835 * 4.92 / 7.16;
    In_Level = double(val_2) * 5 / 2222;
  DEBUG_PRINT_1SEC(" Vin: " + String( In_Level) + "   Bat: " + String(BAT_LEV));
    if (is_active) {
      for (int i = numReadings - 1; i > 0; i--)
      {
        readings_FB[i] = readings_FB[i - 1];
      }
      readings_FB[0] = -val_01 ;
    }
  }
  //////////////////////////////////////////////////////////////////////////////


  ////////////////////////// Time Update LOOP ////////////////////////////////////
  if (0) { //currentMillis - Time_Record_LAST >= Time_Record_ms || first ) {
    LCD_Sleep_Start = millis();

    DEBUG_PRINT_1SEC("First Time Update via GPS and/or NTP");


    Time_Record_LAST = currentMillis;
    while (SerialGPS.available()) {
      if (gps.encode(SerialGPS.read())) { // process gps messages
        DEBUG_PRINT("GPS signal lock, checking time validity");
        // when TinyGPS reports new data...
        gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
        char sz[32];

        if (age < 500 && Year > 2020 && Day > 0 ) {
          DEBUG_PRINT("GPS time apears valid benchmarking against current time setting");
          // set the Time to the latest GPS reading

          first = LOW;
          /// Get current position
          gps.f_get_position(&flat, &flon, &age);
          DEBUG_PRINT(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat);
          DEBUG_PRINT(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon);
          if ((flon != 0) || (flat != 0)) {

            TIME_SET_BY = 1; // Indicates that the time was captured by GPS
            gps_time_updated += 1;
            DEBUG_PRINT("Time Update: " + String(gps_time_updated));
            sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d   ", Month, Day, Year, Hour, Minute, Second);
            DEBUG_PRINT(sz);
            DEBUG_PRINT("Time Zone: " + String(Time_ZONE));
            if (gps_time_updated > 20) {
              ajH = Time_ZONE + Hour;
              if (ajH < 0) {
                ajH += 24;
                Day += -1;
              }
              if (ajH > 23) {
                ajH += -24;
                Day += 1;
              }
              setTime_R(ajH, Minute, Second, Day, Month, Year);


              // Serial.println("Time adjusted: " + web_time());
              if ((abs(sqrt(sq(flat) + sq(flon)) - sqrt(sq(LATT) + sq(LONG)))) > 0.01) {
                LATT = flat;
                LONG = flon;
                Time_ZONE = int((LONG - 7.5) / 15) + 1; //get_tz(LATT,LONG);
                DEBUG_PRINT("Location has changed recalculate location");
                EEPROM.begin(EEPROM_SIZE);
                EEPROM.put( 0 + eepS, LATT);
                EEPROM.put( 4 + eepS, LONG);
                EEPROM.put( 8 + eepS, Time_ZONE);
                EEPROM.commit();
                EEPROM.end();
                ajH = Time_ZONE + Hour;
                if (ajH < 0) {
                  ajH += 24;
                  Day += -1;
                }
                if (ajH > 23) {
                  ajH += -24;
                  Day += 1;
                }
                setTime_R(ajH, Minute, Second, Day, Month, Year);
                //              timeClient.setTimeOffset(3600 * Time_ZONE);

              }
            }
            else {
              DEBUG_PRINT("Waiting for " + String(21 - gps_time_updated) + " more valid GPS reads to set clock");
            }

          }
        }
      }


    }

  }




  //////////////////////////////////////////////////////////////////////////////

  // Accesory Shutoff
  if ((gps_time_updated >= 30 && Move_Complete) || ((currentMillis - ACC_LAST >= ACC_timout_ms ) && Move_Complete)) {
    DEBUG_PRINT("Accesory Shutdown");
    gps_time_updated = 0;
    Move_Complete = 0;
    UploadTS();
    //    digitalWrite(BL, LOW);
    digitalWrite(ACC_EN, LOW);
    // DEEP SLEEP
    //    deep_sleep();
    tt = WiFi.localIP().toString();
    if (tt == "(IP unset)" ) {
      LCD_LOCK_LAST = millis();
      // lcd.setCursor(0, 0);
      // lcd.println("RECONNECTING!");

      // Connect WiFi | If you added multiple WiFi access points in the config pages. Connect to the strongest AP from the list.
      // IAS.WiFiConnect();
      ESP. restart();

    }
  }

  if ((currentMillis - Time_NPT_LAST >= Time_NPT_ms) )
  { Time_NPT_LAST = currentMillis;
    UpdateNPTTime();



  }
  ////////////////////////// DATA UPLOAD ////////////////////////////////////
  if (((currentMillis - DATA_Upload_LAST >= DATA_Upload_ms) || DFirst) && !is_active)
  { DATA_Upload_LAST = currentMillis;
    tt = WiFi.localIP().toString();
    if (tt.indexOf(".") > 0) {
      FirmwareUpdate();
      ////if ( DFirst == 0) {
      UploadTS();
      // }
      DFirst = 0;
    }



    if ((DSE_TV == 1) && !is_active) {
      UploadTS();
      digitalWrite(ACC_EN, LOW);
      // DEEP SLEEP
      //  deep_sleep();



    }






    for (int i = numReadings - 1; i > 0; i--)
    {
      readings_V[i] = readings_V[i - 1];
    }
    //  Serial.println("Battery level" + String(BAT_LEV));
    readings_V[0] = BAT_LEV;

    //  deep_sleep();

  }
  //////////////////////////////////////////////////////////////////////////////


}
