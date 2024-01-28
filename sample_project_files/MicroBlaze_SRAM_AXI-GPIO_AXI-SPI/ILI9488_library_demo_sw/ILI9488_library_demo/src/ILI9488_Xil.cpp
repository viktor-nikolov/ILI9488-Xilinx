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

#include "ILI9488_Xil.h"

#include <stdexcept>
#include "sleep.h"

/*** ILI9488 controller IC command codes ***/
#define ILI9488_NOP     0x00
#define ILI9488_SWRESET 0x01
#define ILI9488_RDDID   0x04
#define ILI9488_RDDST   0x09

#define ILI9488_SLPIN   0x10
#define ILI9488_SLPOUT  0x11
#define ILI9488_PTLON   0x12
#define ILI9488_NORON   0x13

#define ILI9488_RDMODE     0x0A
#define ILI9488_RDMADCTL   0x0B
#define ILI9488_RDPIXFMT   0x0C
#define ILI9488_RDIMGFMT   0x0D
#define ILI9488_RDSELFDIAG 0x0F

#define ILI9488_INVOFF   0x20
#define ILI9488_INVON    0x21
#define ILI9488_GAMMASET 0x26
#define ILI9488_DISPOFF  0x28
#define ILI9488_DISPON   0x29

#define ILI9488_CASET   0x2A
#define ILI9488_PASET   0x2B
#define ILI9488_RAMWR   0x2C
#define ILI9488_RAMRD   0x2E

#define ILI9488_PTLAR    0x30
#define ILI9488_VSCRDEF  0x33
#define ILI9488_MADCTL   0x36
#define ILI9488_VSCRSADD 0x37
#define ILI9488_PIXFMT   0x3A

#define ILI9488_FRMCTR1 0xB1
#define ILI9488_FRMCTR2 0xB2
#define ILI9488_FRMCTR3 0xB3
#define ILI9488_INVCTR  0xB4
#define ILI9488_DFUNCTR 0xB6

#define ILI9488_PWCTR1  0xC0
#define ILI9488_PWCTR2  0xC1
#define ILI9488_PWCTR3  0xC2
#define ILI9488_PWCTR4  0xC3
#define ILI9488_PWCTR5  0xC4
#define ILI9488_VMCTR1  0xC5
#define ILI9488_VMCTR2  0xC7

#define ILI9488_RDID1   0xDA
#define ILI9488_RDID2   0xDB
#define ILI9488_RDID3   0xDC
#define ILI9488_RDID4   0xDD

#define ILI9488_GMCTRP1 0xE0
#define ILI9488_GMCTRN1 0xE1


#if defined(ILI9488_SPI_PS) && defined(ILI9488_GPIO_PS)

void ILI9488::init( XSpiPs *spi, XGpioPs *gpio, u32 _RSTPin, u32 _DCPin ) {
	if( spi  == NULL )
		throw std::invalid_argument( "Argument 'spi' is NULL on calling ILI9488::init" );
	if( gpio == NULL )
		throw std::invalid_argument( "Argument 'gpio' is NULL on calling ILI9488::init" );

	SpiInstance  = spi;
	GpioInstance = gpio;
	RSTPin = _RSTPin;
	DCPin  = _DCPin;

	initDisplay();
} //init

#elif defined(ILI9488_SPI_PS) && defined(ILI9488_GPIO_AXI)

void ILI9488::init( XSpiPs *spi, XGpio *gpio, , u32 _RSTPin, u32 _DCPin, unsigned _GPIOChannel ) {
	if( spi  == NULL )
		throw std::invalid_argument( "Argument 'spi' is NULL on calling ILI9488::init" );
	if( gpio == NULL )
		throw std::invalid_argument( "Argument 'gpio' is NULL on calling ILI9488::init" );
	if( _GPIOChannel != 1 && _GPIOChannel != 2 )
		throw std::invalid_argument( "Argument '_GPIOChannel' must have value 1 or 2 on calling ILI9488::init" );

	SpiInstance  = spi;
	GpioInstance = gpio;
	RSTPin = _RSTPin;
	DCPin  = _DCPin;
	GPIOChannel = _GPIOChannel;

	initDisplay();
} //init

#elif defined(ILI9488_SPI_AXI) && defined(ILI9488_GPIO_PS)

void ILI9488::init( XSpi *spi, XGpioPs *gpio, u32 _RSTPin, u32 _DCPin ) {
	if( spi  == NULL )
		throw std::invalid_argument( "Argument 'spi' is NULL on calling ILI9488::init" );
	if( gpio == NULL )
		throw std::invalid_argument( "Argument 'gpio' is NULL on calling ILI9488::init" );
	if( spi->DataWidth != XSP_DATAWIDTH_BYTE )
		/* We must be working with the AXI Quad SPI of Transaction Width 8 bits
		 * because ILI9488 controller accepts data as bytes. */
		throw std::logic_error( "SPI instance provided to ILI9488::init does not have Transaction Width equal to 8bits" );
	if( !(XSpi_GetOptions(spi) & XSP_MASTER_OPTION) )
		throw std::logic_error( "SPI instance provided to ILI9488::init is not set as a master" );

	SpiInstance  = spi;
	GpioInstance = gpio;
	RSTPin = _RSTPin;
	DCPin  = _DCPin;

	/* Make sure that the SPI instance is started */
	if( SpiInstance->IsStarted != XIL_COMPONENT_IS_STARTED )
		XSpi_Start( SpiInstance );

	/* Cancel inhibition of the transmitter.
	 * We want transfer of data to start as soon as we write to the FIFO. */
	u32 ControlReg = XSpi_ReadReg(SpiInstance->BaseAddr, XSP_CR_OFFSET);
	ControlReg &= ~XSP_CR_TRANS_INHIBIT_MASK;
	XSpi_WriteReg(SpiInstance->BaseAddr, XSP_CR_OFFSET, ControlReg);

	initDisplay();
} //init

#elif defined(ILI9488_SPI_AXI) && defined(ILI9488_GPIO_AXI)

void ILI9488::init( XSpi *spi, XGpio *gpio, u32 _RSTPin, u32 _DCPin, unsigned _GPIOChannel ) {
	if( spi  == NULL )
		throw std::invalid_argument( "Argument 'spi' is NULL on calling ILI9488::init" );
	if( gpio == NULL )
		throw std::invalid_argument( "Argument 'gpio' is NULL on calling ILI9488::init" );
	if( _GPIOChannel != 1 && _GPIOChannel != 2 )
		throw std::invalid_argument( "Argument '_GPIOChannel' must have value 1 or 2 on calling ILI9488::init" );
	if( spi->DataWidth != XSP_DATAWIDTH_BYTE )
		/* We must be working with the AXI Quad SPI of Transaction Width 8 bits
		 * because ILI9488 controller accepts data as bytes. */
		throw std::logic_error( "SPI instance provided to ILI9488::init does not have Transaction Width equal to 8bits" );
	if( !(XSpi_GetOptions(spi) & XSP_MASTER_OPTION) )
		throw std::logic_error( "SPI instance provided to ILI9488::init is not set as a master" );

	SpiInstance  = spi;
	GpioInstance = gpio;
	RSTPin = _RSTPin;
	DCPin  = _DCPin;
	GPIOChannel = _GPIOChannel;

	/* Make sure that the SPI instance is started */
	if( SpiInstance->IsStarted != XIL_COMPONENT_IS_STARTED )
		XSpi_Start( SpiInstance );

	/* Cancel inhibition of the transmitter.
	 * We want transfer of data to start as soon as we write to the FIFO. */
	u32 ControlReg = XSpi_ReadReg(SpiInstance->BaseAddr, XSP_CR_OFFSET);
	ControlReg &= ~XSP_CR_TRANS_INHIBIT_MASK;
	XSpi_WriteReg(SpiInstance->BaseAddr, XSP_CR_OFFSET, ControlReg);

	initDisplay();
} //init

#endif //elif defined(ILI9488_SPI_AXI) && defined(ILI9488_GPIO_AXI)

void ILI9488::drawImage888( const uint8_t* img, uint16_t x, uint16_t y, uint16_t w, uint16_t h ) {
	setAddrWindow(x, y, x+w-1, y+h-1);
	setPinHigh( DCPin );

	/* The data passed as img are ready for writing to the display in 8:8:8 color coding,
	 * i.e., 3 bytes per pixel. However in case of bigger images, we can't write the data with
	 * a single call to writeToSPI (it would fail). We must split data transfer to the display into several
	 * writeToSPI calls.
	 */
	const uint32_t MAX_WRITE_BYTES = 480*100*3;
	uint32_t noOfBytes = 3 * w * h;
	uint8_t *writePtr = (uint8_t *)img;

	//first we do maximum size writes
	for( uint16_t i = 0; i < noOfBytes / MAX_WRITE_BYTES; i++, writePtr += MAX_WRITE_BYTES )
		writeToSPI( writePtr, MAX_WRITE_BYTES );
	//then we write remaining pixels
	if( noOfBytes % MAX_WRITE_BYTES != 0 )
		writeToSPI( writePtr, noOfBytes % MAX_WRITE_BYTES );
} //drawImage888

void ILI9488::drawImage565( const uint8_t* img, uint16_t x, uint16_t y, uint16_t w, uint16_t h ) {
	setAddrWindow(x, y, x+w-1, y+h-1);
	setPinHigh( DCPin );

	uint8_t linebuff[w*3];
	uint32_t count = 0;
	for( uint16_t i = 0; i < h; i++ ) {
		uint16_t pixcount = 0;
		for( uint16_t o = 0; o <  w; o++ ) {
			uint8_t b1 = img[count];
			count++;
			uint8_t b2 = img[count];
			count++;
			uint16_t color = b1 << 8 | b2;
			linebuff[pixcount] = (((color & 0xF800) >> 11)* 255) / 31;
			pixcount++;
			linebuff[pixcount] = (((color & 0x07E0) >> 5) * 255) / 63;
			pixcount++;
			linebuff[pixcount] = ((color & 0x001F)* 255) / 31;
			pixcount++;
		} // for row
		writeToSPI(linebuff, w*3);
	}// for column
} //drawImage565

void ILI9488::setScrollArea( uint16_t topFixedAreaLines, uint16_t bottomFixedAreaLines ){
	writeCommand(ILI9488_VSCRDEF); // Vertical scroll definition
	writeData(topFixedAreaLines >> 8);
	writeData(topFixedAreaLines);

	/* Scrolling is always done "along the long 480px edge" of the display
	   regardless of rotation setting */
	writeData((ILI9488_TFTHEIGHT - topFixedAreaLines - bottomFixedAreaLines) >> 8);
	writeData(ILI9488_TFTHEIGHT - topFixedAreaLines - bottomFixedAreaLines);

	writeData(bottomFixedAreaLines >> 8);
	writeData(bottomFixedAreaLines);
} //setScrollArea

void ILI9488::scroll( uint16_t pixels ){
	writeCommand(ILI9488_VSCRSADD); // Vertical scrolling start address
	writeData(pixels >> 8);
	writeData(pixels);
} //setScrollArea


void ILI9488::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height))
		return;

	setAddrWindow(x,y,x,y);
	setPinHigh( DCPin );
	write16BitColor( color );
} //drawPixel

void ILI9488::fillRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color ) {
	// rudimentary clipping (drawChar w/big text requires this)
	if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height))
		return;
	if((x + w - 1) >= _width)  w = _width  - x;
	if((y + h - 1) >= _height) h = _height - y;

	setAddrWindow(x, y, x+w-1, y+h-1);
	setPinHigh( DCPin );

    /* I did runtime speed measurements on AVNET MicroZed board, which has Zynq-7000 SoC.
     * It seems that sweet spot of buffer size for bigger rectangles is 480 pixels.
     * I guess having the buffer of size 1.4 kB (==480*3) is OK for Zynq-7000 SoC,
     * which typically come with plenty of RAM.
     *
     * Testing on MicroBlaze showed that optimal buffer size is 256 pixels.
     * However on MicroBlaze default stack size is just 1 kB.
     * For MicroBlaze with MIG (i.e., when DDR SDRAM is used), I set BUFFERED_PIXELS to 256
     * (i.e. buffer size 768 B).
     * For MicroBlaze without MIG (i.e., when FPGA block RAM is used), I set BUFFERED_PIXELS to only 32
     * (i.e. buffer size 96 B).
     *
     * COMPILER OPTIMIZATION DOES MATTER A LOT! Do compile the final product in Release Configuration.
     * Real measurements on Zynq-7000 Soc:
	 *   BUFFERED_PIXELS = 480
	 *   build debug, gcc flag -O0:   20.35 us duration of buffer fill
	 *   build release, gcc flag -O2:  2.55 us duration of buffer fill
	 *   build release, gcc flag -O3:  2.34 us duration of buffer fill
	 */
#if defined(__arm__)
	const unsigned BUFFERED_PIXELS = 480;       //Max. size of the buffer of pixels we write to SPI
#elif defined(__MICROBLAZE__)
	/* XPAR_XMIG7SERIES_NUM_INSTANCES is defined in xparameters.h when a MIG (usually for DDR3 SDRAM) is present in the HW design.
	 * XPAR_AXI_EMC is defined when an AXI EMC (External Memory Controller) is present in the HW design.
	 * In either case we are using an external RAM and can spare more memory for the pixel buffer.
	 * However, be aware that default stack size for MicroBlaze project is 1 kB. It can be increased in the lscript.ld.
	 */
	#if defined(XPAR_XMIG7SERIES_NUM_INSTANCES) || defined(XPAR_AXI_EMC)
		const unsigned BUFFERED_PIXELS =  256; //The buffer will consume 768 B (== 256*3)
	#else
		const unsigned BUFFERED_PIXELS =  32;  //We are probably using Block RAM and we keep the buffer small.
	#endif
#else
	#error "Both '__arm__' and '__MICROBLAZE__' macros are undefined. Make sure that a relevant one is defined."
#endif
	u8 buffer[BUFFERED_PIXELS * 3];

	//prepare value of the color
	u8 *r = buffer;
	u8 *g = buffer+1;
	u8 *b = buffer+2;
	*r = (color & 0xF800) >> 11;
	*g = (color & 0x07E0) >> 5;
	*b = color & 0x001F;
	*r = (*r * 255) / 31;
	*g = (*g * 255) / 63;
	*b = (*b * 255) / 31;

	//prepare buffer of pixels to be written to SPI as a batch
	uint32_t noOfPixels = w * h;
	uint16_t loopRange = BUFFERED_PIXELS < noOfPixels ? BUFFERED_PIXELS : noOfPixels;
	for( uint16_t i = 3; i < loopRange*3; i+=3) {
		buffer[i  ] = *r;
		buffer[i+1] = *g;
		buffer[i+2] = *b;
	}

	//write buffer to SPI
	//first we write full buffers
	for( uint16_t i = 0; i < noOfPixels / BUFFERED_PIXELS; i++)
		writeToSPI( buffer, sizeof(buffer) );
	//then we write remaining pixels
	if( noOfPixels % BUFFERED_PIXELS != 0 )
		writeToSPI( buffer, (noOfPixels % BUFFERED_PIXELS) * 3 );
} //fillRect

void ILI9488::setRotation(uint8_t rotation) {
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

	if( rotation > 3 )
		throw std::invalid_argument( "Argument 'rotation' has value >3 on calling ILI9488::setRotation" );

	writeCommand(ILI9488_MADCTL);
	switch (rotation) {
		case 0:
			writeData(MADCTL_MX | MADCTL_BGR);
			_width  = ILI9488_TFTWIDTH;
			_height = ILI9488_TFTHEIGHT;
			break;
		case 1:
			writeData(MADCTL_MV | MADCTL_BGR);
			_width  = ILI9488_TFTHEIGHT;
			_height = ILI9488_TFTWIDTH;
			break;
		case 2:
			writeData(MADCTL_MY | MADCTL_BGR);
			_width  = ILI9488_TFTWIDTH;
			_height = ILI9488_TFTHEIGHT;
			break;
		case 3:
			writeData(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
			_width  = ILI9488_TFTHEIGHT;
			_height = ILI9488_TFTWIDTH;
			break;
	}
} //setRotation

void ILI9488::invertDisplay(bool i) {
	writeCommand(i ? ILI9488_INVON : ILI9488_INVOFF);
}

inline __attribute__((always_inline)) void ILI9488::writeToSPI( u8 c ) {
#if defined(ILI9488_SPI_PS)
	XSpiPs_PolledTransfer( SpiInstance, &c, NULL, 1 );
#elif defined(ILI9488_SPI_AXI)
	writeToSPI( &c, 1 );
#endif
} //writeToSPI

#if defined(ILI9488_SPI_PS)
inline __attribute__((always_inline))
#endif
void ILI9488::writeToSPI( const u8 *buff, u32 len ) {
#if defined(ILI9488_SPI_PS)

	/* Note: On Zynq-7000 SoC PS SPI has FIFO of 128 bytes. */
	XSpiPs_PolledTransfer( SpiInstance, (u8*) buff, NULL, len );

#elif defined(ILI9488_SPI_AXI)
	/* We are using low-level functions to work with the AXI SPI because we have no benefit
	 * from using the more complex function XSpi_Transfer.
	 * We make a performance advantage from the fact that we are only writing to the SPI.
	 * We ignore any data that may come back.
	 *
	 * IMPORTANT: For this writeToSPI method to work, the code using the ILI9488 library must
	 * properly set the slave select register (i.e., enable the SPI slave) by calling for example:
	 *   XSpi_SetSlaveSelectReg(&SpiInstance, SpiInstance.SlaveSelectReg)
	 */

	u32 NumBytesSent = 0;
	u32 TransactionLen;
	while( NumBytesSent < len ) {
		/* Fill transmit FIFO to the whole FIFO capacity or with all remaining data. */
		if ( len - NumBytesSent > SpiInstance->FifosDepth && SpiInstance->FifosDepth != 0 ) {
			TransactionLen = SpiInstance->FifosDepth;
		}
		else if( SpiInstance->FifosDepth == 0 ) {
			TransactionLen = 1; //We are working with the AXI Quad SPI of Transaction Width 8 bits
			                    //because ILI9488 controller accepts data as bytes.
		} else { // "remaining bytes" <= SpiInstance->FifosDepth && FIFO exists
			TransactionLen = len - NumBytesSent;
		}

		/* SPI device is enabled so as soon as the data is written to the DTR/FIFO,
		 * it gets pushed out on to the lines. */
		for( u32 i = 0; i < TransactionLen; i++ )
			XSpi_WriteReg( SpiInstance->BaseAddr, XSP_DTR_OFFSET, buff[ NumBytesSent + i] );

		NumBytesSent += TransactionLen;

		/* Wait for the transmit FIFO to transition to empty.
		 * We also must make sure that transfer is finished before we exit this function.
		 */
		while( !(XSpi_ReadReg(SpiInstance->BaseAddr, XSP_SR_OFFSET) & XSP_SR_TX_EMPTY_MASK) );
	}
#endif //#if defined(ILI9488_SPI_AXI)
} //writeToSPI

inline __attribute__((always_inline)) void ILI9488::setPinHigh( u32 pin ) {
#if defined(ILI9488_GPIO_PS)
	XGpioPs_WritePin( GpioInstance, pin, 1 );
#elif defined(ILI9488_GPIO_AXI)
	XGpio_DiscreteSet( GpioInstance, GPIOChannel, pin );
#endif
} //setPinHigh

inline __attribute__((always_inline)) void ILI9488::setPinLow( u32 pin ) {
#if defined(ILI9488_GPIO_PS)
	XGpioPs_WritePin( GpioInstance, pin, 0 );
#elif defined(ILI9488_GPIO_AXI)
	XGpio_DiscreteClear( GpioInstance, GPIOChannel, pin );
#endif
} //setPinLow

void ILI9488::writeCommand( u8 c ) {
	//drive DC pin low
	setPinLow( DCPin );

	writeToSPI( c );
}

void ILI9488::writeData( u8 c ) {
	//drive DC pin high
	setPinHigh( DCPin );

	writeToSPI( c );
}

void ILI9488::setAddrWindow( uint16_t xstart, uint16_t ystart, uint16_t xend, uint16_t yend ) {
	writeCommand(ILI9488_CASET); // Column addr set

	setPinHigh( DCPin );

	u8 buffer[4];
	buffer[0] = xstart >> 8;
	buffer[1] = xstart & 0xFF;  // XSTART
	buffer[2] = xend >> 8;
	buffer[3] = xend & 0xFF;    // XEND
	writeToSPI( buffer, sizeof(buffer) );

	writeCommand(ILI9488_PASET); // Row addr set

	setPinHigh( DCPin );
	buffer[0] = ystart >> 8;
	buffer[1] = ystart & 0xff;  // YSTART
	buffer[2] = yend >> 8;
	buffer[3] = yend & 0xff;    // YEND
	writeToSPI( buffer, sizeof(buffer) );

	writeCommand(ILI9488_RAMWR); // write to RAM
} //setAddrWindow

void ILI9488::write16BitColor( uint16_t color ) {
	u8 buffer[3];
	u8 *r = buffer;
	u8 *g = buffer+1;
	u8 *b = buffer+2;
	*r = (color & 0xF800) >> 11;
	*g = (color & 0x07E0) >> 5;
	*b = color & 0x001F;

	*r = (*r * 255) / 31;
	*g = (*g * 255) / 63;
	*b = (*b * 255) / 31;

	writeToSPI( buffer, sizeof(buffer) );
} //write16BitColor

void ILI9488::initDisplay() {
	// toggle RST low to reset
	setPinHigh( RSTPin );
	usleep(5*1000);
	setPinLow( RSTPin );
	usleep(20*1000);
	setPinHigh( RSTPin );
	usleep(150*1000);

	writeCommand(0xE0); //PGAMCTRL(Positive Gamma Control)
	writeData(0x00);    //Set the gray scale voltage to adjust the gamma characteristics of the TFT panel.
	writeData(0x03);
	writeData(0x09);
	writeData(0x08);
	writeData(0x16);
	writeData(0x0A);
	writeData(0x3F);
	writeData(0x78);
	writeData(0x4C);
	writeData(0x09);
	writeData(0x0A);
	writeData(0x08);
	writeData(0x16);
	writeData(0x1A);
	writeData(0x0F);

	writeCommand(0XE1); //NGAMCTRL (Negative Gamma Control)
	writeData(0x00);    //Set the gray scale voltage to adjust the gamma characteristics of the TFT panel.
	writeData(0x16);
	writeData(0x19);
	writeData(0x03);
	writeData(0x0F);
	writeData(0x05);
	writeData(0x32);
	writeData(0x45);
	writeData(0x46);
	writeData(0x04);
	writeData(0x0E);
	writeData(0x0D);
	writeData(0x35);
	writeData(0x37);
	writeData(0x0F);

	writeCommand(0xC0);   //Power Control 1
	writeData(0x17);      //Set the VREG1OUT voltage for positive gamma
	writeData(0x15);      //Set the VREG2OUT voltage for negative gammas

	writeCommand(0xC1);   //Power Control 2
	writeData(0x41);      //VGH,VGL - Set the factor used in the step-up circuits.

	writeCommand(0xC5);   //VCOM Control
	writeData(0x00);      //NV memory is not programmed
	writeData(0x12);      //VCM_REG: Set the factor to generate VCOM voltage from the reference voltage VREG2OUT
	writeData(0x80);      //VCOM value from VCM_REG

	writeCommand(0x36);   //Memory Access Control
	writeData(0x48);      //0b01001000 = MX set (D6), BGR set (D3)

	writeCommand(0x3A);   //Interface Pixel Format
	writeData(0x66); 	  //18 bit

	writeCommand(0XB0);   //Interface Mode Control
	writeData(0x80);      //SDA_EN = 1, DIN/SDA pin is used for 3/4 wire serial interface and SDO pin is not used.

	writeCommand(0xB1);   //Frame Rate Control
	writeData(0xA0);      //60Hz
//	writeData(0xB0);      //68Hz
//	writeData(0xC0);      //78Hz

	writeCommand(0xB4);   //Display Inversion Control
	writeData(0x02);      //2-dot inversion

	writeCommand(0xB6);   //Display Function Control  RGB/MCU Interface Control
	writeData(0x02);      //PT=2, Source output in a non-display area is AGND
	writeData(0x02);      //ISC=2, Source,Gate scan direction

	writeCommand(0xE9);   //Set Image Function
	writeData(0x00);      //Disable 24 bit data

	writeCommand(0xF7);   //Adjust Control 3
	writeData(0xA9);
	writeData(0x51);
	writeData(0x2C);
	writeData(0x82);      //DSI write DCS command, use loose packet RGB 666

	writeCommand(ILI9488_SLPOUT);    //Exit Sleep
	usleep(120*1000);
	writeCommand(ILI9488_DISPON);    //Display on
} //initDisplay
