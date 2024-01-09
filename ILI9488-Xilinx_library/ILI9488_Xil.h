/*
This is the port of Jaret Burkett's ILI9488 Arduino library to AMD Xilinx SoCs.

I removed all Arduino-specific code, optimized SPI writes for the DMA capabilities
of Xilinx SPI libraries and did other modifications.

Tested on AMD Xilinx Zynq-7000 SoC and Artix-7 FPGA (using MicroBlaze CPU) with
following display: http://www.lcdwiki.com/3.5inch_SPI_Module_ILI9488_SKU:MSP3520

Published on GitHub: https://github.com/viktor-nikolov/ILI9488-Xilinx
I must also port the Adafruit_GFX library: https://github.com/adafruit/Adafruit-GFX-Library
Original ILI9488 Arduino library: https://github.com/jaretburkett/ILI9488

BSD 2-Clause License:

Copyright (c) 2023 Viktor Nikolov for porting to AMD Xilinx SoCs
Copyright (c) 2016 Jaret Burkett for original ILI9488 Arduino library
Copyright (c) 2012 Adafruit Industries for Adafruit_GFX library

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _ILI9488_XIL_H
#define _ILI9488_XIL_H

#include "Adafruit_GFX/Adafruit_GFX.h"

#include "ILI9488_Xil_setup.h" //user-defined header with settings specific for given application

#if defined(ILI9488_SPI_PS)
	#include "xspips.h"
#elif defined(ILI9488_SPI_AXI)
	#include "xspi.h"
#endif
#if defined(ILI9488_GPIO_PS)
	#include "xgpiops.h"
#elif defined(ILI9488_SPI_AXI)
	#include "xgpio.h"
#endif

// Display dimensions
#define ILI9488_TFTWIDTH  320
#define ILI9488_TFTHEIGHT 480

// Color definitions
#define ILI9488_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9488_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9488_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9488_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9488_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9488_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9488_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9488_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9488_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9488_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9488_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9488_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9488_RED         0xF800      /* 255,   0,   0 */
#define ILI9488_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9488_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9488_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9488_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9488_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9488_PINK        0xF81F

class ILI9488 : public Adafruit_GFX {
public:
	/****** Methods specific for the class ILI9488: ******/

	ILI9488() : Adafruit_GFX(ILI9488_TFTWIDTH, ILI9488_TFTHEIGHT) {};

	/* The method init must be called before any output can be drawn on the display.
	 * It initializes internal class attributes and configures the display.
	 *
	 * Important:
	 *   Arguments spi and gpio must point to initialized instance of
	 *   the SPI/GPIO driver, which is ready for use.
	 *
	 *   Arguments RSTPin and DCPin specify GPIO pins connected to RST/DC pins on the display.
	 *   - When EMIO GPIO of Zynq Processing Systems is used, then value of RSTPin/DCPin is the number
	 *     of EMIO pin used, i.e. the value passed to the function XGpioPs_WritePin.
	 *     The first EMIO pin has number 54
	 *   - When AXI GPIO IP is used, then value of RSTPin/DCPin is the mask of given pin, i.e. the
	 *     valued passed to functions XGpio_DiscreteSet/XGpio_DiscreteClear. GPIO pin represented
	 *     by bit 0 has the mask 0x01.
	 *
	 *  Argument gpioChannel is valid only for use with AXI GPIO IP, which can be configured to provide
	 *  two sets (channels) of GPIO signals, which are identified by gpioChannel value 1 or 2.
	 */
#if defined(ILI9488_SPI_PS) && defined(ILI9488_GPIO_PS)
	void init(   XSpiPs *spi, XGpioPs *gpio, u32 _RSTPin, u32 _DCPin );
#elif defined(ILI9488_SPI_PS) && defined(ILI9488_GPIO_AXI)
	void init(   XSpiPs *spi, XGpio   *gpio, u32 _RSTPin, u32 _DCPin, unsigned _GPIOChannel = 1 );
#elif defined(ILI9488_SPI_AXI) && defined(ILI9488_GPIO_PS)
	void init(   XSpi   *spi, XGpioPs *gpio, u32 _RSTPin, u32 _DCPin );
#elif defined(ILI9488_SPI_AXI) && defined(ILI9488_GPIO_AXI)
	void init(   XSpi   *spi, XGpio   *gpio, u32 _RSTPin, u32 _DCPin, unsigned _GPIOChannel = 1 );
#endif

	// Pass 8-bit (each) R,G,B, get back 16-bit packed color
	static inline __attribute__((always_inline)) uint16_t color565( uint8_t r, uint8_t g, uint8_t b ) {
		return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	}

	/* drawImage methods expect an array of consecutive image's pixels at pointer "img" starting
	 * with top left corner pixel [0,0], going horizontally along the x axis.
	 * drawImage888 works with pixels in color coding R:G:B 8b:8b:8b (i.e., 3 bytes per pixel).
	 * drawImage565 works with pixels in color coding R:G:B 5b:6b:5b (i.e., 2 bytes per pixel).
	 *
	 * In the library's GitHub repository I provided two Python scripts (for both color codings), which
	 * read an image file and write to the standard output definition of a constant in C++
	 * (an array of bytes) usable as input to the drawImage888 or drawImage565. See:
	 * https://github.com/viktor-nikolov/ILI9488-Xilinx/tree/main/image_to_source_code_conversion
	 */
	void drawImage888( const uint8_t* img, uint16_t x, uint16_t y, uint16_t w, uint16_t h );
	void drawImage565( const uint8_t* img, uint16_t x, uint16_t y, uint16_t w, uint16_t h );

	/* Scrolling is always done along the long 480px edge of the display, regardless of rotation setting.
	 *
	 * The method setScrollArea() defines the scrolling area by setting how many lines on top and bottom
	 * of the display (in the portrait mode) are stationary.
	 * In order to scroll the whole display, call setScrollArea(0,0) or don't call it at all (the default
	 * setting is that the whole display is scrolled).
	 *
	 * The method scroll() does the actual scrolling by setting address (i.e. line's number) of the line in the display's
	 * Frame Memory to be displayed as the last line above Bottom Fixed Area. Beware that this address is relative
	 * to the whole display (not just to the scrolling area) and is indexed BACKWARD (i.e. the last line at the bottom
	 * of the display has address 0).
	 * E.g., by calling 'setScrollArea(0,0); scroll(10);', the graphics on the display will seem to scroll down by 10 pixels,
	 * however the 10 bottom pixels will appear on top of the display.
	 * If you want to scroll graphics by another 10 pixels, you must call 'scroll(20)'.
	 */
    void setScrollArea( uint16_t topFixedAreaLines, uint16_t bottomFixedAreaLines );
    void scroll( uint16_t pixels );

	/****** Methods overridden from Adafruit_GFX: ******/

	void drawPixel( int16_t x, int16_t y, uint16_t color );
	void fillRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color );
	void setRotation(uint8_t rotation);
	void invertDisplay(bool i);

	void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
		fillRect( x, y, 1, h, color );
    }
	void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
		fillRect( x, y, w, 1, color );
    }

protected:

#if defined(ILI9488_SPI_PS)
	XSpiPs *SpiInstance = NULL;
#elif defined(ILI9488_SPI_AXI)
	XSpi   *SpiInstance = NULL;
#endif
#if defined(ILI9488_GPIO_PS)
	XGpioPs *GpioInstance = NULL;
#elif defined(ILI9488_GPIO_AXI)
	XGpio   *GpioInstance = NULL;
	unsigned  GPIOChannel = 0;
#endif

	u32 RSTPin = 0, DCPin = 0;

	inline __attribute__((always_inline)) void writeToSPI( u8 c );

#if defined(ILI9488_SPI_PS)
	inline __attribute__((always_inline))
#endif
	void writeToSPI( const u8 *buff, u32 len );

	inline __attribute__((always_inline)) void setPinHigh( u32 pin );
	inline __attribute__((always_inline)) void setPinLow( u32 pin );

	void writeCommand( u8 c );
	void writeData( u8 c );

	void setAddrWindow( uint16_t xstart, uint16_t ystart, uint16_t xend, uint16_t yend );
	void write16BitColor( uint16_t color );
	void initDisplay();
};

#endif // _ILI9488_XIL_H
