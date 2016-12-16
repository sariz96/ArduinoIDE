//We always have to include the library

/* --
 WiFi kapcsolat beállítási lehetőség EEPROM mentéssel

 -- Előre beállított SSID/pwd értékek (3 db)
 --

 #include "ZWiFiConfig.h"
 #include <EEPROM.h>
 -- EEPROM használata: --
 [0]: Választott üzemmód és beállított érték
  - 0: AP kiválasztáshoz
  - 1..3: tárolt változatok ZWiFiConfig_ssid[] / ZWiFiConfig_password[]
  - 16: EEPROM felhasználói beállítás(1)
 [1..32]: felhasználói SSID (32 byte),
 [33..95]: felhasználói jelszó (63 byte)
 *  
 -- */

#define MY_MAXIMUM_NUMBER_OF_DEVICES 24
#define MY_MAXIMUM_NUMBER_OF_VERTICAL_NODES 2
#define MY_MAXIMUM_NUMBER_OF_VIRTUAL_DEVICES 248
//#define SERIAL_DEBUG_ON 1

#include "ZMaxLEDFont.h"
#include "AlapFont8x8V01.h"
#include "ZMaxLEDMatrix.h"

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
/* #include <ESP8266mDNS.h> */
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <ZDateTime.h>
#include <ZDateTimeStrings.h>
#include <Wire.h> 

#include "FS.h"
#include "ZWiFiConfig.h"
#include "ZWireCommands.h"
#include "ZScheduler.h"

/* --- WiFi beállítások -- */
/* -- 
#define ZWIFICONFIG_CNT 3

extern const char *ZWiFiConfig_ssid[];
extern const char *ZWiFiConfig_password[];

--- AP beállítások --
extern const char ZWiFiConfig_ssid_AP_Prefix[];
extern const char ZWiFiConfig_AP_password_01[];

-- */

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 08 is connected to the DataIn 
 pin 10 is connected to the CLK 
 pin 9 is connected to LOAD 
 We have only a single MAX72XX.
 */

/* -------------
 *  Csak 8x8 matrix modulok, de tobb darab *
   ------------- */
/*
 * NODEMCU: Pin D8 to DIN, D7 to Clk, D6 to LOAD
 * 
 */
#define LED_PIN_DATA_8X8MATRIX D8
#define LED_PIN_CS_8X8MATRIX D6
#define LED_PIN_CLK_8X8MATRIX D7
#define LED_DEVICE_CNT_8X8MATRIX 24
#define LED_NODES_HORIZONTAL 12
#define LED_NODES_VERTICAL 2
#define LED_VIRTUAL_NODES_HORIZONTAL 24
#define LED_VIRTUAL_NODES_VERTICAL 10

#define ZSETTINGS_LANG_EN 1
#define ZSETTINGS_LANG_HU 2

ZMaxLEDMatrix z_8x8Matrix = ZMaxLEDMatrix(
	LED_PIN_DATA_8X8MATRIX,
	LED_PIN_CLK_8X8MATRIX,
	LED_PIN_CS_8X8MATRIX,
	LED_DEVICE_CNT_8X8MATRIX,
	LED_NODES_HORIZONTAL,
	LED_NODES_VERTICAL,
	LED_VIRTUAL_NODES_HORIZONTAL,
	LED_VIRTUAL_NODES_VERTICAL);

const uint8_t bESPI2CAddress = 8;
const uint8_t bSlaveArduinoI2CAddress = 9;
const int iNTPUpdateIntervalMs = 2 * 3600 * 1000;  /* -- 3 óra NTP idő lekérése -- */

/* we always wait a bit between updates of the display */
unsigned long delaytime_8x8Matrix =250;
unsigned long delaytimeMillis_01 = 10;
unsigned long lastMillis_01;
unsigned long actMillis_01;
unsigned long lSzamlalo = 0;
unsigned long lSzamlaloElozo = 0;
unsigned long ulMicro;
unsigned long ulMicroDisplay;
unsigned long ulMicroDateTimeSync;
unsigned long ulMicroDisplayDelayMs = 250;
unsigned long ulMicroDateTimeSyncDelayMs = 60000;
unsigned long ulLegutobbiSensor = 0;
unsigned long ulLegutobbiSensorDelay = 30000;
unsigned long ulLegutobbiIoTSend = 0;
unsigned long ulLegutobbiIoTSendDelay = 300000;
unsigned long ulLegutobbiWifiClient = 0;
unsigned long ulLegutobbiWifiClientDelay = 1000;
unsigned long tmSync = 0;
unsigned long tmSyncArduino = 0;
unsigned long ulTmp1 = 0;
byte *pzBuffer = NULL;
byte bTmp1;
byte bTmp2;
byte bdigit;
byte bReqLEn;
char chTxtBuffer[32];
bool LED_value = false;
String sHello1;
String sHello2;
String sHelloArg;
String sSensorsLast;
int iHelloArg;
int iNyelv; /* 0: English, 1:Hungarian */
double dd1;
int iLejartPosElozoX;
int iLejartPosElozoY;
int offsetX;
int offsetY;
int i;
int i2;
int iScroll1;
int h;
int k;
int m;
double d1;
double d2;
double dDelta;
double dPI;
double fTempCelsiusDallas = 0.0;
double fTempCelsiusDHT = 0.0;
double fHuminityDHT = 0.0;
float fTmp1;
bool bSensorDateValid = false;

const char* csServerTS = "api.thingspeak.com";
String csApikey = "IXHFPJJ8YW027YJB";

int iWifiStatus = WL_IDLE_STATUS;
WiFiMode iWifiMode = WIFI_AP;
uint8_t bWifiConnMode = ZWIFICONFIG_CONNMODE_AUTO;
String sUserSSID;
String sUserPwd;

String sTmp01;
String sTmp02;
String webPageSourceZ0 = "";
String webPageSourceZ1 = "";
String webPageSourceZ2 = "";
String webPageSourceZ3 = "";
bool blInternetConnectionActivE = false;

ZDateTimeClass zdtTmp01;

AlapFont8x8V01 font01 = AlapFont8x8V01();

//WiFiServer server(80);
ESP8266WebServer server(80);

WiFiClient clientTS;

/* MDNSResponder mdns; */
// A UDP instance to let us send and receive packets over UDP
WiFiUDP ntpUDP;
// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
// NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, iNTPUpdateIntervalMs);

ZWireCommands zwire;

void setup() 
{
 InitData();
  
/* -- Működött: 2016.11.21. --- */
/* -- NODEMCU -- */
/* -- !!! SDA: D2, SCL: D1 -- */
 Wire.pins(D2,D1);
 delay(1);
 Wire.begin();
 Wire.setClockStretchLimit(15000);    // in µs

 EEPROM.begin(512);
 delay(10);
 
#ifdef SERIAL_DEBUG_ON
  Serial.begin(115200);
  Serial.println("Start...");
#endif  // SERIAL_DEBUG_ON
  delay(10);

#ifdef SERIAL_DEBUG_ON
  Serial.println("Reading EEPROM settings");
#endif  // SERIAL_DEBUG_ON
  ReadEEPROMConfig();
  

  ulMicroDisplay = 0;
  ulMicroDateTimeSync = 0;
  int iCnt = z_8x8Matrix.GetDeviceCount();
#ifdef SERIAL_DEBUG_ON
  Serial.print("getDeviceCount:");
  Serial.println(iCnt);
  Serial.print("GetVirtualNodesHorizontal():");
  Serial.println(z_8x8Matrix.GetVirtualNodesHorizontal());
  Serial.print("GetVirtualNodesVertical():");
  Serial.println(z_8x8Matrix.GetVirtualNodesVertical());
  Serial.print("GetPixelDataLen():");
  Serial.println(z_8x8Matrix.GetPixelDataLen());
#endif  // SERIAL_DEBUG_ON
  delay(1);
  /*
  The MAX72XX is in power-saving mode on startup,
  we have to do a wakeup call
  */
#ifdef SERIAL_DEBUG_ON
  Serial.print("Init:");
#endif  // SERIAL_DEBUG_ON
  z_8x8Matrix.ShutdownEach(false);
  /* Set the brightness to a medium values */
  z_8x8Matrix.SetIntensityEach(2);
  z_8x8Matrix.DisplayTestEach(0);
  delay(5);
  z_8x8Matrix.SetDisplayOffset(0,0);  
  z_8x8Matrix.ClearDisplayEach();
  z_8x8Matrix.Display();


#ifdef SERIAL_DEBUG_ON
  Serial.println("  end.");  
#endif  // SERIAL_DEBUG_ON

 lastMillis_01 = millis();
 lSzamlalo = 10000;
 lSzamlaloElozo = 0;
 iLejartPosElozoX = 0;
 iLejartPosElozoY = 0;
 iScroll1 = -1;
 iNyelv = ZSETTINGS_LANG_HU;

 sHelloArg = "Hello...";
 iHelloArg = ZSETTINGS_LANG_HU;
 z_8x8Matrix.PrintStr(0, 8, sHelloArg, 12, &font01);
 z_8x8Matrix.Display();

 delay(2000);
/* -- get RTC date/time -- */
 ulTmp1 = GetI2CTime(false);
 if(ulTmp1 != 0)
 {
  ZDateTime.sync(ulTmp1);
  ZDateTime.available(); //refresh the Date and time properties
  sHello1 = ZTimeToString();
  z_8x8Matrix.PrintStr(0, 0, sHello1, 12, &font01);
  z_8x8Matrix.Display();  
  delay(1000);
 }

 z_8x8Matrix.ClearDisplayEach();
  // Connect to WiFi network
 ConnectToWiFi();
 delay(250);
 if(blInternetConnectionActivE == true)
 {
  timeClient.begin();
 }
 delay(4000);
 z_8x8Matrix.ClearDisplayEach();
 z_8x8Matrix.Display();
}

void loop() 
{ 
  loopDateTime();
  loopWiFi();
  loopLEDDisplay();
  LoopSensors();
  LoopIoTSend();
  LoopWiFiClientStop();
  delay(20);
}

void loopDateTime()
{
  // wait to see if a reply is available
  if((blInternetConnectionActivE == true) &&
      timeClient.update()
     )
  {
    if((ulMicroDateTimeSync == 0) ||
      ((millis() - ulMicroDateTimeSync) >= ulMicroDateTimeSyncDelayMs))
      {
        ulMicroDateTimeSync = millis();
        tmSync = timeClient.getEpochTime();
        ZDateTime.sync(tmSync);
        SendI2CTime(tmSync);
      }    
  }
}

void loopLEDDisplay()
{
  static char *ptrHello02 = "T";
  
  if((ulMicroDisplay == 0) ||
     ((millis() - ulMicroDisplay) >= ulMicroDisplayDelayMs))
     {
      int iCnt = z_8x8Matrix.GetDeviceCount();
      int iNodesH = z_8x8Matrix.GetDisplayNodesHorizontal();
      int iNodesV = z_8x8Matrix.GetDisplayNodesVertical();
      
      /*-- refresh display -- */
       ulMicroDisplay = millis();
       ZDateTime.available(); //refresh the Date and time properties
      
      if((lSzamlalo > 0) && (lSzamlaloElozo <= 0))
      {
         z_8x8Matrix.SetIntensityEach(2);
      }
      if(lSzamlalo > 0)
      {
        z_8x8Matrix.ClearDisplayEach();
        /*
        z_8x8Matrix.ClearDisplay(0);
        z_8x8Matrix.ClearDisplay(1);
        z_8x8Matrix.ClearDisplay(2);
        z_8x8Matrix.ClearDisplay(3);
        z_8x8Matrix.ClearDisplay(4);
        z_8x8Matrix.ClearDisplay(5);
        */
        /*
         * 
         m = z_8x8Matrix.PrintTxt(0, 0, ptrHello02, 48, &font01);
         */
        /*
        z_8x8Matrix.ClearDisplay(24);
        z_8x8Matrix.ClearDisplay(25);
        z_8x8Matrix.ClearDisplay(26);
        z_8x8Matrix.ClearDisplay(27);
        z_8x8Matrix.ClearDisplay(28);
        z_8x8Matrix.ClearDisplay(29);
        z_8x8Matrix.ClearDisplay(30);
        */
        sHello1 = ZTimeToString();
        z_8x8Matrix.PrintStr(8, 0, sHello1, 9, &font01);

        sHello1 = ZDateToString();
        i2 = 34 - (int)((lSzamlalo / 10) % 30);
        z_8x8Matrix.PrintStr(i2, 8, sHello1, 12, &font01);

        sHello1 = String(lSzamlalo,DEC);
        z_8x8Matrix.PrintStr(60, 0, sHello1, 12, &font01);

        z_8x8Matrix.Display();
        lSzamlalo--;
      }
      
      if(lSzamlalo <= 0)
      {
        actMillis_01 = millis();
        k = (int)((actMillis_01 / 1000) & 0x01F);
    
        if(iLejartPosElozoX != k)
        {
         z_8x8Matrix.SetIntensityEach(0);
         iLejartPosElozoX = k;
         iLejartPosElozoY = 0;
         sHello1 = ZTimeToString();

         z_8x8Matrix.ClearDisplayEach();                  
         z_8x8Matrix.PrintStr(((40 - k)/4) + 2, 8, sHello1, 12, &font01);
         z_8x8Matrix.PrintStr((k / 3), 0, sHelloArg, 48, &font01);
         z_8x8Matrix.Display();
        }        
      }
    
      lSzamlaloElozo = lSzamlalo;         
     }  
}

String IpToString(IPAddress ip)
{
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

void SetWebPageStr(int argLanguage)
{
  if(argLanguage == ZSETTINGS_LANG_EN)
  {
//    webPageSourceZ0 = "<BR><FONT SIZE=6><STRONG>Display state: </STRONG></FONT></div>";
    webPageSourceZ0 = F("");
    webPageSourceZ0 += F("<FONT face=\"Arial\" size=\"6\">");
    webPageSourceZ0 += F("<BR>");
    webPageSourceZ0 += sSensorsLast;
    webPageSourceZ0 += F("<BR>");
    if(blInternetConnectionActivE == true)
    {
//      webPageSourceZ0 += F("<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/200088/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&max=85.0&min=-10.0&results=120&title=DHT+Temp+%5BC%5D&type=line\"></iframe>");
      webPageSourceZ0 += F("<img width=\"125\" height=\"125\" src=\"http://lorempixel.com/150/150/nightlife/\" alt=\"LED Matrix Display\">");
    }
    webPageSourceZ0 += F("<TABLE BORDER=3 BGCOLOR=white CELLPADDING=2>");
    webPageSourceZ0 += F("<CAPTION>Functions</CAPTION><TR><TH>Command</TH><TH>Description</TH><TH>Command</TH><TH>Description</TH></TR>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/INFO\">Click here </a></TD><TD BGCOLOR=lightgreen> Network info </TD>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/WIFICONFIG\">Click here </a></TD><TD BGCOLOR=lightgreen> WiFi Config </TD>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/KEZDD\">Click here </a></TD><TD BGCOLOR=lightgreen> Switch on. </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/ALLJ\">Click here </a></TD><TD BGCOLOR=yellow> Switch off. </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/SZAMOL\">Click here </a></TD><TD BGCOLOR=green> Query </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/SZAMOL?ERTEK=1000\">Click here </a></TD><TD BGCOLOR=green> 10.00 </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/SZAMOL?ERTEK=10000\">Click here </a></TD><TD BGCOLOR=green> 100.00 </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/HELLO\">Click here </a></TD><TD BGCOLOR=brown> --Default-- </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/HELLO?ERTEK=Let's go!\">Click here </a></TD><TD BGCOLOR=brown> Let's go! </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/HELLO?ERTEK=Awesome\">Click here </a></TD><TD BGCOLOR=brown> Awesome </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/I2CSCANNER\">Click here </a></TD><TD BGCOLOR=white> I2C Scanner </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/LANGUAGE?ERTEK=2\">Click here </a></TD><TD BGCOLOR=white> Hungarian </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CRTC\">Click here </a></TD><TD BGCOLOR=orange> RTC </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CTEST4\">Click here </a></TD><TD BGCOLOR=orange> Test I2C 4 </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CTEST8\">Click here </a></TD><TD BGCOLOR=orange> Test I2C 8 </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CTEST16\">Click here </a></TD><TD BGCOLOR=orange> Test I2C 16 </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CSENDNVRAM\">Click here </a></TD><TD BGCOLOR=orange> Set RTC NVRAM </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CGETNVRAM\">Click here </a></TD><TD BGCOLOR=orange> Get RTC NVRAM </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CGETSENSORS\">Click here </a></TD><TD BGCOLOR=orange> Get sensors </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/SPIFFSDIR\">Click here </a></TD><TD BGCOLOR=orange> List SPIFFS </TD></TR><br>");

    webPageSourceZ0 += F("<TR><TD ><form action=\"/I2CRC\">");
    webPageSourceZ0 += F("<input type=\"radio\" name=\"KOD\" value=\"a\"> LEDs off <br>");
    webPageSourceZ0 += F("<input type=\"radio\" name=\"KOD\" value=\"A\"> LEDs on <br>");
    webPageSourceZ0 += F("<input type=\"radio\" name=\"KOD\" value=\"b\"> 1. lamp off <br>");
    webPageSourceZ0 += F("<input type=\"radio\" name=\"KOD\" value=\"B\"> 1. lamp on <br>");
    webPageSourceZ0 += F("<input type=\"radio\" name=\"KOD\" value=\"c\"> 2. lamp off <br>");
    webPageSourceZ0 += F("<input type=\"radio\" name=\"KOD\" value=\"C\"> 2. lamp on <br>");
    webPageSourceZ0 += F("<input type=\"radio\" name=\"KOD\" value=\"d\"> 3. lamp off <br>");
    webPageSourceZ0 += F("<input type=\"radio\" name=\"KOD\" value=\"D\"> 3. lamp on <br>");
    webPageSourceZ0 += F("<input type=\"radio\" name=\"KOD\" value=\"x\"> Each lamp off <br>");
    webPageSourceZ0 += F("<br><input type=\"submit\" </form> </TD></TR><br>");
    
    webPageSourceZ0 += F("<TR><TD ><a href=\"/I2CRC?KOD=a\">Click here </a></TD><TD BGCOLOR=yellow> LEDs off </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/I2CRC?KOD=A\">Click here </a></TD><TD BGCOLOR=yellow> LEDs on </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/I2CRC?KOD=b\">Click here </a></TD><TD BGCOLOR=yellow> 1. lamp off </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/I2CRC?KOD=B\">Click here </a></TD><TD BGCOLOR=yellow> 1. lamp on </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/I2CRC?KOD=c\">Click here </a></TD><TD BGCOLOR=yellow> 2. lamp off </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/I2CRC?KOD=C\">Click here </a></TD><TD BGCOLOR=yellow> 2. lamp on </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/I2CRC?KOD=d\">Click here </a></TD><TD BGCOLOR=yellow> 3. lamp off </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/I2CRC?KOD=D\">Click here </a></TD><TD BGCOLOR=yellow> 3. lamp on </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><button type=\"submit\" form action=\"/I2CRC?KOD=x\">Click here </button></TD><TD BGCOLOR=gray> Each off </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><form action=\"/HELLO\" ERTEK:<br><input type=\"text\" name=\"ERTEK\" value=\"Awesome\"><br><input type=\"submit\" value=\"Go\"> </form></TD></TR><br>");
    webPageSourceZ0 += F("</TABLE><BR></FONT>");
  }
  else if (argLanguage == ZSETTINGS_LANG_HU)
  {
//    webPageSourceZ0 = "<BR><FONT SIZE=6><STRONG>A kijelzo allapota: </STRONG></FONT></div>";
    webPageSourceZ0 = F("<CENTER>");
    webPageSourceZ0 += F("<FONT face=\"Arial\" size=\"6\">");
    webPageSourceZ0 += F("<BR>");
    webPageSourceZ0 += sSensorsLast;
    webPageSourceZ0 += F("<BR>");
    if(blInternetConnectionActivE == true)
    {
//      webPageSourceZ0 += F("<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/200088/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&max=85.0&min=-10.0&results=120&title=DHT+Temp+%5BC%5D&type=line\"></iframe>");
      webPageSourceZ0 += F("<img width=\"125\" height=\"125\" src=\"http://lorempixel.com/150/150/nightlife/\" alt=\"LED Matrix Display\">");
    }
    webPageSourceZ0 += F("<TABLE BORDER=3 BGCOLOR=white CELLPADDING=2>");
    webPageSourceZ0 += F("<CAPTION>Funkciok</CAPTION><TR><TH>Parancs</TH><TH>Mukodes</TH><TH>Parancs</TH><TH>Mukodes</TH></TR>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/INFO\">Kattints ide </a></TD><TD BGCOLOR=lightgreen> Network info </TD>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/WIFICONFIG\">Kattints ide </a></TD><TD BGCOLOR=lightgreen> WiFi Config </TD>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/KEZDD\">Kattints ide </a></TD><TD BGCOLOR=lightgreen> bekapcsolashoz </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/ALLJ\">Kattints ide </a></TD><TD BGCOLOR=yellow> kikapcsolashoz </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/SZAMOL\">Kattints ide </a></TD><TD BGCOLOR=green> lekerdezeshez </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/SZAMOL?ERTEK=1000\">Kattints ide </a></TD><TD BGCOLOR=green> 10.00 </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/SZAMOL?ERTEK=10000\">Kattints ide </a></TD><TD BGCOLOR=green> 100.00 </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/HELLO\">Kattints ide </a></TD><TD BGCOLOR=brown> --Alapertek-- </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/HELLO?ERTEK=Gyerunk!\">Kattints ide </a></TD><TD BGCOLOR=brown> Gyerunk! </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/HELLO?ERTEK=Fantasztikus\">Kattints ide </a></TD><TD BGCOLOR=brown> Fantasztikus </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/I2CSCANNER\">Kattints ide </a></TD><TD BGCOLOR=white> I2C kereso </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/LANGUAGE?ERTEK=1\">Kattints ide </a></TD><TD BGCOLOR=cyan> Angol </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/I2CRC?KOD=a\">Kattints ide </a></TD><TD BGCOLOR=yellow> LEDs off </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/I2CRC?KOD=A\">Kattints ide </a></TD><TD BGCOLOR=yellow> LEDs on </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/I2CRC?KOD=b\">Kattints ide </a></TD><TD BGCOLOR=yellow> 1. lamp off </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/I2CRC?KOD=B\">Kattints ide </a></TD><TD BGCOLOR=yellow> 1. lamp on </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/I2CRC?KOD=c\">Kattints ide </a></TD><TD BGCOLOR=yellow> 2. lamp off </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/I2CRC?KOD=C\">Kattints ide </a></TD><TD BGCOLOR=yellow> 2. lamp on </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD ><a href=\"/I2CRC?KOD=d\">Kattints ide </a></TD><TD BGCOLOR=yellow> 3. lamp off </TD>");
    webPageSourceZ0 += F("<TD ><a href=\"/I2CRC?KOD=D\">Kattints ide </a></TD><TD BGCOLOR=yellow> 3. lamp on </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CRC?KOD=x\">Kattints ide </a></TD><TD BGCOLOR=gray> Each off </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CRTC\">Kattints ide </a></TD><TD BGCOLOR=orange> RTC </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CTEST4\">Kattints ide </a></TD><TD BGCOLOR=orange> Test I2C 4 </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CTEST8\">Kattints ide </a></TD><TD BGCOLOR=orange> Test I2C 8 </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CTEST16\">Kattints ide </a></TD><TD BGCOLOR=orange> Test I2C 16 </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CSENDNVRAM\">Kattints ide </a></TD><TD BGCOLOR=orange> Set RTC NVRAM </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CGETNVRAM\">Kattints ide </a></TD><TD BGCOLOR=orange> Get RTC NVRAM </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/I2CGETSENSORS\">Kattints ide </a></TD><TD BGCOLOR=orange> Get sensors </TD></TR><br>");
    webPageSourceZ0 += F("<TR><TD><a href=\"/SPIFFSDIR\">Kattints ide </a></TD><TD BGCOLOR=orange> List SPIFFS </TD></TR><br>");
    webPageSourceZ0 += F("</TABLE></FONT></CENTER>");
  } 
}

void CreateWebPageStringINFO(uint8_t argMode)
{
  
  webPageSourceZ1 = F("");
  webPageSourceZ1 += F("<FONT face=\"Arial\" size=\"6\">");
  webPageSourceZ1 += F("<DIV><STRONG>Network Info Page</STRONG></DIV>");
  webPageSourceZ1 += F("<BR>");
  webPageSourceZ1 += F("WiFi status: ");
  if(iWifiStatus == WL_IDLE_STATUS)
  {
    webPageSourceZ1 += F("WL_IDLE_STATUS");
  }
  else if(iWifiStatus == WL_CONNECTED)
  {
    webPageSourceZ1 += F("WL_CONNECTED");
  }
  else 
  {
    webPageSourceZ1 += String(iWifiStatus, DEC);
  }
  webPageSourceZ1 += F("<BR>");
  webPageSourceZ1 += F("Local IP: ");
  webPageSourceZ1 += IpToString(WiFi.localIP());
  webPageSourceZ1 += F("<BR>");
  webPageSourceZ1 += F("SoftAP IP: ");
  webPageSourceZ1 += IpToString(WiFi.softAPIP());
  webPageSourceZ1 += F("<BR>");

  webPageSourceZ1 += F("WiFi connection mode: ");
  switch(bWifiConnMode)
  {
    case ZWIFICONFIG_CONNMODE_AP:
      webPageSourceZ1 += F("AP \"");
     break;
    
    case ZWIFICONFIG_CONNMODE_PRE1:
      webPageSourceZ1 += F("Home 1 \"");
      webPageSourceZ1 += ZWiFiConfig_ssid[ZWIFICONFIG_CONNMODE_PRE1-1];
     break;
     
    case ZWIFICONFIG_CONNMODE_PRE2:
      webPageSourceZ1 += F("Home 2 \"");
      webPageSourceZ1 += ZWiFiConfig_ssid[ZWIFICONFIG_CONNMODE_PRE2-1];
     break;
     
    case ZWIFICONFIG_CONNMODE_PRE3:
      webPageSourceZ1 += F("Job developer \"");
      webPageSourceZ1 += ZWiFiConfig_ssid[ZWIFICONFIG_CONNMODE_PRE3-1];
     break;

    case ZWIFICONFIG_CONNMODE_EEPROM1:
      webPageSourceZ1 += F("User \"");
      webPageSourceZ1 += sUserSSID;
     break;

    case ZWIFICONFIG_CONNMODE_AUTO:
      webPageSourceZ1 += F("Auto selection 1/2/3/user/AP\"");
     break;

    case ZWIFICONFIG_CONNMODE_NOTDEFINED:
      webPageSourceZ1 += F("Not defined\"");
     break;

    default:
      webPageSourceZ1 += F("Not defined\"");
     break;    
  }
  webPageSourceZ1 += F("\"");
  webPageSourceZ1 += F("<BR><BR>");
  webPageSourceZ1 += F("User SSID: \"");
  webPageSourceZ1 += sUserSSID;
  webPageSourceZ1 += F("\" ");
  webPageSourceZ1 += F("<BR>");
  
  webPageSourceZ1 += F("<BR>");
  webPageSourceZ1 += F("</FONT>");
}

void CreateWebPageStringWiFiConfig(uint8_t argMode)
{
  webPageSourceZ2 = F("<form method='get' action='WIFICONFIG'>");
  webPageSourceZ2 += F("<label>Mode: </label>");
  webPageSourceZ2 += F("<input name='mode' length=3>");
  webPageSourceZ2 += F("<label>SSID: </label>");
  webPageSourceZ2 += F("<input name='ssid' length=32>");
  webPageSourceZ2 += F("<label>Pwd: </label>");
  webPageSourceZ2 += F("<input name='pass' length=63>");
  webPageSourceZ2 += F("<input type='submit'>");
  webPageSourceZ2 += F("</form>");
  webPageSourceZ1 += F("<BR>");
  webPageSourceZ1 += F("<BR><a href=\"/\"> --Home-- </a><BR>");
}

 // Connect to WiFi network
bool ConnectToWiFi()
{
  int iVarakozasSzam = 60;
  int i;
  int iV;
  long int iRssi;
  String sTmp;
  String sActssid;
  String sActpwd;

  blInternetConnectionActivE = false;
  if(bWifiConnMode == 0)
  {
    z_8x8Matrix.PrintTxt(0, 0, "AP", 48, &font01);
    z_8x8Matrix.Display();    
  }
  else if((bWifiConnMode > 0) && (bWifiConnMode <= ZWIFICONFIG_CNT))
  {
    sActssid = ZWiFiConfig_ssid[bWifiConnMode - 1];
    sActpwd = ZWiFiConfig_password[bWifiConnMode - 1];
    
    z_8x8Matrix.PrintStr(0, 0, sActssid, 48, &font01);
    z_8x8Matrix.Display();
  }
  else if(bWifiConnMode == ZWIFICONFIG_CONNMODE_EEPROM1)
  {
    sActssid = sUserSSID;
    sActpwd = sUserPwd;
    
    z_8x8Matrix.PrintStr(0, 0, sActssid, 48, &font01);
    z_8x8Matrix.Display();    
  }

  SetWebPageStr(iNyelv);
  /* WiFi.flush(); */
  /* WiFi.stop(); */
  delay(10);
  if(((bWifiConnMode > 0) && (bWifiConnMode <= ZWIFICONFIG_CNT)) ||
     (bWifiConnMode == ZWIFICONFIG_CONNMODE_EEPROM1))
  {  
    WiFi.disconnect();
    delay(10);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(sActssid.c_str(), sActpwd.c_str());
    delay(100);
    iWifiStatus = WiFi.status();
        
    for (i = 0; (iWifiStatus != WL_CONNECTED) && (i < iVarakozasSzam); i++)
    {
      z_8x8Matrix.PrintTxt(0, 8, "Kapcsolodas...", 48, &font01);
      z_8x8Matrix.Display();
    
      iWifiStatus = WiFi.status();
      
      /* iWifiStatus = WiFi.status(); */
      
      iV = (i % 4)*3;
      z_8x8Matrix.SetIntensity(12, iV);
      iV--; if(iV<0) iV = 9;
      z_8x8Matrix.SetIntensity(13, iV);
      iV--; if(iV<0) iV = 9;
      z_8x8Matrix.SetIntensity(14, iV);
      iV--; if(iV<0) iV = 9;
      z_8x8Matrix.SetIntensity(15, iV);
      iV--; if(iV<0) iV = 9;
      z_8x8Matrix.SetIntensity(16, iV);
      iV--; if(iV<0) iV = 9;
      z_8x8Matrix.SetIntensity(17, iV);
      iV--; if(iV<0) iV = 9;
      z_8x8Matrix.SetIntensity(18, iV);
      iV--; if(iV<0) iV = 9;
      z_8x8Matrix.SetIntensity(19, iV);
      delay(500);
    }
   iWifiStatus = WiFi.status();
   if(iWifiStatus == WL_CONNECTED)
   {
    blInternetConnectionActivE = true;
   }
   else
   {
    blInternetConnectionActivE = false;
    sActssid = sUserSSID;
    sActpwd = sUserPwd;
    bWifiConnMode = ZWIFICONFIG_CONNMODE_AP;
   }
  }
  if((bWifiConnMode == ZWIFICONFIG_CONNMODE_AP) ||
     (bWifiConnMode == ZWIFICONFIG_CONNMODE_AUTO))
  {
    iWifiStatus = WL_IDLE_STATUS;
    blInternetConnectionActivE = false;
    WiFi.softAPdisconnect();

#ifdef SERIAL_DEBUG_ON
    Serial.println("Mode:WIFI_AP");
#endif  // SERIAL_DEBUG_ON    
    WiFi.mode(WIFI_AP);
    
   uint8_t mac[WL_MAC_ADDR_LENGTH];
   WiFi.softAPmacAddress(mac);
   String macID = String(mac[WL_MAC_ADDR_LENGTH - 3], HEX) +
                  String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                  String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
   macID.toUpperCase();
   
#ifdef SERIAL_DEBUG_ON
   Serial.print("macID: ");
   Serial.print(macID);
#endif  // SERIAL_DEBUG_ON    
   
   String AP_NameString = String(ZWiFiConfig_ssid_AP_Prefix) + macID;

#ifdef SERIAL_DEBUG_ON
   Serial.print(", AP_NameString: ");
   Serial.print(AP_NameString);
   Serial.println();
#endif  // SERIAL_DEBUG_ON    
   
   char AP_NameChar[AP_NameString.length() + 1];
   memset(AP_NameChar, 0, AP_NameString.length() + 1);

   for (int i=0; i<AP_NameString.length(); i++)
     AP_NameChar[i] = AP_NameString.charAt(i);

   if(WiFi.softAP(AP_NameChar, ZWiFiConfig_AP_password_01))
   {
    iWifiStatus = WL_CONNECTED;
#ifdef SERIAL_DEBUG_ON
   Serial.println("Ready");
#endif  // SERIAL_DEBUG_ON        
   }
   else
   {
#ifdef SERIAL_DEBUG_ON
   Serial.println("Failed!");
#endif  // SERIAL_DEBUG_ON        
   }
   delay(100);
#ifdef SERIAL_DEBUG_ON
   Serial.println(IpToString(WiFi.softAPIP()));
#endif  // SERIAL_DEBUG_ON    
  }
  z_8x8Matrix.SetIntensityEach(1);

  
  if((iWifiStatus == WL_CONNECTED) ||
     (bWifiConnMode == ZWIFICONFIG_CONNMODE_AP) ||
     (bWifiConnMode == ZWIFICONFIG_CONNMODE_AUTO)
     )
  {
    
    z_8x8Matrix.ClearDisplayEach();
    z_8x8Matrix.PrintTxt(0, 0, "Ok.", 48, &font01);
    z_8x8Matrix.Display();
    
    // Start the server
    server.begin();

    if(bWifiConnMode == ZWIFICONFIG_CONNMODE_AP)
    {
      sTmp = "IP:";
      sTmp += IpToString(WiFi.softAPIP());
      z_8x8Matrix.PrintStr(0, 8, sTmp, 48, &font01);      
    }
    else
    {
      sTmp = "IP:";
      sTmp += IpToString(WiFi.localIP());
      z_8x8Matrix.PrintStr(0, 8, sTmp, 48, &font01);
    }

    iRssi = WiFi.RSSI();
    sTmp = "RSSI:";
    sTmp += String(iRssi);
    sTmp += F(" dBm");
    z_8x8Matrix.PrintStr(20, 0, sTmp, 48, &font01);
    
    z_8x8Matrix.Display();
/*  
    mdns.begin("zledmatrix", WiFi.localIP());
*/
    server.on("/", handle_rootWiFi);
    server.on("/INFO", handle_INFO);
    server.on("/WIFICONFIG", handle_WIFICONFIG);    
    server.on("/INDEX", handle_rootWiFi);
    server.on("/SZAMOL", handle_rootWiFi_SZAMOL);
    server.on("/KEZDD", handle_rootWiFi_KEZDD);
    server.on("/ALLJ", handle_rootWiFi_ALLJ);
    server.on("/HELLO", handle_rootWiFi_HELLO);
    server.on("/LANGUAGE", handle_rootWiFi_LANGUAGE);
    server.on("/I2CSCANNER",handle_rootWiFi_I2CSCANNER);
    server.on("/I2CRC",handle_rootWiFi_I2CRC);
    server.on("/I2CRTC",handle_rootWiFi_I2CRTC);
    server.on("/I2CTEST4",handle_rootWiFi_I2CTEST4);
    server.on("/I2CTEST8",handle_rootWiFi_I2CTEST8);
    server.on("/I2CTEST16",handle_rootWiFi_I2CTEST16);
    server.on("/I2CSENDNVRAM",handle_rootWiFi_I2CSENDNVRAM);
    server.on("/I2CGETNVRAM",handle_rootWiFi_I2CGETNVRAM);
    server.on("/I2CGETSENSORS",handle_rootWiFi_I2CGETSENSORS);
    server.on("/SPIFFSDIR",handle_rootWiFi_SPIFFSDIR);

    return true;
  }
  else if(WiFi.status() != WL_CONNECTED)
  {
    blInternetConnectionActivE = false;
    
    z_8x8Matrix.ClearDisplayEach();
    z_8x8Matrix.PrintTxt(0, 0, "Sikertelen!", 48, &font01);
    z_8x8Matrix.Display();
  }
  
  return false;
}

void handle_rootWiFi()
{
  server.send(200, "text/html", webPageSourceZ0);
  delay(2);
}

void handle_rootWiFi_SZAMOL()
{
  String state = server.arg("ERTEK");
  if( state.length() > 0)
  {
    lSzamlalo = (unsigned long)state.toInt();
  }
  
  if(lSzamlalo > 0)
  {
    dd1 = (double)lSzamlalo / 100;
    sHello2 = F("ZMaxLEDMatrix - SZAMOL ");
    sHello2 += String(dd1);
    server.send(200, "text/plain", sHello2);
    sHello2 = String(dd1);
  }
  else
  {
    server.send(200, "text/plain", "ZMaxLEDMatrix - Lejart");
  }
  delay(2);
}

void handle_rootWiFi_KEZDD()
{
  lSzamlalo = 2000;
  dd1 = (double)lSzamlalo / 100;
  sHello2 = F("ZMaxLEDMatrix - KEZDD ");
  sHello2 += String(dd1);
  server.send(200, "text/plain", sHello2);
  sHello2 = String(dd1);
  delay(2);
}

void handle_rootWiFi_HELLO()
{
  String state = server.arg("ERTEK");
  if( state.length() > 0)
  {
    sHelloArg = state;
  }
  else
  {
    sHelloArg = F("LEJART");
  }
  lSzamlalo = 0;
  
  sHello2 = F("ZMaxLEDMatrix - HELLO ");
  sHello2 += sHelloArg;
  server.send(200, "text/plain", sHello2);
  delay(2);
}

void handle_rootWiFi_LANGUAGE()
{
  String state = server.arg("ERTEK");
  if( state.length() > 0)
  {
    i2 = (int)state.toInt();

    if(((i2 == ZSETTINGS_LANG_EN) || (i2 == ZSETTINGS_LANG_HU)) &&
       (iNyelv != i2))
    {
      iNyelv = i2;
      SetWebPageStr(iNyelv);
    }
  }  
  handle_rootWiFi();
}

void handle_rootWiFi_I2CRC()
{
  String state = server.arg("KOD");
  if( state.length() > 0)
  {
    char chKod = state.charAt(0);

    SendI2CKod(chKod);
  }  
  handle_rootWiFi();
}

void handle_rootWiFi_ALLJ()
{
  if(lSzamlalo > 0)
  {
    lSzamlalo = 0;
    server.send(200, "text/plain", "ZMaxLEDMatrix - Megallt");
  }
  else
  {
    server.send(200, "text/plain", "ZMaxLEDMatrix - Lejart");
  }
  delay(2);  
}

void handle_rootWiFi_I2CSCANNER()
{
  int count = 0;

  sHello2 = F("<FONT face=\"Arial\" size=\"6\">");
  sHello2 += F("<TABLE BORDER=3 BGCOLOR=white CELLPADDING=2>");
  sHello2 += F("<CAPTION>ZMaxLEDMatrix - I2C Scanner</CAPTION><TR><TH>Address dec</TH><TH>Address hex</TH></TR>");

  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
        sHello2 += F("<TR><TD BGCOLOR=yellow>");
        sHello2 += String(i, DEC);
        sHello2 += F("</TD>");
        sHello2 += F("<TD BGCOLOR=lightgreen>0x");
        sHello2 += String(i, HEX);
        sHello2 += F("</TD></TR>");
        count++;
        
        delay (1);  // maybe unneeded?
      } // end of good response
  } // end of for loop
  sHello2 += F("</TABLE><BR>");
  if(count > 0)
  {
    sHello2 += F("\n Number of devices: ");
    sHello2 += String(count, DEC);
  }
  else
  {
    sHello2 += "\n There is no any device.";
  }
  sHello2 += F("<BR></FONT>");
  sHello2 += F("<BR><a href=\"/INDEX\"> --Home-- </a><BR>");
  
  server.send(200, "text/html", sHello2);
  sHello2 = "I2C: ";
  sHello2 += String(count, DEC);
  delay(2);
}

void handle_rootWiFi_I2CRTC()
{
  ulTmp1 = GetI2CTime(true);

  if( ulTmp1 == 0)
  {
    server.send(200, "text/plain", "I2CRTC: 0");    
  }
  else
  {
    String s1 = String("I2CRTC:");
    s1 += String(ulTmp1, HEX);
    zdtTmp01.sync((dTtime_t)ulTmp1);
    if( zdtTmp01.available())
    {
      s1 += " Date:";
      s1 += String(zdtTmp01.GetYear(),DEC);
      s1 += ".";
      bdigit = zdtTmp01.Month + 1;
      if(bdigit < 10)
      {
        s1 += "0";
      }
      s1 += String(bdigit,DEC);
      s1 += ".";
      bdigit = zdtTmp01.Day;
      if(bdigit < 10)
      {
        s1 += "0";
      }
      s1 += String(bdigit,DEC);

      s1 += " Time:";
      bdigit = zdtTmp01.Hour;
      if(bdigit < 10)
      {
        s1 += "0";
      }
      else
      {
        s1 += "";
      }
      s1 += String(bdigit,DEC);
      s1 += ":";
      bdigit = zdtTmp01.Minute;
      if(bdigit < 10)
      {
        s1 += "0";
      }
      s1 += String(bdigit,DEC);
      s1 += ":";
      bdigit = zdtTmp01.Second;
      if(bdigit < 10)
      {
        s1 += "0";
      }
      s1 += String(bdigit,DEC);      
    }
    server.send(200, "text/plain", s1);
  }
  delay(1);
}

void handle_rootWiFi_I2CTEST4()
{
  String s1 = GetI2CTest4();

  server.send(200, "text/plain", s1);
}

void handle_rootWiFi_I2CTEST8()
{
  String s1 = GetI2CTest8();

  server.send(200, "text/plain", s1);
}

void handle_rootWiFi_I2CTEST16()
{
  String s1 = GetI2CTest16();

  server.send(200, "text/plain", s1);
}

void handle_rootWiFi_I2CSENDNVRAM()
{
  ZDateTime.available(); //refresh the Date and time properties
  sHello1 = ZTimeToString();
  
  zwire.SetCommand(ZWIRECOMM_RTC_WRITE_NVRAM, 13);
  zwire.SetByte(0, 0); /* -- position -- */
  zwire.SetByte(1, 8); /* -- length -- */
  pzBuffer = zwire.GetBuffer();
  pzBuffer += 4;
  sHello1.toCharArray((char *)pzBuffer, 8);
  
  Wire.beginTransmission (bSlaveArduinoI2CAddress); // transmit to device #9
  Wire.write(zwire.GetBuffer(), 12);
  Wire.endTransmission();    // stop transmitting
  
  server.send(200, "text/plain", sHello1);
  delay(1);
}

void handle_rootWiFi_I2CGETNVRAM()
{
  sHello1 = "I2CGETNVRAM(): ";
  
  zwire.SetCommand(ZWIRECOMM_RTC_READ_NVRAM, 13);
  zwire.SetByte(0, 0); /* -- position -- */
  zwire.SetByte(1, 8); /* -- length -- */
  
  Wire.beginTransmission (bSlaveArduinoI2CAddress); // transmit to device #9
  Wire.write(zwire.GetBuffer(), 4);
  Wire.endTransmission();    // stop transmitting
  delay(10);

  zwire.SetCommand(ZWIRECOMM_RTC_GET_NVRAM, 13);  
  Wire.beginTransmission (bSlaveArduinoI2CAddress); // transmit to device #9
  Wire.write(zwire.GetBuffer(), 2);
  Wire.endTransmission();    // stop transmitting

  Wire.requestFrom((int8_t)bSlaveArduinoI2CAddress, (int8_t)8);
  zwire.ClearBuffer(9);
  bReqLEn = (uint8_t)Wire.available();
  if(bReqLEn >= 8)
  {  
    for(bTmp2 = 0; bTmp2 < bReqLEn; bTmp2++)
    {
      bTmp1 = (byte)Wire.read();
      zwire.SetClientByte(bTmp2, bTmp1);
    }
    sHello1 += BufferToString(zwire.GetBuffer(), 8);
  }

  server.send(200, "text/plain", sHello1);
  delay(1);
}

void handle_rootWiFi_I2CGETSENSORS()
{
  sHello1 = "Sensors: ";
  sSensorsLast = "";

  GetI2CSensors();
  sHello1 += sSensorsLast;
  server.send(200, "text/plain", sHello1);
  delay(1);
}

void handle_INFO()
{
  CreateWebPageStringINFO(1);
  server.send(200, "text/html", webPageSourceZ1);
  delay(2);
}

void handle_WIFICONFIG()
{
  String qmode = server.arg("mode");
  String qsid = server.arg("ssid");
  String qpass = server.arg("pass");
  if ((qsid.length() > 0) && (qpass.length() > 0))
  {
    if(qmode.length() > 0)
    {
      bWifiConnMode = (uint8_t)qmode.toInt();
    }
    sUserSSID = qsid;
    sUserPwd = qpass;    
    WriteEEPROMUserConfig();
    
    handle_rootWiFi();    
  }
  else if(qmode.length() > 0)
  {
     bWifiConnMode = (uint8_t)qmode.toInt();    
     WriteEEPROMUserConfig();
    
     handle_rootWiFi();    
  }
  else
  {
    server.send(200, "text/html", webPageSourceZ2);
  }
  delay(2);
}

/* -- list directory of SPIFFS -- */
void handle_rootWiFi_SPIFFSDIR()
{
  webPageSourceZ3 = F("--- Dir SPIFFS ---");
  webPageSourceZ3 += F("\r\n");

  bool ok = SPIFFS.begin();
  if(ok)
  {
    File f1 = SPIFFS.open("/config.txt", "r");
    if(!f1)
    {
      /* -- create file -- */
      webPageSourceZ3 += F("--- create 'config.txt' ---\r\n");
      f1 = SPIFFS.open("/config.txt", "w");
      if(f1)
      {
        f1.println("#empty#");
        f1.close();
      }
    }
    
    Dir dir = SPIFFS.openDir("/");
    while (dir.next())
    {
        webPageSourceZ3 += dir.fileName();
        webPageSourceZ3 += " / ";
        webPageSourceZ3 += dir.fileSize();
        webPageSourceZ3 += "\r\n";
    }
    webPageSourceZ3 += F("--- end of list ---\r\n");
    SPIFFS.end();
  }
  else
  {
    webPageSourceZ3 += F("**not available**\r\n");
  }

  server.send(200, "text/plain", webPageSourceZ3);
}

void GetI2CSensors()
{
  bReqLEn = SendnGetI2CRequest(bSlaveArduinoI2CAddress, ZWIRECOMM_DHTDS_LASTTXT, 30);
  if(bReqLEn > 0)
  {
    bdigit = zwire.GetClientByte(0);
    bTmp1 = zwire.GetClientTxt(0, (char *)chTxtBuffer, bdigit);
    sSensorsLast = String(chTxtBuffer);
  }
}

void GetI2CSensorsBin()
{
  bSensorDateValid = false;
  
  bReqLEn = SendnGetI2CRequest(bSlaveArduinoI2CAddress, ZWIRECOMM_DHTDS_LASTBIN, 12);
  if(bReqLEn > 0)
  {
    fTmp1 = zwire.GetClientFloat(0);
    if(fTmp1 > -999)
    {
      fHuminityDHT = (double)fTmp1;
      fTmp1 = zwire.GetClientFloat(4);
      fTempCelsiusDHT = (double)fTmp1;
      fTmp1 = zwire.GetClientFloat(8);
      fTempCelsiusDallas = (double)fTmp1;
      bSensorDateValid = true;
      sSensorsLast = DHTSensorValueStr();
    }
    else
    {
      sSensorsLast = "";
    }    
  }
}

bool loopWiFi()
{
  // Check if a client has connected
  server.handleClient();
  return true;
}

/* -- I2C egy karakter kuldese parancskent Arduino Nano szamara (9) -- */
void SendI2CKod(char chKod)
{
  zwire.SetCommand(ZWIRECOMM_RF433_SEND_CHAR1);
  zwire.SetByte(0, (uint8_t)chKod);
  
  Wire.beginTransmission (bSlaveArduinoI2CAddress); // transmit to device #9
  Wire.write(zwire.GetBuffer(), 3);
  Wire.endTransmission();    // stop transmitting
  delay(10);
}

/* -- I2C unsigned long  Epoch time kuldese parancskent Arduino Nano szamara (9) -- */
void SendI2CTime(unsigned long dtEpoch)
{
  zwire.SetCommand(ZWIRECOMM_NTP_SET);
  zwire.SetDWord(0, (uint32_t)dtEpoch);
  
  Wire.beginTransmission (bSlaveArduinoI2CAddress); // transmit to device #9
  Wire.write(zwire.GetBuffer(), 6);
  Wire.endTransmission();    // stop transmitting
  delay(10);
}

/* -- Get request to zwire -- */
int8_t SendnGetI2CRequest(int8_t argSlaveAddress, uint16_t argCommand, int8_t argLenght)
{
  bReqLEn = 0;
  zwire.SetCommand(argCommand, 2);
  Wire.beginTransmission (argSlaveAddress); // transmit to device #9
  Wire.write(zwire.GetBuffer(), 2);
  Wire.endTransmission();    // stop transmitting

  delay(2);
  Wire.requestFrom(argSlaveAddress, argLenght);
  zwire.ClearBuffer(argLenght + 1);
  bReqLEn = (uint8_t)Wire.available();
  if(bReqLEn >= argLenght)
  {  
    for(bTmp2 = 0; bTmp2 < bReqLEn; bTmp2++)
    {
      bTmp1 = (byte)Wire.read();
      zwire.SetClientByte(bTmp2, bTmp1);
    }
  }
  return bReqLEn;
}

/* -- I2C unsigned long  Epoch time kuldese parancskent Arduino Nano szamara (9) -- */
unsigned long GetI2CTime(bool argMemOnly)
{
  tmSyncArduino = 0;

  if(argMemOnly)
  {
    bReqLEn = SendnGetI2CRequest(bSlaveArduinoI2CAddress, ZWIRECOMM_NTP_GET_MEM, 4);
  }
  else
  {
    bReqLEn = SendnGetI2CRequest(bSlaveArduinoI2CAddress, ZWIRECOMM_NTP_GET_RTC, 4);
  }

  if(bReqLEn >= 4)
  {  
    tmSyncArduino = (unsigned long)zwire.GetClientDWord(0);
  }

  return tmSyncArduino;
}

String GetI2CTest4()
{
  String s1 = "GetI2CTest(): ";

  s1 += "{4}:";

  bReqLEn = SendnGetI2CRequest(bSlaveArduinoI2CAddress, ZWIRECOMM_TEST_GET_TXT04, 6);
  
  if(bReqLEn >= 6)
  { 
    bdigit = zwire.GetClientByte(0);
    s1 += "Len: ";
    s1 += String(bdigit);
    s1 += " Txt:";
    bTmp1 = zwire.GetClientTxt(0, (char *)chTxtBuffer, bdigit);
    s1 += String(chTxtBuffer);
    s1 += BufferToString(zwire.GetBuffer(), 10);
  }

  return s1;
}

String GetI2CTest8()
{
  String s1 = "GetI2CTest(): ";

  s1 += "{8}:";

  bReqLEn = SendnGetI2CRequest(bSlaveArduinoI2CAddress, ZWIRECOMM_TEST_GET_TXT08, 10);
  
  if(bReqLEn >= 10)
  { 
    bdigit = zwire.GetClientByte(0);
    s1 += "Len: ";
    s1 += String(bdigit);
    s1 += " Txt:";
    bTmp1 = zwire.GetClientTxt(0, (char *)chTxtBuffer, bdigit);
    s1 += String(chTxtBuffer);
    s1 += BufferToString(zwire.GetBuffer(), 12);
  }

  return s1;
}

String GetI2CTest16()
{
  String s1 = "GetI2CTest(): ";

  s1 += "{16}:";

  bReqLEn = SendnGetI2CRequest(bSlaveArduinoI2CAddress, ZWIRECOMM_TEST_GET_TXT16, 18);
  
  if(bReqLEn >= 18)
  { 
    bdigit = zwire.GetClientByte(0);
    s1 += "Len: ";
    s1 += String(bdigit);
    s1 += " Txt:";
    bTmp1 = zwire.GetClientTxt(0, (char *)chTxtBuffer, bdigit);
    s1 += String(chTxtBuffer);
    s1 += BufferToString(zwire.GetBuffer(), 20);
  }

  return s1;
}

String BufferToString(uint8_t *argpBuffer, uint8_t argLen)
{
  String s1 = " Buffer: ";

  if((argpBuffer != NULL) && (argLen > 0))
  {
    for(; argLen > 0; argLen--)
    {
      s1 += "0x";
      s1 += String(*argpBuffer++, HEX);
      s1 += ", ";
    }
    s1 += " ";
  }
  
  return s1;
}

/* -- ZDateTime --> String -- */
String ZDateToString()
{
  sHello2 = String(ZDateTime.GetYear(),DEC);
  sHello2 += ".";
  bdigit = ZDateTime.Month + 1;
  if(bdigit < 10)
  {
    sHello2 += "0";
  }
  sHello2 += String(bdigit,DEC);
  sHello2 += ".";
  bdigit = ZDateTime.Day;
  if(bdigit < 10)
  {
    sHello2 += "0";
  }
  sHello2 += String(bdigit,DEC);
      
  return sHello2;  
}

/* -- ZDateTime --> String -- */
String ZTimeToString()
{
  bdigit = ZDateTime.Hour;
  if(bdigit < 10)
  {
    sHello2 = "0";
  }
  else
  {
    sHello2 = "";
  }
  sHello2 += String(bdigit,DEC);
  sHello2 += ":";
  bdigit = ZDateTime.Minute;
  if(bdigit < 10)
  {
    sHello2 += "0";
  }
  sHello2 += String(bdigit,DEC);
  sHello2 += ":";
  bdigit = ZDateTime.Second;
  if(bdigit < 10)
  {
    sHello2 += "0";
  }
  sHello2 += String(bdigit,DEC);
      
  return sHello2;  
}

/* -- hőmérsékletek és páratartalom -- */
String DHTSensorValueStr()
{
  if(bSensorDateValid)
  {
    sHello2 = String(fHuminityDHT,1) + "%,";
    sHello2 += String(fTempCelsiusDHT,1) + "C,";
    sHello2 += String(fTempCelsiusDallas,1) + "C";
  }
  else
  {
    sHello2 = "<nincs>";
  }
  return sHello2;
}

void SendSensorDataTS()
{
  // use ip 184.106.153.149 or api.thingspeak.com
  if((blInternetConnectionActivE == true) &&
     clientTS.connect(csServerTS, 80))
  {
#ifdef SERIAL_DEBUG_ON
   Serial.println("SendSensorDataTS(): connected!");  
#endif  // SERIAL_DEBUG_ON
   String postStr = csApikey;
   postStr += "&field1=";
   postStr += String(fTempCelsiusDHT);
   postStr += "&field2=";
   postStr += String(fTempCelsiusDallas);
   postStr += "&field3=";
   postStr += String(fHuminityDHT);
   postStr += "\r\n\r\n";

   clientTS.print("POST /update HTTP/1.1\n");
   clientTS.print("Host: api.thingspeak.com\n");
   clientTS.print("Connection: close\n");
   clientTS.print("X-THINGSPEAKAPIKEY: " + csApikey + "\n");
   clientTS.print("Content-Type: application/x-www-form-urlencoded\n");
   clientTS.print("Content-Length: ");
   clientTS.print(postStr.length());
   clientTS.print("\n\n");
   clientTS.print(postStr);
/* -- start timer for WiFi client stop -- */
   ulLegutobbiWifiClient = millis();
#ifdef SERIAL_DEBUG_ON
   Serial.println("Send data to TS");
   Serial.println(postStr);
#endif  // SERIAL_DEBUG_ON
   
  }
}

void LoopSensors()
{
  if((ulLegutobbiSensor == 0) ||
    ((millis() - ulLegutobbiSensor) >= ulLegutobbiSensorDelay))
    {      
      ulLegutobbiSensor = millis();
      //GetI2CSensors();
      GetI2CSensorsBin();
      sHelloArg = sSensorsLast;
      SetWebPageStr(iNyelv);
    }  
}

void LoopIoTSend()
{
  if((blInternetConnectionActivE == true) &&
     (bSensorDateValid == true) &&
    (
     (ulLegutobbiIoTSend == 0) ||
     ((millis() - ulLegutobbiIoTSend) >= ulLegutobbiIoTSendDelay)
     )
    )
    {      
      ulLegutobbiIoTSend = millis();
      SendSensorDataTS();
    }  
}

void LoopWiFiClientStop()
{
  if((ulLegutobbiWifiClient != 0) &&
     ((millis() - ulLegutobbiWifiClient) >= ulLegutobbiWifiClientDelay)
     )
    {      
      ulLegutobbiWifiClient = 0;
      clientTS.stop();
    }  
}

/* --
 -- EEPROM használata: --
 [0]: Választott üzemmód és beállított érték
  - 0: AP kiválasztáshoz
  - 1..3: tárolt változatok ZWiFiConfig_ssid[] / ZWiFiConfig_password[]
  - 16: EEPROM felhasználói beállítás(1)
 [1..32]: felhasználói SSID (32 byte),
 [33..95]: felhasználói jelszó (63 byte)
 -- */
void ReadEEPROMConfig()
{
  bWifiConnMode = (uint8_t)(EEPROM.read(0));

  if(bWifiConnMode == ZWIFICONFIG_CONNMODE_AUTO)
  {
    bWifiConnMode = ZWIFICONFIG_CONNMODE_AP;
  }
  /* -- teszt -- */
  /*
  if(bWifiConnMode == ZWIFICONFIG_CONNMODE_AP)
  {
    bWifiConnMode = ZWIFICONFIG_CONNMODE_PRE1;
  }
  */
  if(bWifiConnMode == ZWIFICONFIG_CONNMODE_NOTDEFINED)
  {
#ifdef SERIAL_DEBUG_ON
    Serial.println("Config is not defined. Init default...");
#endif  // SERIAL_DEBUG_ON      
    bWifiConnMode = ZWIFICONFIG_CONNMODE_AP;
    sUserSSID = String();
    sUserPwd = String();
    WriteEEPROMUserConfig();
  }
  
#ifdef SERIAL_DEBUG_ON
      Serial.println("ReadEEPROMConfig():");
      Serial.print("bWifiConnMode: ");
      Serial.print(bWifiConnMode);
      Serial.println("");
#endif  // SERIAL_DEBUG_ON      
  ReadEEPROMUserConfig();
}

void ReadEEPROMUserConfig()
{
  sUserSSID = String();
  for (int i = 1; i < 33; ++i)
  {
    sUserSSID += char(EEPROM.read(i));
  }
  sUserPwd = String();
  for (int i = 33; i < 96; ++i)
  {
    sUserPwd += char(EEPROM.read(i));
  }
#ifdef SERIAL_DEBUG_ON
      Serial.println("ReadEEPROMUserConfig():");
      Serial.print("sUserSSID: ");
      Serial.print(sUserSSID);
      Serial.print(",sUserPwd: ");
      Serial.print(sUserPwd);
      Serial.println("");
#endif  // SERIAL_DEBUG_ON      
}

void WriteEEPROMUserConfig()
{
#ifdef SERIAL_DEBUG_ON
      Serial.println("clearing eeprom...");
#endif  // SERIAL_DEBUG_ON      
  for (int i = 0; i < 96; ++i)
  {
    EEPROM.write(i, 0);
  }

  EEPROM.write(0, bWifiConnMode);

#ifdef SERIAL_DEBUG_ON
  Serial.println("writing eeprom ssid:");
#endif  // SERIAL_DEBUG_ON      
  for (int i = 0; (i < sUserSSID.length()) && (i < 32); ++i)
    {
      EEPROM.write(i + 1, sUserSSID[i]);
    }
#ifdef SERIAL_DEBUG_ON
  Serial.println("writing eeprom pass:"); 
#endif  // SERIAL_DEBUG_ON      
  for (int i = 0; (i < sUserPwd.length()) && (i < 63); ++i)
    {
      EEPROM.write(i + 33, sUserPwd[i]);
    }    
  EEPROM.commit(); 
}

void InitData()
{
  bWifiConnMode = ZWIFICONFIG_CONNMODE_AUTO;

  blInternetConnectionActivE = false;
  iWifiMode = WIFI_AP;

  ulMicroDateTimeSyncDelayMs = 60000;
  ulLegutobbiSensor = 0;
  ulLegutobbiSensorDelay = 30000;
  ulLegutobbiIoTSend = 0;
  ulLegutobbiIoTSendDelay = 300000;
  ulLegutobbiWifiClient = 0;
  ulLegutobbiWifiClientDelay = 1000;
  tmSync = 0;
  tmSyncArduino = 0;

  sUserSSID = String();
  sUserPwd = String();

  webPageSourceZ1 = String();
  webPageSourceZ0 = String();
  webPageSourceZ2 = String();

  SetWebPageStr(ZSETTINGS_LANG_HU);
  CreateWebPageStringWiFiConfig(1);
}

