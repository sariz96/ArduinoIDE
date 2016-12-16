/*
  ZDateTimeStrings.h - Arduino library for day and month strings
  Copyright (c) Michael Margolis.  All right reserved.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*/

#ifndef ZDateTimeStrings_h
#define ZDateTimeStrings_h

#ifdef __AVR__
extern "C" {
 #include <avr/io.h>
 #include <avr/pgmspace.h>
} 
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

//#include <inttypes.h>
#include <ZDateTime.h> 

#define dt_MAX_STRING_LEN 30 // length of longest string (excluding terminating null)

class ZDateTimeStringsClass
{
private:
	char buffer[dt_MAX_STRING_LEN+1];
public:
	ZDateTimeStringsClass();
	char* monthStr(byte month);
	char* dayStr(byte day);
};

extern ZDateTimeStringsClass ZDateTimeStrings;  // make an instance for the user

#endif /* ZDateTimeString_h */

