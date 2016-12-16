/* ---

  2016.12.14. Scheduler with ZDateTime
  
  
  Zoltan Sari
-- */

#include "ZScheduler.h"

ZSchedulerEntry::ZSchedulerEntry(eDaystoGo_t argDay, eProcess_t argProc, uint8_t argHour, uint8_t argMinute) :
 DayToGo(argDay), ProcessToGo(argProc), Hour(argHour), Minute(argMinute)
{
	this->Init();
}

void ZSchedulerEntry::Init()
{
	this->m_nextFireSeconds = 0;
	this->m_lastFireSeconds = 0;
	this->m_check1 = 0;
	this->m_enabled = true;
	this->m_isFired = true;
}

dTtime_t ZSchedulerEntry::GetLastFired()
{
	if(this->m_isFired)
	{
		return this->m_lastFireSeconds;
	}
	return 0;
}

dTtime_t ZSchedulerEntry::GetNextFire()
{
	return this->m_nextFireSeconds;	
}

bool ZSchedulerEntry::Check(ZDateTimeClass *argTime)
{
	argTime->available(); //refresh the Date and time properties
	
	this->m_check1 = argTime->now();
	if((this->m_nextFireSeconds == 0) && (this->DayToGo != dgNever))
	{
		/* -- set the next fire time -- */
		this->m_nextFireSeconds = argTime->makeTime(
			0,
			this->Minute,
			this->Hour,
			argTime->Day,
			argTime->Month,
			(int)(1900 + argTime->Year));
		
		if((DayToGo != dgEveryday) &&
		    (
		     ((this->DayToGo == dgWeekday) && ((argTime->DayofWeek == 0) || (argTime->DayofWeek == 6))) ||
			 ((this->DayToGo == dgWeekend) && (argTime->DayofWeek >= 1) && (argTime->DayofWeek <= 5))
			)
		   )
		{
			int x = (1 << (int)argTime->DayofWeek);
			while(x != (int)this->DayToGo)
			{
				this->m_nextFireSeconds += SECS_PER_DAY;
				x = x << 1;
				if(x > 0x40)
				{
					x = 0x01;
				}
			}
		}
	}
	
	return false;
}
