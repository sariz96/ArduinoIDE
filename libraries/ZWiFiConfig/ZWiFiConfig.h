#ifndef ZWiFiConfig_h
#define ZWiFiConfig_h

/* -- Zoltan Sari
   -- WiFi Config
   -- Library was created: 2016.11.30.
   --
	
 -- */


 /* --- WiFi beállítások -- */
#define ZWIFICONFIG_CNT 3
 
/* --
 [0]: Választott üzemmód és beállított érték
  - 0: AP kiválasztáshoz
  - 1..3: tárolt változatok ZWiFiConfig_ssid[] / ZWiFiConfig_password[]
  - 16: EEPROM felhasználói beállítás(1)
  - 32: Automatikus - próbálkozik 1-3 értékekkel, ha sikertelen 0 lesz
-- */ 
#define ZWIFICONFIG_CONNMODE_AP ((uint8_t)0)
#define ZWIFICONFIG_CONNMODE_PRE1 ((uint8_t)1)
#define ZWIFICONFIG_CONNMODE_PRE2 ((uint8_t)2)
#define ZWIFICONFIG_CONNMODE_PRE3 ((uint8_t)3)
#define ZWIFICONFIG_CONNMODE_EEPROM1 ((uint8_t)16)
#define ZWIFICONFIG_CONNMODE_AUTO ((uint8_t)32)
#define ZWIFICONFIG_CONNMODE_NOTDEFINED ((uint8_t)255)
 
extern const char ZWiFiConfig_ssid_01[];
extern const char ZWiFiConfig_password_01[];

extern const char ZWiFiConfig_ssid_02[];
extern const char ZWiFiConfig_password_02[];

extern const char ZWiFiConfig_ssid_03[];
extern const char ZWiFiConfig_password_03[];

extern const char *ZWiFiConfig_ssid[];
extern const char *ZWiFiConfig_password[];

 /* --- AP beállítások -- */
extern const char ZWiFiConfig_ssid_AP_Prefix[];
extern const char ZWiFiConfig_AP_password_01[];

#endif
