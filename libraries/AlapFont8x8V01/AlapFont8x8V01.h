#ifndef AlapFont8x8V01_h
#define AlapFont8x8V01_h
/* -- font class for LED matrix
   -- Library was created: 2016.11.19 01:38:02.477
   -- Zoltan Sari
*/

#ifdef __AVR__
 #include <avr/io.h>
 #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#else
 #define PROGMEM
#endif


#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "ZMaxLEDFont.h"

class AlapFont8x8V01 : public ZMaxLEDFontBase
{
 public:
   AlapFont8x8V01();

 protected:
    static const uint16_t v_charInfoVector[];
    static const uint8_t v_charTable[];

};
#endif

