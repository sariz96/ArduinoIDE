/* ---

  2016.12.14. Scheduler with ZDateTime
  
  
  Zoltan Sari
-- */

#ifndef ZScheduler_h
#define ZScheduler_h

#include "ZDateTime.h"

typedef enum {
	  dgNever = 0x00,
	  
	  dgSunday = 0x01,
	  dgMonday = 0x02,
	  dgTuesday = 0x04,
	  dgWednesday = 0x08,
	  dgThursday = 0x10,
	  dgFriday = 0x20,
	  dgSaturday = 0x40,
	  
	  dgWeekday = 0x3e,
	  dgWeekend = 0x41,
	  dgEveryday = 0xff
} eDaystoGo_t;

typedef enum {
	pcNothing = 0,
	pcWakeUp = 1,
	pcSwitchOffEachLamp = 2,
	pcSwitchOnLamp1 = 10,
	pcSwitchOnLamp2 = 11,
	pcSwitchOnLamp3 = 12,
	pcSwitchOnLampLEDs = 13,
	pcSwitchOffLamp1 = 20,
	pcSwitchOffLamp2 = 21,
	pcSwitchOffLamp3 = 22,
	pcSwitchOffLampLEDs = 23,
	
} eProcess_t;

class ZSchedulerEntry
{
public:
	ZSchedulerEntry(eDaystoGo_t argDay, eProcess_t argProc, uint8_t argHour, uint8_t argMinute);
	void Init();
	bool Check(ZDateTimeClass *argTime);
	dTtime_t GetLastFired();
	dTtime_t GetNextFire();
	
	eDaystoGo_t	DayToGo;
	eProcess_t ProcessToGo;
	uint8_t Hour;
	uint8_t Minute;	
private:	
    dTtime_t m_nextFireSeconds;
    dTtime_t m_lastFireSeconds;
	dTtime_t m_check1;
	bool m_isFired;
	bool m_enabled;
};


#endif /* ZScheduler_h */
