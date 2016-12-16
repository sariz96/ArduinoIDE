/* -- Zoltan Sari
   -- LED matrix of Max7218 controlled 8x8 modules (Arduino UNO / Nano)
   -- Library was created: 2016.10.15.
   --
 -- */

#include "ZMaxLEDFont.h"

//the opcodes
#define ZMLEDDFONT_OPCODE01_16x10_8x5 0x01

ZMaxLEDFontBase::ZMaxLEDFontBase()
{
	m_opcode01 = 0;
	m_bytesPerBitmap = 0;
	m_charInfoVector = NULL;
	m_charBitmaps = NULL;
	m_charValidMin = 0;
	m_charValidMax = 0;
	m_charActual = 0;
	m_actualIndexWord = 0;
	m_actualCharStoreIndex = 0;
	m_actualCharWidthPixels = 0;
	m_actualCharLineIndex = 0;
	
}

uint8_t ZMaxLEDFontBase::GetBytesPerCharBitmap()
{
	return m_bytesPerBitmap;
}

bool ZMaxLEDFontBase::IsCharSize8x5()
{
	if((m_opcode01 & ZMLEDDFONT_OPCODE01_16x10_8x5) == 0x00)
	{
		return true;
	}
	return false;
}

bool ZMaxLEDFontBase::IsCharSize16x10()
{
	if((m_opcode01 & ZMLEDDFONT_OPCODE01_16x10_8x5) != 0x00)
	{
		return true;
	}
	return false;
}

bool ZMaxLEDFontBase::IsValidChar(char argChar)
{
	m_byte01 = (uint8_t)argChar;
	m_actualIndexWord = 0;
	if ((m_byte01 >= m_charValidMin) && (m_byte01 <= m_charValidMax) && (m_charInfoVector != NULL) )
	{
		m_byte02 = m_byte01 - m_charValidMin;
		m_actualIndexWord = (pgm_read_word_near(m_charInfoVector + m_byte02));
		if(m_actualIndexWord != 0)
		{
			return true;
		}
	}
	return false;
}

uint8_t ZMaxLEDFontBase::GetCharStoreIndex(char argChar)
{
	if( IsValidChar(argChar) )
	{
		m_actualCharStoreIndex = (uint8_t)(m_actualIndexWord & 0x00ff);
		return m_actualCharStoreIndex;
	}
	m_actualCharStoreIndex = 0;
	return 0x00;
}

uint8_t ZMaxLEDFontBase::GetCharWidthPixels(char argChar)
{
	if( IsValidChar(argChar) )
	{
		m_actualCharWidthPixels = (uint8_t)((m_actualIndexWord & 0x0f00 ) >> 8);
		return m_actualCharWidthPixels;
	}
	m_actualCharWidthPixels = 0;
	return 0x00;
}

uint8_t ZMaxLEDFontBase::GetCharWidthPixels()
{
	return m_actualCharWidthPixels;
}

bool ZMaxLEDFontBase::BeginCharacter(char argChar)
{
	m_charActual = 0;
	m_actualCharStoreIndex = 0;
	m_actualCharWidthPixels = 0;
	m_actualCharLineIndex = 0;

	if( IsValidChar(argChar) )
	{
		m_charActual = m_byte01;
		m_actualCharStoreIndex = (uint8_t)(m_actualIndexWord & 0x00ff);
		m_actualCharWidthPixels = (uint8_t)((m_actualIndexWord & 0x0f00 ) >> 8);
		
		return true;
	}
	return false;
}

void ZMaxLEDFontBase::EndCharacter()
{
	m_charActual = 0;
	m_actualCharStoreIndex = 0;
	m_actualCharWidthPixels = 0;
	m_actualCharLineIndex = 0;	
	m_actualIndexWord = 0;
}

uint8_t* ZMaxLEDFontBase::GetCharStoreAddress()
{
	if(m_charBitmaps != NULL)
	{
		return (m_charBitmaps + ((int)(m_bytesPerBitmap) * (int)m_actualCharStoreIndex));
	}
	return NULL;
}

uint8_t ZMaxLEDFontBase::GetCharBitmapValueByte()
{
	return 0;	
}

uint8_t ZMaxLEDFontBase::GetCharBitmapValueWord()
{
	return 0;
}

bool ZMaxLEDFontBase::GetNextValue()
{
	return false;
}
