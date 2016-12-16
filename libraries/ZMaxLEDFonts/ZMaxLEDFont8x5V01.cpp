/* -- Zoltan Sari
   -- LED matrix of Max7218 controlled 8x8 modules (Arduino UNO / Nano)
   -- Library was created: 2016.10.15.
   --
 -- */

#include "ZMaxLEDFont8x5V01.h"

 const uint16_t ZMaxLEDFont8x5V01::v_charInfoVector[5] = {
	0x0500,	/* 0 */
	0x0301,	/* 1 */
	0x0502,	/* 2 */
	0x0503,	/* 3 */
	0x0504	/* 4 */
};

 const uint8_t ZMaxLEDFont8x5V01::v_charTable [] PROGMEM  = {
/* -- 0 -- */
  0x00,
  0x0e,
  0x11,
  0x13,
  0x15,
  0x19,
  0x11,
  0x0e,
/* -- 1 -- */
  0x00,
  0x07,
  0x02,
  0x02,
  0x02,
  0x02,
  0x03,
  0x02,
/* -- 2 -- */
  0x00,
  0x1F,
  0x02,
  0x04,
  0x08,
  0x10,
  0x11,
  0x0E,
/* -- 3 -- */
  0x00,
  0x0E,
  0x11,
  0x10,
  0x08,
  0x10,
  0x11,
  0x0E,
/* -- 4 -- */
  0x00,
  0x08,
  0x1F,
  0x09,
  0x0A,
  0x0C,
  0x08,
  0x08
	
};

ZMaxLEDFont8x5V01::ZMaxLEDFont8x5V01()
{
	m_opcode01 = 0;
	m_bytesPerBitmap = 8;
	m_charInfoVector = (uint16_t *)v_charInfoVector;
	m_charBitmaps = (uint8_t *)v_charTable;
	m_charValidMin = (uint8_t)('0');
	m_charValidMax = (uint8_t)('4');
	
	
}
		