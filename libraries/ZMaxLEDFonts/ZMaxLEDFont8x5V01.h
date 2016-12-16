#ifndef ZMaxLEDFont8x5V01_h
#define ZMaxLEDFont8x5V01_h

/* -- Zoltan Sari
   -- Base font class for LED matrix
   -- Library was created: 2016.11.07.
   --
 *    Thaks for that source:   
 *    LedControl.h - A library for controling Leds with a MAX7219/MAX7221
 *    Copyright (c) 2007 Eberhard Fahle
 *
	-- 2016.11.07.
	-- It has been created.
	
 -- */
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


class ZMaxLEDFont8x5V01 : public ZMaxLEDFontBase
{
  public:
        /* 
         * Create a new font
         * Params :
         */
        ZMaxLEDFont8x5V01();

  protected:
    static const uint16_t v_charInfoVector[];
    static const uint8_t v_charTable[];

};


#endif
