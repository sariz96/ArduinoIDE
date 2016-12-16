#ifndef ZMaxLEDMatrix_h
#define ZMaxLEDMatrix_h

/* -- Zoltan Sari
   -- LED matrix of Max7219 controlled 8x8 modules (Arduino UNO / Nano)
   -- Library was created: 2016.10.15.
   --
 *    Thaks for that source:   
 *    LedControl.h - A library for controling Leds with a MAX7219/MAX7221
 *    Copyright (c) 2007 Eberhard Fahle
 *
	-- 2016.10.30.
	-- I changed the logical storing structure of pixels: first rows, second row ...
	
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

#include "ZMaxLEDFont.h"

#ifndef MY_MAXIMUM_NUMBER_OF_DEVICES
#define MY_MAXIMUM_NUMBER_OF_DEVICES 24
#endif	// MY_MAXIMUM_NUMBER_OF_DEVICES

#ifndef MY_MAXIMUM_NUMBER_OF_VIRTUAL_DEVICES
#define MY_MAXIMUM_NUMBER_OF_VIRTUAL_DEVICES 248
#endif	// MY_MAXIMUM_NUMBER_OF_VIRTUAL_DEVICES

#ifndef MY_MAXIMUM_NUMBER_OF_VERTICAL_NODES
#define MY_MAXIMUM_NUMBER_OF_VERTICAL_NODES 2
#endif	// MY_MAXIMUM_NUMBER_OF_VERTICAL_NODES

class ZMaxLEDMatrix
{
  public:
        /* 
         * Create a new controler 
         * Params :
         * argdataPin		pin on the Arduino where data gets shifted out
         * argclkPin		pin for the clock
         * argcsPin			pin for selecting the device 
         * argnumDevices	maximum number of devices that can be controled
		 * argNodesHorizontal	number of physical horizontal nodes
		 * argNodesVertical		number of physical vertical nodes
         */
        ZMaxLEDMatrix(int argdataPin, int argclkPin, int argcsPin, int argnumDevices,
			int argNodesHorizontal, int argNodesVertical);
        /* 
         * Create a new controler 
         * Params :
         * argdataPin		pin on the Arduino where data gets shifted out
         * argclkPin		pin for the clock
         * argcsPin			pin for selecting the device 
         * argnumDevices	maximum number of devices that can be controled
		 * argNodesHorizontal	number of physical horizontal nodes
		 * argNodesVertical		number of physical vertical nodes
		 * argVirtualNodesHorizontal	number of virtual horizontal nodes (argVirtualNodesHorizontal >= argNodesHorizontal)
		 * argVirtualNodesVertical		number of virtual vertical nodes (argVirtualNodesVertical >= argNodesVertical)
         */
        ZMaxLEDMatrix(int argdataPin, int argclkPin, int argcsPin, int argnumDevices,
			int argNodesHorizontal, int argNodesVertical,
			int argVirtualNodesHorizontal, int argVirtualNodesVertical );
			
		/* -- Initialize member data buffers, etc. -- */
		void Init();
		
        /*
         * Gets the number of devices attached to this ZMaxLEDMatrix.
         * Returns :
         * int	the number of devices on this ZMaxLEDMatrix
         */
        int GetDeviceCount();
        /* 
         * Set the shutdown (power saving) mode for the device
         * Params :
         * addr	The address of the display to control
         * status	If true the device goes into power-down mode. Set to false
         *		for normal operation.
         */
        void Shutdown(int argaddr, bool argstatus);
        void ShutdownEach(bool argstatus);

        /* 
         * Set the number of digits (or rows) to be displayed.
         * See datasheet for sideeffects of the scanlimit on the brightness
         * of the display.
         * Params :
         * addr	address of the display to control
         * limit	number of digits to be displayed (1..8)
         */
        void SetScanLimit(int argaddr, int arglimit);
        void SetScanLimitEach(int arglimit);

        /* 
         * Set the brightness of the display.
         * Params:
         * addr		the address of the display to control
         * intensity	the brightness of the display. (0..15)
         */
        void SetIntensity(int argaddr, int argintensity);
        void SetIntensityEach(int argintensity);

        /* 
         * Switch all Leds on the display off. 
         * Params:
         * addr	address of the display to control
         */
        void ClearDisplay(int argaddr);
        void ClearDisplayEach();

        /* 
         * Test each display
         * Params:
         * argTest: 1 - test
         */
        void DisplayTestEach(bool argTest);

        /* 
         * Set the status of a single Led.
         * Params :
         * addr	address of the display 
         * row	the row of the Led (0..7)
         * col	the column of the Led (0..7)
         * state	If true the led is switched on, 
         *		if false it is switched off
         */
        void SetLed(int argaddr, int argrow, int argcol, boolean argState);

        /* 
         * Set all 8 Led's in a row to a new state
         * Params:
         * addr	address of the display
         * row	row which is to be set (0..7)
         * value	each bit set to 1 will light up the
         *		corresponding Led.
         */
        void SetRow(int argaddr, int argrow, byte argvalue);
		
        /* 
         * Set all 8 Led's in a column to a new state
         * Params:
         * addr	address of the display
         * col	column which is to be set (0..7)
         * value	each bit set to 1 will light up the
         *		corresponding Led.
         */
        void SetColumn(int argaddr, int argcol, byte argvalue);
		
		/* -- Set the pixel with the logical coordinates  -- */
		void SetPixel(int argX, int argY, boolean argState);

		/* -- Get the pixel state with the logical coordinates  -- */
		boolean GetPixel(int argX, int argY);
		
		int GetMinX();
		int GetMaxX();
		int GetMinY();
		int GetMaxY();
		
		int GetDisplayMinX();
		int GetDisplayMaxX();
		int GetDisplayMinY();
		int GetDisplayMaxY();

		int GetDisplayOffsetX();
		int GetDisplayOffsetY();
		bool SetDisplayOffset(int argOffsetX, int argOffsetY);
		
		int GetVirtualNodesHorizontal();
		int GetVirtualNodesVertical();
		int GetDisplayNodesHorizontal();
		int GetDisplayNodesVertical();
		
		int GetPixelDataLen();
		
		/* -- Send the modified state to display -- */
		bool Display();
		
		void ClearCharPos1(int argRow, int argPos);
		void PrintCharPos1(int argRow, int argPos, byte* argCharBits);
		/* ---
			argX: logical x position of the char,
			argY: logical y position of the char,
			argCharWidth: width of the char in pixels
			argCharBits: char bitmap (8 lines)
		-- */
		void PrintCharPos2(int argX, int argY, int argCharWidth, byte* argCharBits);

		/* ---
			argX: logical x position of the first char,
			argY: logical y position of the first char,
			argCharWidth: width of the char in pixels
			argCharBits: char bitmap (8 lines)
		-- */
		int PrintTxt(int argX, int argY, char *argTxt, int argMaxLength, ZMaxLEDFontBase *argFont);
		int PrintStr(int argX, int argY, String argTxt, int argMaxLength, ZMaxLEDFontBase *argFont);

		/* -- Scroll left a line in virtual pixel memory
			argY: logical y position (line) in virtual display memory,
		-- */
		void ScrollLeftVirtualLine(int argY);

		/* -- Scroll left lines in virtual pixel memory
			argYMin: minimal logical y position (line) in virtual display memory,
			argYMax: maximal logical y position (line) in virtual display memory,
		-- */
		void ScrollLeftVirtualLines(int argYMin, int argYMax);
		
		/* -- Scroll right a line in virtual pixel memory
			argY: logical y position (line) in virtual display memory,
		-- */		
		void ScrollRightVirtualLine(int argY);

		/* -- Scroll left lines in virtual pixel memory
			argYMin: minimal logical y position (line) in virtual display memory,
			argYMax: maximal logical y position (line) in virtual display memory,
		-- */		
		void ScrollRightVirtualLines(int argYMin, int argYMax);
		
		/* -- Invert a line in virtual pixel memory
			argY: logical y position (line) in virtual display memory,
		-- */		
		void InvertVirtualLine(int argY);

		/* -- Invert lines in virtual pixel memory
			argYMin: minimal logical y position (line) in virtual display memory,
			argYMax: maximal logical y position (line) in virtual display memory,
		-- */		
		void InvertVirtualLines(int argYMin, int argYMax);
		
		/* -- Draw a vertical line in virtual pixel memory
		    argX:    logical x position (line) in virtual display memory,
			argYMin: minimal logical y position (line) in virtual display memory,
			argYMax: maximal logical y position (line) in virtual display memory,
			argState: set or clear pixel
		-- */		
		void DrawVerticalLine(int argX, int argYMin, int argYMax, boolean argState);

		/* -- Draw a vertical line in virtual pixel memory
		    argX:    logical x position (line) in virtual display memory,
			argYMin: minimal logical y position (line) in virtual display memory,
			argYMax: maximal logical y position (line) in virtual display memory,
			argWidth: width in pixels,
			argState: set or clear pixel
		-- */		
		void DrawVerticalLineWidth(int argX, int argYMin, int argYMax, int argWidth, boolean argState);

		/* -- Draw a vertical line in virtual pixel memory (argX % 8 = 0)
		    argX:    logical x position (line) in virtual display memory (argWidth <= 32),
			argYMin: minimal logical y position (line) in virtual display memory,
			argYMax: maximal logical y position (line) in virtual display memory,
			argWidth: width in pixels,
			argState: set or clear pixel
		-- */		
		void DrawVerticalLineWidthGrid0(int argX, int argYMin, int argYMax, int argWidth, boolean argState);
		
  private:
        int m_SPI_MOSI;
        /* The clock is signaled on this pin */
        int m_SPI_CLK;
        /* This one is driven LOW for chip selectzion */
        int m_SPI_CS;
        /* The maximum number of devices we use */
        int m_maxDevices;
        /* The array for shifting the data to the devices */
        byte m_spiData[MY_MAXIMUM_NUMBER_OF_DEVICES * 2];
		/* -- The array for pixel state of each node. The bytes in logical row data format. -- */
		byte m_pixelData[(MY_MAXIMUM_NUMBER_OF_VIRTUAL_DEVICES * 8)];
		/* -- Size of actual buffer in bytes -- */
		int m_pixelDataLen;
		/* -- How many nodes (8x8 matrix node) are there vertical -- */
		int m_nodesVertical;
		/* -- -- */
		/* -- How many nodes (8x8 matrix node) are there horizontal -- */
		int m_nodesHorizontal;
		/* -- How many virtual nodes (8x8 matrix node) are there vertical -- */
		int m_virtualNodesVertical;
		/* -- -- */
		/* -- How many virtual nodes (8x8 matrix node) are there horizontal -- */
		int m_virtualNodesHorizontal;

		/* -- Actual display offset x coordinate -- */
		int m_displayOffsetX;

		/* -- Actual display offset y coordinate -- */
		int m_displayOffsetY;
		
		/* -- local variables -- */
		int m_offset;
		int m_x;
		int m_xMin;
		int m_xMax;
		int m_yMin;
		int m_yMax;
		int m_i;
		int m_j;
		int m_k;
		int m_x1;
		int m_x2;
		int m_xPrint1;
		int m_y1;
		int m_y2;
		int m_y3;
		int m_yPrint1;
		int m_idx;
		int m_widthPrint1;
		int m_idxPrint1;
		byte m_bval;
		byte m_bi;
		char m_ch01;
		unsigned int m_w;
		uint32_t m_dw1;
		uint32_t m_dw2;
		uint32_t *m_ptrdw1;
		uint32_t *m_ptrdw2;
		byte* m_PrintCharBits1;
		char m_chBuffer1[256];
		
        /* Send out a single command to the device */
        void SpiTransfer(int argaddr, byte argopcode, byte argdata);
		
        /* Send out a single command each of the device */
        void SpiTransferEach(byte argopcode, byte argdata);

        /* Send out all pixels to each of the device */
        void SpiTransferPixelsEach();

};


#endif
