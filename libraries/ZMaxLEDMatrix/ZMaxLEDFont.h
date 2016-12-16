#ifndef ZMaxLEDFont_h
#define ZMaxLEDFont_h

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


class ZMaxLEDFontBase
{
  public:
        /* 
         * Create a new font
         * Params :
         */
        ZMaxLEDFontBase();
				
		/* --  -- */
		virtual bool BeginCharacter(char argChar);
		
		/* --  -- */
		virtual void EndCharacter();
		
		/* --  -- */
		virtual uint8_t GetCharWidthPixels();
		
		/* --  -- */
		virtual uint8_t GetCharBitmapValueByte();

		/* --  -- */
		virtual uint8_t GetCharBitmapValueWord();

		virtual uint8_t* GetCharStoreAddress();

		/* --  -- */
		virtual bool GetNextValue();

		/* --  -- */
		virtual uint8_t GetBytesPerCharBitmap();
		
		/* --  -- */
		bool IsCharSize8x5();
		
		/* --  -- */
		bool IsCharSize16x10();
		
		/* --  -- */
		bool IsValidChar(char argChar);

		/* --  -- */
		uint8_t GetCharStoreIndex(char argChar);

		/* --  -- */
		uint8_t GetCharWidthPixels(char argChar);
		
		
  protected:
		uint8_t m_opcode01;
		uint8_t m_bytesPerBitmap;
		uint16_t *m_charInfoVector;
		uint8_t *m_charBitmaps;
		uint8_t m_charValidMin;
		uint8_t m_charValidMax;
		uint16_t m_actualIndexWord;
		uint8_t m_charActual;
		uint8_t m_actualCharStoreIndex;
		uint8_t m_actualCharWidthPixels;
		uint8_t m_actualCharLineIndex;
		
		
		/* -- runtime variables -- */
		uint8_t m_byte01;
		uint8_t m_byte02;
		uint8_t m_byte03;
		
		uint16_t m_word01;
		uint16_t m_word02;
		uint16_t m_word03;

};


#endif
