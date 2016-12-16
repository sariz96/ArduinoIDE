/*
  ZDateTime.cpp - Arduino Date and Time library
  Copyright (c) Michael Margolis.  All right reserved.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*/

/* ---
  2016.11.29. Conflict time_t on ESP8266 --> I have changed the name... dTtime_t
-- */

extern "C" {
  // AVR LibC Includes
}
//#include <string.h> // for memset
#include "ZDateTime.h"

//#include <wiring.h>
//extern unsigned long _time;

#define LEAP_YEAR(_year) ((_year%4)==0)
static  byte monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

// private methods

void ZDateTimeClass::setTime(dTtime_t time)
{
    // set the system time to the given time value (as seconds since Jan 1 1970)
    this->sysTime = time;  
	this->prevMillis = millis();
}


//******************************************************************************
//* ZDateTime Public Methods
//******************************************************************************

ZDateTimeClass::ZDateTimeClass()
{
   this->status = dtStatusNotSet;
}

dTtime_t ZDateTimeClass::now()
{
  while( millis() - prevMillis >= 1000){
    this->sysTime++;
    this->prevMillis += 1000;
  }
  return sysTime;
}

void ZDateTimeClass::sync(dTtime_t time) 
{
   setTime(time); 
   //status.isSynced = true;   // this will be set back to false if the clock resets 
   //status.isSet = true; // if this is true and isSynced is false then clock was reset using EEPROM -- TODO
   this->status = dtStatusSync;
}

boolean ZDateTimeClass::available()
{  
// refresh time components if clock is set (even if not synced), just return false if not set
   if(this->status != dtStatusNotSet) 
   { 
      this->now(); // refresh sysTime   
      this->localTime(&this->sysTime,&Second,&Minute,&Hour,&Day,&DayofWeek,&Month,&Year);
	  return true;
   }
   else
      return false;
}
void ZDateTimeClass::localTime(dTtime_t *timep,byte *psec,byte *pmin,byte *phour,byte *pday,byte *pwday,byte *pmonth,byte *pyear) {
// convert the given dTtime_t to time components
// this is a more compact version of the C library localtime function

  dTtime_t long epoch=*timep;
  byte year;
  byte month, monthLength;
  unsigned long days;
  
  *psec=epoch%60;
  epoch/=60; // now it is minutes
  *pmin=epoch%60;
  epoch/=60; // now it is hours
  *phour=epoch%24;
  epoch/=24; // now it is days
  *pwday=(epoch+4)%7;
  
  year=70;  
  days=0;
  while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= epoch) {
    year++;
  }
  *pyear=year; // *pyear is returned as years from 1900
  
  days -= LEAP_YEAR(year) ? 366 : 365;
  epoch -= days; // now it is days in this year, starting at 0
  //*pdayofyear=epoch;  // days since jan 1 this year
  
  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) {
    if (month==1) { // february
      if (LEAP_YEAR(year)) {
        monthLength=29;
      } else {
        monthLength=28;
      }
    } else {
      monthLength = monthDays[month];
    }
    
    if (epoch>=monthLength) {
      epoch-=monthLength;
    } else {
        break;
    }
  }
  *pmonth=month;  // jan is month 0
  *pday=epoch+1;  // day of month
}


dTtime_t ZDateTimeClass::makeTime(byte sec, byte min, byte hour, byte day, byte month, int year ){
// converts time components to dTtime_t 
// note year argument is full four digit year (or digits since 2000), i.e.1975, (year 8 is 2008)
  
   int i;
   dTtime_t seconds;

   if(year < 69) 
      year+= 2000;
    // seconds from 1970 till 1 jan 00:00:00 this year
    seconds= (year-1970)*(60*60*24L*365);

    // add extra days for leap years
    for (i=1970; i<year; i++) {
        if (LEAP_YEAR(i)) {
            seconds+= 60*60*24L;
        }
    }
    // add days for this year
    for (i=0; i<month; i++) {
      if (i==1 && LEAP_YEAR(year)) { 
        seconds+= 60*60*24L*29;
      } else {
        seconds+= 60*60*24L*monthDays[i];
      }
    }

    seconds+= (day-1)*3600*24L;
    seconds+= hour*3600L;
    seconds+= min*60L;
    seconds+= sec;
    return seconds; 
}

// get years with century
int ZDateTimeClass::GetYear()
{
	return ((int)(this->Year) + 1900);
}


////////////////////////////////////////////////////////////////////////////////
// ZTimeSpan implementation

ZTimeSpan::ZTimeSpan (dTtime_t seconds):
  _seconds(seconds)
{}

ZTimeSpan::ZTimeSpan (int16_t days, int8_t hours, int8_t minutes, int8_t seconds):
  _seconds((dTtime_t)days*86400L + (dTtime_t)hours*3600 + (dTtime_t)minutes*60 + seconds)
{}

ZTimeSpan::ZTimeSpan (const ZTimeSpan& copy):
  _seconds(copy._seconds)
{}

ZTimeSpan ZTimeSpan::operator+(const ZTimeSpan& right) {
  return ZTimeSpan(_seconds+right._seconds);
}

ZTimeSpan ZTimeSpan::operator-(const ZTimeSpan& right) {
  return ZTimeSpan(_seconds-right._seconds);
}


// make one instance for ZDateTime class the user 
ZDateTimeClass ZDateTime = ZDateTimeClass() ;
