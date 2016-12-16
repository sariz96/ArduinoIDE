/*

 Udp NTP Client

 Get the time from a Network Time Protocol (NTP) time server
 Demonstrates use of UDP sendPacket and ReceivePacket
 For more on NTP time servers and the messages needed to communicate with them,
 see http://en.wikipedia.org/wiki/Network_Time_Protocol

 created 4 Sep 2010
 by Michael Margolis
 modified 9 Apr 2012
 by Tom Igoe

 This code is in the public domain.
 */

#define DEBUG_NTPClient 1
 
#include <NTPClient.h>
#include <ESP8266WiFi.h>
/*
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
*/
#include <WiFiUdp.h>

#include <ZDateTime.h>
#include <ZDateTimeStrings.h>

#include "ZWiFiConfig.h"
#include "ZWireCommands.h"

/* --- WiFi beállítások -- */
/* -- 
const char ZWiFiConfig_ssid_01[],
const char ZWiFiConfig_password_01[]

const char ZWiFiConfig_ssid_AP_Prefix[]
const char ZWiFiConfig_AP_password_01[]
-- */

int iWifiStatus = WL_IDLE_STATUS;
WiFiMode iWifiMode = WIFI_AP;
unsigned long ulLegutobbiIdo = 0;
unsigned long ulLegutobbiIdoDelay = 60000;

// A UDP instance to let us send and receive packets over UDP
WiFiUDP ntpUDP;
// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
// NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 600000);

WiFiServer server(80);

IPAddress local_IP(192,168,42,10);
IPAddress gateway(192,168,42,1);
IPAddress subnet(255,255,255,0);

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  // check for the presence of the shield:
/*  WiFi.config(local_ip, dns_server, gateway); */

 if (iWifiMode == WIFI_STA)
 {
   Serial.println("Mode:WIFI_STA");
   WiFi.mode(WIFI_STA);
  //WiFi.mode(WIFI_AP_STA);

    WiFi.begin(ZWiFiConfig_ssid_01, ZWiFiConfig_password_01);
    delay(100);
    iWifiStatus = WiFi.status();
    Serial.print("iWifiStatus:");
    Serial.println(iWifiStatus);
  
    // attempt to connect to Wifi network:
    while (iWifiStatus != WL_CONNECTED)
    {
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ZWiFiConfig_ssid_01);
      iWifiStatus = WiFi.status();
      Serial.print("iWifiStatus:");
      Serial.println(iWifiStatus);
  
      // wait 1 seconds for connection:
      delay(1000);
    }
 }
 else if (iWifiMode == WIFI_AP)
 {
   WiFi.softAPdisconnect();
  
   Serial.println("Mode:WIFI_AP");
   WiFi.mode(WIFI_AP);

   Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
   
   uint8_t mac[WL_MAC_ADDR_LENGTH];
   WiFi.softAPmacAddress(mac);
   String macID = String(mac[WL_MAC_ADDR_LENGTH - 3], HEX) +
                  String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                  String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
   macID.toUpperCase();
   
   Serial.print("macID: ");
   Serial.print(macID);
   
   String AP_NameString = String(ZWiFiConfig_ssid_AP_Prefix) + macID;

   Serial.print(", AP_NameString: ");
   Serial.print(AP_NameString);
   Serial.println();
   
   char AP_NameChar[AP_NameString.length() + 1];
   memset(AP_NameChar, 0, AP_NameString.length() + 1);

   for (int i=0; i<AP_NameString.length(); i++)
     AP_NameChar[i] = AP_NameString.charAt(i);

   Serial.println(WiFi.softAP(AP_NameChar, ZWiFiConfig_AP_password_01) ? "Ready" : "Failed!");
   //Serial.println(WiFi.softAP(AP_NameChar) ? "Ready" : "Failed!");
   delay(100);
   Serial.println(WiFi.softAPIP());
 }

  Serial.println("printDiag:");
  WiFi.printDiag(Serial);

  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  timeClient.begin();

  server.begin();
}

void loop() 
{
  LoopTimer();

// Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<FONT face=\"Arial\" size=\"6\"><BR>";
  s += "AP hello.<BR>";
  s += "<button type=\"submit\" form action=\"/HELLO\">Click here </button>";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");      

  delay(10);
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void digitalClockDisplay()
{
  Serial.print("ZDateTime: ");
  // digital clock display of current date and time
  Serial.print(ZDateTime.Hour,DEC);
  printDigits(ZDateTime.Minute);
  printDigits(ZDateTime.Second);
  
  Serial.print(" ");
  Serial.print(ZDateTimeStrings.dayStr(ZDateTime.DayofWeek));
  Serial.print(" / ");
  Serial.print(ZDateTime.GetYear());
  Serial.print(" ");
  Serial.print(ZDateTimeStrings.monthStr(ZDateTime.Month));
  Serial.print(" ");
  Serial.print(ZDateTime.Day,DEC);
  Serial.println("");
}

void printDigits(byte digits)
{
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits,DEC);
}

void LoopTimer()
{
  if((ulLegutobbiIdo == 0) ||
    ((millis() - ulLegutobbiIdo) >= ulLegutobbiIdoDelay))
    {      
      ulLegutobbiIdo = millis();

      unsigned long tmSync = 0;
    
      // wait to see if a reply is available
      if(timeClient.update())
      {
        tmSync = timeClient.getEpochTime();
        ZDateTime.sync(tmSync);
      }
    
      Serial.print("Day: ");
      Serial.print(timeClient.getDay());
      Serial.print(", Time: ");
      Serial.print(timeClient.getFormattedTime());
      Serial.print(", EpochTime: ");
      Serial.print(timeClient.getEpochTime(), HEX);
      Serial.println("");
    
      ZDateTime.available(); //refresh the Date and time properties
      digitalClockDisplay();   // update digital clock
      Serial.println("");
      
    }  
}

