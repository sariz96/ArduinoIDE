/* -- Zoltan Sari
   -- LED matrix of Max7218 controlled 8x8 modules (Arduino UNO / Nano)
   -- Library was created: 2016.10.15.
   --
 -- */

#include "ZMaxLEDMatrix.h"

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

ZMaxLEDMatrix::ZMaxLEDMatrix(int argdataPin, int argclkPin, int argcsPin, int argnumDevices,
			int argNodesHorizontal, int argNodesVertical,
			int argVirtualNodesHorizontal, int argVirtualNodesVertical )
{
    m_SPI_MOSI=argdataPin;
    m_SPI_CLK=argclkPin;
    m_SPI_CS=argcsPin;
    if(argnumDevices <= 0 || argnumDevices > MY_MAXIMUM_NUMBER_OF_DEVICES )
        argnumDevices = MY_MAXIMUM_NUMBER_OF_DEVICES;
    m_maxDevices=argnumDevices;
    pinMode(m_SPI_MOSI,OUTPUT);
    pinMode(m_SPI_CLK,OUTPUT);
    pinMode(m_SPI_CS,OUTPUT);
    digitalWrite(m_SPI_CS,HIGH);
    m_SPI_MOSI=argdataPin;
	
	if(argNodesHorizontal < 1)
	{
		argNodesHorizontal = 1;
	}
	if(argNodesVertical < 1)
	{
		argNodesVertical = 1;
	}
	m_nodesHorizontal = argNodesHorizontal;
	m_nodesVertical = argNodesVertical;
	if(argVirtualNodesHorizontal < argNodesHorizontal)
	{
	 argVirtualNodesHorizontal = argNodesHorizontal;
	}
	m_virtualNodesHorizontal = argVirtualNodesHorizontal;
	if(argVirtualNodesVertical < argNodesVertical)
	{
	 argVirtualNodesVertical = argNodesVertical;
	}
	m_virtualNodesVertical = argVirtualNodesVertical;
	m_k = m_virtualNodesHorizontal * m_virtualNodesVertical;
	if(m_k > (MY_MAXIMUM_NUMBER_OF_VIRTUAL_DEVICES))
	{
		m_virtualNodesVertical = ((MY_MAXIMUM_NUMBER_OF_VIRTUAL_DEVICES) / m_virtualNodesHorizontal);
		if(m_virtualNodesVertical < m_nodesVertical)
		{
		 m_nodesVertical = m_virtualNodesVertical;
		}
	}
	m_displayOffsetX = 0;
	m_displayOffsetY = 0;
	
	m_pixelDataLen = (m_virtualNodesHorizontal * m_virtualNodesVertical * 8);
	m_xMin = 0;
	m_xMax = (m_virtualNodesHorizontal * 8) - 1;
	m_yMin = 0;
	m_yMax = (m_virtualNodesVertical * 8) - 1;
	
	Init();
	delayMicroseconds(1);
	SpiTransferEach(OP_DISPLAYTEST,0);
	delayMicroseconds(1);
	SpiTransferEach(OP_SCANLIMIT, 7);
	delayMicroseconds(1);
	SpiTransferEach(OP_DECODEMODE,0);
	delayMicroseconds(1);
	ClearDisplayEach();
	Display();
/*	
*/		
	delayMicroseconds(1);
	ShutdownEach(true);
	delayMicroseconds(1);
}

ZMaxLEDMatrix::ZMaxLEDMatrix(
	int argdataPin, int argclkPin, int argcsPin,
	int argnumDevices,
	int argNodesHorizontal, int argNodesVertical)
{
	ZMaxLEDMatrix(argdataPin, argclkPin, argcsPin,
		argnumDevices,
		argNodesHorizontal, argNodesVertical,
		argNodesHorizontal, argNodesVertical);
}

void ZMaxLEDMatrix::Init()
{
    for(m_i = 0; m_i < (MY_MAXIMUM_NUMBER_OF_DEVICES * 2);m_i++)
	{		
        m_spiData[m_i] = (byte)0;
	}
    for(m_i = 0; m_i < m_pixelDataLen;m_i++)
	{		
		m_pixelData[m_i] = (byte)0;
	}	
}

int ZMaxLEDMatrix::GetPixelDataLen()
{
    return m_pixelDataLen;	
}

int ZMaxLEDMatrix::GetDeviceCount()
{
    return m_maxDevices;
}

int ZMaxLEDMatrix::GetVirtualNodesHorizontal()
{
	return m_virtualNodesHorizontal;
}

int ZMaxLEDMatrix::GetVirtualNodesVertical()
{
	return m_virtualNodesVertical;
}

int ZMaxLEDMatrix::GetDisplayNodesHorizontal()
{
	return m_nodesHorizontal;
}

int ZMaxLEDMatrix::GetDisplayNodesVertical()
{
	return m_nodesVertical;
}

int ZMaxLEDMatrix::GetMinX()
{
    return m_xMin;
}

int ZMaxLEDMatrix::GetMaxX()
{
    return m_xMax;
}

int ZMaxLEDMatrix::GetMinY()
{
    return m_yMin;
}

int ZMaxLEDMatrix::GetMaxY()
{
    return m_yMax;
}

int ZMaxLEDMatrix::GetDisplayMinX()
{
    return m_xMin + m_displayOffsetX;
}

int ZMaxLEDMatrix::GetDisplayMaxX()
{
    return m_xMin + m_displayOffsetX + (m_nodesHorizontal * 8) - 1;
}

int ZMaxLEDMatrix::GetDisplayMinY()
{
    return m_yMin + m_displayOffsetY;
}

int ZMaxLEDMatrix::GetDisplayMaxY()
{
    return m_yMin + m_displayOffsetY + (m_nodesVertical * 8) - 1;
}

bool ZMaxLEDMatrix::SetDisplayOffset(int argOffsetX, int argOffsetY)
{
	if ((argOffsetX < 0) ||
		(argOffsetX > m_xMax) ||
		(argOffsetY < 0) ||
		(argOffsetY > m_yMax) ||
		((argOffsetX % 8) != 0) )
		{
			return false;
		}
	m_displayOffsetX = argOffsetX;
	m_displayOffsetY = argOffsetY;
}


void ZMaxLEDMatrix::Shutdown(int argaddr, bool argstatus)
{
    if((argaddr < 0) || (argaddr >= m_maxDevices))
        return;
    if(argstatus)
        SpiTransfer(argaddr, OP_SHUTDOWN,0);
    else
        SpiTransfer(argaddr, OP_SHUTDOWN,1);
}

void ZMaxLEDMatrix::ShutdownEach(bool argstatus)
{
    if(argstatus)
        SpiTransferEach(OP_SHUTDOWN,0);
    else
        SpiTransferEach(OP_SHUTDOWN,1);
	
}

void ZMaxLEDMatrix::SetScanLimit(int argaddr, int arglimit)
{
    if((argaddr < 0) || (argaddr >= m_maxDevices))
        return;
    if((arglimit >= 0) && (arglimit < 8))
        SpiTransfer(argaddr, OP_SCANLIMIT, arglimit);
}

void ZMaxLEDMatrix::SetScanLimitEach(int arglimit)
{
    if((arglimit >= 0) && (arglimit < 8))
        SpiTransferEach(OP_SCANLIMIT, arglimit);	
}

void ZMaxLEDMatrix::SetIntensity(int argaddr, int argintensity) {
    if((argaddr < 0) || (argaddr >= m_maxDevices))
        return;
    if((argintensity >= 0) && (argintensity < 16))
        SpiTransfer(argaddr, OP_INTENSITY, argintensity);
}

void ZMaxLEDMatrix::SetIntensityEach(int argintensity)
{
    if((argintensity >= 0) && (argintensity < 16))
        SpiTransferEach(OP_INTENSITY, argintensity);	
}

void ZMaxLEDMatrix::DisplayTestEach(bool argTest)
{
	if(argTest)
	{
	  SpiTransferEach(OP_DISPLAYTEST,1);		
	}
	else
	{
	  SpiTransferEach(OP_DISPLAYTEST,0);
	}
}

void ZMaxLEDMatrix::ClearDisplay(int argaddr)
{
	m_k = m_virtualNodesHorizontal * m_virtualNodesVertical;
    if((argaddr < 0) || (argaddr >= m_k))
        return;
	
    m_offset = m_virtualNodesHorizontal * ((argaddr / m_virtualNodesHorizontal) * 8) + (argaddr % m_virtualNodesHorizontal);
    for(m_i = 0; m_i < 8; m_i++)
	{
        m_pixelData[m_offset] = 0;
		m_offset += m_virtualNodesHorizontal;
    }
}

void ZMaxLEDMatrix::ClearDisplayEach()
{
    for(m_i = 0; m_i < m_pixelDataLen;m_i++)
	{		
		m_pixelData[m_i] = (byte)0;
	}		
}

void  ZMaxLEDMatrix::SetLed(int argaddr, int argrow, int argcol, boolean argState)
{
    m_bval = 0x00;
	
    if((argaddr < 0) || (argaddr >= m_maxDevices))
        return;
    if((argrow < 0) || (argrow >= 8))
        return;
    if((argcol < 0) || (argcol >= 8))
        return;
	
    m_offset = (m_nodesHorizontal + 2) * (argrow + ((argaddr / m_nodesHorizontal) * 8)) + (argaddr % m_nodesHorizontal) + 1;
    m_bval = B10000000 >> argcol;
    if(argState)
	{
        m_pixelData[m_offset] |= m_bval;
	}
    else
	{
        m_bval =~ m_bval;
        m_pixelData[m_offset] &= m_bval;
    }
}

void   ZMaxLEDMatrix::SetRow(int argaddr, int argrow, byte argvalue)
{
    if((argaddr < 0) || (argaddr >= m_maxDevices))
        return;
    if((argrow < 0) || (argrow >= 8))
        return;

    m_offset = (m_nodesHorizontal + 2) * (argrow + ((argaddr / m_nodesHorizontal) * 8)) + (argaddr % m_nodesHorizontal) + 1;
    m_pixelData[m_offset] = argvalue;
}

/* -- Set the pixel with the logical coordinates  -- */
/* --  0 < argX < (m_virtualNodesHorizontal * 8)  -- */
/* --  0 < argY < (m_virtualNodesVertical * 8)  -- */
/* -- It uses the virtual coordinates! -- */
/* -- -- */
/* -- -- */
void ZMaxLEDMatrix::SetPixel(int argX, int argY, boolean argState)
{
	if((m_virtualNodesHorizontal > 0) &&
		(m_virtualNodesVertical > 0) &&
		(argX >= m_xMin) &&
		(argY >= m_yMin) &&
		(argX <= m_xMax) &&
		(argY <= m_yMax)
		)
		{
			m_offset = (m_virtualNodesHorizontal * argY) + (argX / 8);
			m_x2 = 7 - (argX % 8);
			m_bval = B10000000 >> m_x2;
			if(argState)
			{
				m_pixelData[m_offset] |= m_bval;
			}
			else
			{
				m_bval =~ m_bval;
				m_pixelData[m_offset] &= m_bval;
			}
		}
}

/* -- Draw a vertical line in virtual pixel memory
	argX:    logical x position (line) in virtual display memory,
	argYMin: minimal logical y position (line) in virtual display memory,
	argYMax: maximal logical y position (line) in virtual display memory,
-- */		
void ZMaxLEDMatrix::DrawVerticalLine(int argX, int argYMin, int argYMax, boolean argState)
{
	if(argYMin < m_yMin)
	{
		argYMin = m_yMin;
	}
	if(argYMax > m_yMax)
	{
		argYMax = m_yMax;
	}
	if(argYMin > argYMax)
	{
		argYMin = argYMax;
	}
	
	if((m_virtualNodesHorizontal > 0) &&
		(m_virtualNodesVertical > 0) &&
		(argX >= m_xMin) &&
		(argX <= m_xMax)
		)
		{
			m_y1 = argYMin;
			m_offset = (m_virtualNodesHorizontal * m_y1) + (argX / 8);
			m_x2 = 7 - (argX % 8);
			m_bval = B10000000 >> m_x2;
			if(argState)
			{
				while(m_y1 <= argYMax)
				{
				  m_pixelData[m_offset] |= m_bval;
				  m_y1++;
				  m_offset += m_virtualNodesHorizontal;
				}
			}
			else
			{
				m_bval =~ m_bval;
				while(m_y1 <= argYMax)
				{				
				  m_pixelData[m_offset] &= m_bval;
				  m_y1++;
				  m_offset += m_virtualNodesHorizontal;
				}
			}			
		}
}

/* -- Draw a vertical line in virtual pixel memory
	argX:    logical x position (line) in virtual display memory,
	argYMin: minimal logical y position (line) in virtual display memory,
	argYMax: maximal logical y position (line) in virtual display memory,
	argWidth: width in pixels,
	argState: set or clear pixel
-- */		
void ZMaxLEDMatrix::DrawVerticalLineWidth(int argX, int argYMin, int argYMax, int argWidth, boolean argState)
{
	if(argYMin < m_yMin)
	{
		argYMin = m_yMin;
	}
	if(argYMax > m_yMax)
	{
		argYMax = m_yMax;
	}
	if(argYMin > argYMax)
	{
		argYMin = argYMax;
	}
	
	if((m_virtualNodesHorizontal > 0) &&
		(m_virtualNodesVertical > 0) &&
		(argX >= m_xMin) &&
		(argX <= m_xMax)
		)
		{
			m_y1 = argYMin;
			m_offset = (m_virtualNodesHorizontal * m_y1) + (argX / 8);
			m_x2 = 7 - (argX % 8);
			m_bval = B10000000 >> m_x2;
			if(argState)
			{
				while(m_y1 <= argYMax)
				{
				  m_pixelData[m_offset] |= m_bval;
				  m_y1++;
				  m_offset += m_virtualNodesHorizontal;
				}
			}
			else
			{
				m_bval =~ m_bval;
				while(m_y1 <= argYMax)
				{				
				  m_pixelData[m_offset] &= m_bval;
				  m_y1++;
				  m_offset += m_virtualNodesHorizontal;
				}
			}			
		}	
}

/* -- Draw a vertical line in virtual pixel memory (argX % 8 = 0)
	argX:    logical x position (line) in virtual display memory (argWidth <= 32),
	argYMin: minimal logical y position (line) in virtual display memory,
	argYMax: maximal logical y position (line) in virtual display memory,
	argWidth: width in pixels,
	argState: set or clear pixel
-- */		
void ZMaxLEDMatrix::DrawVerticalLineWidthGrid0(int argX, int argYMin, int argYMax, int argWidth, boolean argState)
{
	if(argYMin < m_yMin)
	{
		argYMin = m_yMin;
	}
	if(argYMax > m_yMax)
	{
		argYMax = m_yMax;
	}
	if(argYMin > argYMax)
	{
		argYMin = argYMax;
	}
	if(argX < m_xMin)
	{
		argX = m_xMin;
	}
	else if (argX > m_xMax)
	{
		argX = m_xMax;
	}
		
	if((argX % 8) != 0)
	{
		argX -= (argX % 8);
	}
	if(argWidth > 32)
	{
		argWidth = 32;
	}

	m_y1 = argYMin;
	m_offset = (m_virtualNodesHorizontal * m_y1) + (argX / 8);
	m_dw1 = 0x01;
	if(argState)
	{
		while(m_y1 <= argYMax)
		{
		  m_pixelData[m_offset] |= m_bval;
		  m_y1++;
		  m_offset += m_virtualNodesHorizontal;
		}
	}
	else
	{
		m_bval =~ m_bval;
		while(m_y1 <= argYMax)
		{				
		  m_pixelData[m_offset] &= m_bval;
		  m_y1++;
		  m_offset += m_virtualNodesHorizontal;
		}
	}			

}

		
bool ZMaxLEDMatrix::Display()
{
	SpiTransferPixelsEach();
	return true;
}

void ZMaxLEDMatrix::ClearCharPos1(int argRow, int argPos)
{
    if((m_virtualNodesVertical <= 0) || (m_virtualNodesHorizontal <= 0) ||
	   (argRow < 0) ||
	   (argRow >= m_virtualNodesVertical) ||
	   (argPos < 0) ||
	   (argPos >= m_virtualNodesHorizontal))
        return;

    m_offset = (m_virtualNodesHorizontal * argRow * 8) + argPos;
    for(m_i = 0; m_i < 8; m_i++)
	{
        m_pixelData[m_offset] = 0;
		m_offset += m_virtualNodesHorizontal;
    }	
}


void ZMaxLEDMatrix::PrintCharPos1(int argRow, int argPos, byte* argCharBits)
{	
    if((m_virtualNodesVertical <= 0) || (m_virtualNodesHorizontal <= 0) ||
	   (argRow < 0) ||
	   (argRow >= m_virtualNodesVertical) ||
	   (argPos < 0) || (argPos >= m_virtualNodesHorizontal))
        return;

    m_offset = (m_virtualNodesHorizontal * argRow * 8) + argPos;
    for(m_bi = 0; m_bi < 8; m_bi++)
	{
		m_bval = pgm_read_byte_near(argCharBits + m_bi);
        m_pixelData[m_offset] = m_bval;
		m_offset += m_virtualNodesHorizontal;
    }	
}

/* --- --- */
int ZMaxLEDMatrix::PrintTxt(int argX, int argY, char *argTxt, int argMaxLength, ZMaxLEDFontBase *argFont)
{
	if((argTxt == NULL) ||
		(*argTxt == 0) ||
		(argFont == NULL) ||
		(argMaxLength <= 0))
	{
	 return -1;
	}
    if((m_virtualNodesVertical <= 0) ||
	   (m_virtualNodesHorizontal <= 0) ||
	   (argX < m_xMin) || (argX > m_xMax) ||
	   (argY < m_yMin) || (argY > m_yMax))
	 return -2;
	
	m_xPrint1 = argX;
	m_yPrint1 = argY;
	for(m_idxPrint1 = 0; (m_idxPrint1 < argMaxLength) && (*argTxt != 0) && (m_xPrint1 <= m_xMax); m_idxPrint1++)
	{
		if( argFont->BeginCharacter(*argTxt))
		{
			m_PrintCharBits1 = (byte*)argFont->GetCharStoreAddress();
			m_widthPrint1 = (int)argFont->GetCharWidthPixels();
			PrintCharPos2(m_xPrint1, m_yPrint1, m_widthPrint1, m_PrintCharBits1);
			m_xPrint1 += (m_widthPrint1 + 1);
			argFont->EndCharacter();
		}
		argTxt++;
	}	
	
	return m_xPrint1;
}

int ZMaxLEDMatrix::PrintStr(int argX, int argY, String argTxt, int argMaxLength, ZMaxLEDFontBase *argFont)
{
	if((argTxt == NULL) ||
		(argFont == NULL) ||
		(argMaxLength <= 0))
	{
	 return -1;
	}
    if((m_virtualNodesVertical <= 0) ||
	   (m_virtualNodesHorizontal <= 0) ||
	   (argX < m_xMin) || (argX > m_xMax) ||
	   (argY < m_yMin) || (argY > m_yMax))
	 return -2;

 	m_xPrint1 = argX;
	m_yPrint1 = argY;
	if(argMaxLength > 255)
	{
		argMaxLength = 255;
	}
	if(argMaxLength < 1)
	{
	 return 0;
	}
	argTxt.toCharArray(m_chBuffer1, (unsigned int)argMaxLength);
	m_widthPrint1 = sizeof(m_chBuffer1);
	if(argMaxLength > m_widthPrint1)
	{
		argMaxLength = m_widthPrint1;
	}
	
	m_ch01 = m_chBuffer1[0];
	for(m_idxPrint1 = 0; (m_idxPrint1 < argMaxLength) && (m_ch01 != 0) && (m_xPrint1 <= m_xMax); m_idxPrint1++)
	{
		m_ch01 = m_chBuffer1[m_idxPrint1];
		if( argFont->BeginCharacter(m_ch01))
		{
			m_PrintCharBits1 = (byte*)argFont->GetCharStoreAddress();
			m_widthPrint1 = (int)argFont->GetCharWidthPixels();
			PrintCharPos2(m_xPrint1, m_yPrint1, m_widthPrint1, m_PrintCharBits1);
			m_xPrint1 += (m_widthPrint1 + 1);
			argFont->EndCharacter();
		}
	}	

	return m_xPrint1;
}

/* ---
	argX: logical x position of the char,
	argY: logical y position of the char,
	argCharWidth: width of the char in pixels
	argCharBits: char bitmap (8 lines)
-- */
void ZMaxLEDMatrix::PrintCharPos2(int argX, int argY, int argCharWidth, byte* argCharBits)
{
    if((m_virtualNodesVertical <= 0) ||
	   (m_virtualNodesHorizontal <= 0) ||
	   (argX < m_xMin) || (argX > m_xMax) ||
	   (argY < m_yMin) || (argY > m_yMax) ||
	   (argCharWidth < 0) || (argCharWidth > 8) ||
	   ((argX + argCharWidth) > m_xMax) )
        return;
	
	m_idx	= -1;
	m_x1 = (argX % 8);
	m_x2 = m_x1 + argCharWidth - 1;
	if(m_x2 > 7)
	{
		m_idx	= (8 - m_x1);
	}

	m_offset = (m_virtualNodesHorizontal * argY) + (argX / 8);

	if(m_x1 == 0)
	{
		for(m_bi = 0; m_bi < 8; m_bi++)
		{
			m_bval = pgm_read_byte_near(argCharBits + m_bi);
			m_pixelData[m_offset] |= m_bval;
			m_offset += m_virtualNodesHorizontal;
		}
	}
	else if(m_idx < 0)
	{
		for(m_bi = 0; m_bi < 8; m_bi++)
		{
			m_bval = pgm_read_byte_near(argCharBits + m_bi);
			m_bval = m_bval << m_x1;
			m_pixelData[m_offset] |= m_bval;
			m_offset += m_virtualNodesHorizontal;
		}		
	}
	else
	{
		for(m_bi = 0; m_bi < 8; m_bi++)
		{
			m_w = (unsigned int)pgm_read_byte_near(argCharBits + m_bi);
			m_bval = (byte)(m_w << m_x1);
			m_pixelData[m_offset] |= m_bval;
			if(m_idx >= 0)
			{
			 m_bval = (byte)(m_w >> m_idx);
			 m_pixelData[m_offset + 1] |= m_bval;
			}			
			m_offset += m_virtualNodesHorizontal;
		}		
	}
}


/* Send out a single command to the device */
void ZMaxLEDMatrix::SpiTransfer(int argaddr, volatile byte argopcode, volatile byte argdata)
{
    m_offset = argaddr * 2;
    m_j = m_maxDevices * 2;

    for(m_i = 0; m_i < m_j; m_i++)
        m_spiData[m_i] = (byte)0;

   //put our device data into the array
    m_spiData[m_offset+1] = argopcode; /* -- high byte -- */
    m_spiData[m_offset] = argdata; /* -- low byte -- */
    digitalWrite(m_SPI_CS,LOW);
    //Now shift out the data 
    for(m_i = m_j; m_i > 0; m_i--)
        shiftOut(m_SPI_MOSI, m_SPI_CLK, MSBFIRST, m_spiData[m_i-1]);
    //latch the data onto the display
    digitalWrite(m_SPI_CS,HIGH);

}
		
/* Send out a single command each of the device */
void ZMaxLEDMatrix::SpiTransferEach(byte argopcode, byte argdata)
{
    m_j = m_maxDevices * 2;

    for(m_i = 0; m_i < m_j; m_i+=2)
	{
        m_spiData[m_i+1] = argopcode; /* -- high byte -- */
        m_spiData[m_i] = argdata; /* -- low byte -- */
	}
    digitalWrite(m_SPI_CS,LOW);
    //Now shift out the data 
    for(m_i = m_j; m_i > 0; m_i--)
        shiftOut(m_SPI_MOSI, m_SPI_CLK, MSBFIRST, m_spiData[m_i-1]);
    //latch the data onto the display
    digitalWrite(m_SPI_CS,HIGH);
	//delayMicroseconds(1);
}

/* Send out all pixels to each of the device */
/* -- It uses the virtual coordinates! -- */
/* -- m_displayOffsetX, m_displayOffsetY -- */
void ZMaxLEDMatrix::SpiTransferPixelsEach()
{
    for(m_i = 0; m_i < 8; m_i++)
	{
		/* -- init buffer -- */
		for(m_j = 0; m_j < m_maxDevices; m_j++)
		{
         m_spiData[m_j*2+1] = (byte)(m_i+1); /* -- high byte -- */
         m_spiData[m_j*2] = (byte)0; /* -- low byte -- */
		}
		m_y2 = m_i + m_displayOffsetY;
		m_x2 = (m_displayOffsetX / 8);
		if(m_y2 <= m_yMax)
		{
			m_offset = (m_virtualNodesHorizontal * m_y2);
			for(m_j = 0; m_j < m_maxDevices; m_j++)
			{
				if(m_y2 <= m_yMax)
				{
					if(m_x2 < m_virtualNodesHorizontal)
					{
						m_spiData[m_j*2] = m_pixelData[(m_j % m_nodesHorizontal) + m_offset + m_x2]; /* -- low byte -- */
					}
					if((m_j % m_nodesHorizontal) == (m_nodesHorizontal - 1))
					{
						m_y2 += 8;
						m_offset = (m_virtualNodesHorizontal * m_y2);
					}
				}
			}
		}
		digitalWrite(m_SPI_CS,LOW);
		//Now shift out the data 
		for(m_k = m_maxDevices*2; m_k > 0; m_k--)
		{
			shiftOut(m_SPI_MOSI, m_SPI_CLK, MSBFIRST, m_spiData[m_k-1]);
		}
		//latch the data onto the display
		digitalWrite(m_SPI_CS,HIGH);
		//delayMicroseconds(1);
	}	
}

/* -- Scroll left a line in virtual pixel memory
	argY: logical y position (line) in virtual display memory,
-- */
void ZMaxLEDMatrix::ScrollLeftVirtualLine(int argY)
{
	if((argY < m_yMin) || (argY > m_yMax))
		return;
	
	m_offset = (m_virtualNodesHorizontal * argY);
	
	if ((m_virtualNodesHorizontal % 4)	== 0)
	{
	 /* -- I can use 32bit long units -- */
	 m_i = m_virtualNodesHorizontal / 4;
	 m_ptrdw1 = (uint32_t *)(m_pixelData + m_offset);
	 for(m_idx = 0; m_idx < m_i; m_idx++)
	 {
		 m_dw1 = *m_ptrdw1;
		 if((m_idx > 0) && (m_dw1 & 0x00000001) != 0x00000000)
		 {
			 *(m_ptrdw1 - 1) |= 0x80000000;
		 }
		 *m_ptrdw1++ = m_dw1 >> 1;
	 }	 
	}
	else
	{
	 
	}
}

/* -- Scroll right a line in virtual pixel memory
	argY: logical y position (line) in virtual display memory,
-- */
void ZMaxLEDMatrix::ScrollRightVirtualLine(int argY)
{
	if((argY < m_yMin) || (argY > m_yMax))
		return;
	
	m_offset = (m_virtualNodesHorizontal * (argY + 1));
	
	if ((m_virtualNodesHorizontal % 4)	== 0)
	{
	 /* -- I can use 32bit long units -- */
	 m_i = m_virtualNodesHorizontal / 4;
	 m_ptrdw1 = (uint32_t *)(m_pixelData + m_offset);
	 for(m_idx = 0; m_idx < m_i; m_idx++)
	 {
		 m_ptrdw1--;
		 m_dw1 = *m_ptrdw1;
		 if((m_idx > 0) && (m_dw1 & 0x80000000) != 0x00000000)
		 {
			 *(m_ptrdw1 + 1) |= 0x00000001;
		 }
		 *m_ptrdw1 = m_dw1 << 1;
	 }	 
	}
	else
	{
	 
	}
}

/* -- Scroll left lines in virtual pixel memory
	argYMin: minimal logical y position (line) in virtual display memory,
	argYMax: maximal logical y position (line) in virtual display memory,
-- */
void ZMaxLEDMatrix::ScrollLeftVirtualLines(int argYMin, int argYMax)
{
	if((argYMin < m_yMin) || (argYMax > m_yMax) || (argYMin > argYMax))
		return;
	
	for(m_y2 = argYMin; m_y2 <= argYMax; m_y2++)
	{
		ScrollLeftVirtualLine(m_y2);
	}
}

/* -- Scroll left lines in virtual pixel memory
	argYMin: minimal logical y position (line) in virtual display memory,
	argYMax: maximal logical y position (line) in virtual display memory,
-- */
void ZMaxLEDMatrix::ScrollRightVirtualLines(int argYMin, int argYMax)
{
	if((argYMin < m_yMin) || (argYMax > m_yMax) || (argYMin > argYMax))
		return;
	
	for(m_y2 = argYMin; m_y2 <= argYMax; m_y2++)
	{
		ScrollRightVirtualLine(m_y2);
	}
}

/* -- Invert a line in virtual pixel memory
	argY: logical y position (line) in virtual display memory,
-- */		
void ZMaxLEDMatrix::InvertVirtualLine(int argY)
{
	if((argY < m_yMin) || (argY > m_yMax))
		return;
	
	m_offset = (m_virtualNodesHorizontal * argY);
	
	if ((m_virtualNodesHorizontal % 4)	== 0)
	{
	 /* -- I can use 32bit long units -- */
	 m_i = m_virtualNodesHorizontal / 4;
	 m_ptrdw1 = (uint32_t *)(m_pixelData + m_offset);
	 for(m_idx = 0; m_idx < m_i; m_idx++)
	 {
		 m_dw1 = *m_ptrdw1;
		 *m_ptrdw1++ = m_dw1 ^ 0xffffffff;
	 }	 
	}
	else
	{
	 
	}	
}

/* -- Invert lines in virtual pixel memory
	argYMin: minimal logical y position (line) in virtual display memory,
	argYMax: maximal logical y position (line) in virtual display memory,
-- */		
void ZMaxLEDMatrix::InvertVirtualLines(int argYMin, int argYMax)
{
	if((argYMin < m_yMin) || (argYMax > m_yMax) || (argYMin > argYMax))
		return;
	
	for(m_y2 = argYMin; m_y2 <= argYMax; m_y2++)
	{
		InvertVirtualLine(m_y2);
	}	
}

