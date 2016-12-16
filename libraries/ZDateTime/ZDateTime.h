/*
  ZDateTime.h - Arduino library for date and time functions
  Copyright (c) Michael Margolis.  All right reserved.


  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*/

/* ---
  2016.11.29. Conflict time_t on ESP8266 --> I have changed the name... dTtime_t
-- */

#ifndef ZDateTime_h
#define ZDateTime_h

#ifdef __AVR__
extern "C" {
 #include <avr/io.h>
 #include <avr/pgmspace.h>
} 
#elif defined(ESP8266)
 #include <pgmspace.h>
 typedef uint8_t boolean;
#else
 #define PROGMEM
#endif

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

//#include <inttypes.h>
//#include <wiring.h> // next two typedefs replace <wiring.h> here (fixed for rel 0012)
typedef uint8_t byte;  

typedef unsigned long dTtime_t;

/*==============================================================================*/
/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
#define DAYS_PER_WEEK (7L)
#define SECS_PER_WEEK (SECS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR (SECS_PER_WEEK * 52L)
#define SECS_YR_2000  (946681200UL)
 
/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define dayOfWeek(_time_)  (( _time_ / SECS_PER_DAY + 4)  % DAYS_PER_WEEK) // 0 = Sunday
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)  // this is number of days since Jan 1 1970
#define elapsedSecsToday(_time_)  (_time_ % SECS_PER_DAY)   // the number of seconds since last midnight 
#define previousMidnight(_time_) (( _time_ / SECS_PER_DAY) * SECS_PER_DAY)  // time at the start of the given day
#define nextMidnight(_time_) ( previousMidnight(_time_)  + SECS_PER_DAY ) // time at the end of the given day 
#define elapsedSecsThisWeek(_time_)  (elapsedSecsToday(_time_) +  (dayOfWeek(_time_) * SECS_PER_DAY) )   

// todo add date math macros
/*============================================================================*/

typedef enum {
	  dtSunday, dtMonday, dtTuesday, dtWednesday, dtThursday, dtFriday, dtSaturday
} dtDays_t;

typedef enum {dtStatusNotSet, dtStatusSet, dtStatusSync
}  dtStatus_t ;

class ZDateTimeClass
{
private:
	dTtime_t sysTime;  // this is the internal time counter as seconds since midnight Jan 1 1970 (aka unix time)  
	unsigned long prevMillis;
	void setTime(dTtime_t time);
public:
	ZDateTimeClass();
	void sync(dTtime_t time); // set internal time to the given value 
	dTtime_t now(); // return the current time as seconds since Jan1 1970
	boolean available();  // returns false if not set, else refreshes the Date and Time properties and returns true
	dtStatus_t status;
	byte Hour;
	byte Minute;
	byte Second;
	byte Day;
	byte DayofWeek; // Sunday is day 0 
	byte Month; // Jan is month 0
	byte Year;  // the Year minus 1900 
	
	int GetYear();
	// functions to convert to and from time components (hrs, secs, days, years etc) to dTtime_t  
	void localTime(dTtime_t *timep,byte *psec,byte *pmin,byte *phour,byte *pday,byte *pwday,byte *pmonth,byte *pyear); // extracts time components from dTtime_t
	dTtime_t makeTime(byte sec, byte min, byte hour, byte day, byte month, int year ); // returns dTtime_t from components
};

extern ZDateTimeClass ZDateTime;  // make an instance for the user

// Timespan which can represent changes in time with seconds accuracy.
// It has come from RTClib.h : class TimeSpan
class ZTimeSpan {
public:
    ZTimeSpan (dTtime_t seconds = 0);
    ZTimeSpan (int16_t days, int8_t hours, int8_t minutes, int8_t seconds);
    ZTimeSpan (const ZTimeSpan& copy);
    int16_t days() const         { return _seconds / 86400L; }
    int8_t  hours() const        { return _seconds / 3600 % 24; }
    int8_t  minutes() const      { return _seconds / 60 % 60; }
    int8_t  seconds() const      { return _seconds % 60; }
    dTtime_t totalseconds() const { return _seconds; }

    ZTimeSpan operator+(const ZTimeSpan& right);
    ZTimeSpan operator-(const ZTimeSpan& right);

protected:
    dTtime_t _seconds;
};

#endif /* ZDateTime_h */

