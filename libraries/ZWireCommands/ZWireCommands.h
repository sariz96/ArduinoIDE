#ifndef ZWireCommands_h
#define ZWireCommands_h

/* -- Zoltan Sari
   -- ESP8266 <--> Arduino Nano I2C Commands
   -- Library was created: 2016.11.30.
   --
	
 -- */

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
 
 
#define ZWIRECOMM_MAX_BUFFER_LENGTH 32
#define ZWIRECOMM_MAX_BUFFER_AVG_LENGTH 8
 
 /* --- Commands -- */
 /* --- RF433 communication -- */
#define ZWIRECOMM_RF433_SEND_CHAR1 ((uint16_t)400)
/* --- debug LCD display -- */
#define ZWIRECOMM_LCD_DEBUG_TEXT ((uint16_t)100)
/* --- time -- */
#define ZWIRECOMM_NTP_SET ((uint16_t)200)
#define ZWIRECOMM_NTP_GET_MEM ((uint16_t)201)
#define ZWIRECOMM_NTP_GET_RTC ((uint16_t)202)
/* --- RTC -- */
#define ZWIRECOMM_RTC_READ_NVRAM ((uint16_t)210)
#define ZWIRECOMM_RTC_GET_NVRAM ((uint16_t)211)
#define ZWIRECOMM_RTC_WRITE_NVRAM ((uint16_t)212)
/* --- Sensors DHT, DS -- */
#define ZWIRECOMM_DHTDS_LASTTXT ((uint16_t)300)
#define ZWIRECOMM_DHTDS_LASTBIN ((uint16_t)301)

/* --- test - debug -- */
#define ZWIRECOMM_TEST_GET_TXT04 ((uint16_t)1001)
#define ZWIRECOMM_TEST_GET_TXT08 ((uint16_t)1002)
#define ZWIRECOMM_TEST_GET_TXT16 ((uint16_t)1003)

class ZBuffer08
{
public:
	ZBuffer08(uint8_t argSize);
	uint8_t *GetBuffer();
	uint8_t GetBufferLength();
	uint8_t GetUsedBufferLength();
	
	void ClearBuffer();
	void ClearBuffer(uint8_t argLen);
	void ClearBuffer(int argLen);
	
	/* -- argIdx: 0 base without Command word ! -- */
	void SetClientByte(uint8_t argIdx, uint8_t argValueByte);
	void SetClientWord(uint8_t argIdx, uint16_t argValueWord);
	void SetClientDWord(uint8_t argIdx, uint32_t argValueDWord);
	uint8_t SetClientTxt(uint8_t argIdx, char *argTxt, uint8_t argLength);
	void SetClientFloat(uint8_t argIdx, float argValueFloat);

	/* -- argIdx: 0 base without Command word ! -- */
	uint8_t GetClientByte(uint8_t argIdx);
	uint16_t GetClientWord(uint8_t argIdx);
	uint32_t GetClientDWord(uint8_t argIdx);
	uint8_t GetClientTxt(uint8_t argIdx, char *argTxt, uint8_t argLength);
	float GetClientFloat(uint8_t argIdx);

protected:
	uint8_t m_buffer[ZWIRECOMM_MAX_BUFFER_LENGTH];
	uint8_t m_size;
	uint8_t m_usedLength;
	uint32_t m_dw1;
	uint32_t m_dw2;
	uint8_t m_bTmp1;
	uint8_t m_bTmp2;
	uint8_t m_bTmp3;
	char *m_ptrCh1;
	char *m_ptrCh2;
};

class ZWireCommands : public ZBuffer08
{
public:
	ZWireCommands();

	void SetCommand(uint16_t argCmd);
	void SetCommand(uint16_t argCmd, uint8_t argClearedBuffLen);
	uint16_t GetCommand();
	uint16_t GetLastReceivedCommand();
	void ClearLastReceivedCommand();
	
	/* -- argIdx: 0 base after Command word ! -- */
	void SetByte(uint8_t argIdx, uint8_t argValueByte);
	void SetWord(uint8_t argIdx, uint16_t argValueWord);
	void SetDWord(uint8_t argIdx, uint32_t argValueDWord);
	void SetFloat(uint8_t argIdx, float argValueFloat);
	uint8_t SetTxt(uint8_t argIdx, char *argTxt, uint8_t argLength);

	/* -- argIdx: 0 base after Command word ! -- */
	uint8_t GetByte(uint8_t argIdx);
	uint16_t GetWord(uint8_t argIdx);
	uint32_t GetDWord(uint8_t argIdx);
	float GetFloat(uint8_t argIdx);
	uint8_t GetTxt(uint8_t argIdx, char *argTxt, uint8_t argLength);
	
protected:
	uint16_t m_uLastReceivedCommand;
};



#endif
