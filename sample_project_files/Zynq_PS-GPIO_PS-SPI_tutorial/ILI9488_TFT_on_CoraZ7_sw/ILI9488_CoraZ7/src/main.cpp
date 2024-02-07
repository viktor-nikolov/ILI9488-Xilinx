/*
This is the test and demo program for ILI9488 Arduino library port to AMD Xilinx SoC and FPGA.
Details published on https://github.com/viktor-nikolov/ILI9488-Xilinx

This file is part of the tutorial
https://github.com/viktor-nikolov/ILI9488-Xilinx/tree/main/sample_project_files/Zynq_PS-GPIO_PS-SPI_tutorial

Tested on AMD Xilinx Zynq-7000 SoC with following display:
http://www.lcdwiki.com/3.5inch_SPI_Module_ILI9488_SKU:MSP3520

BSD 2-Clause License:

Copyright (c) 2024 Viktor Nikolov

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

#include <vector>
#include "xil_printf.h"
#include "xil_cache.h"
#include "sleep.h"

#include "ILI9488_Xil.h"
#include "Adafruit_GFX/Fonts/FreeSansBold24pt7b.h" //needed for testTextBigFont()
#include "demo_image1.h"
#include "demo_image2.h"

/* Define numbers of EMIO GPIO pins to which are the RST and DC pins of ILI9488 display connected.
 * The first EMIO pin has number 54.
 */
#define ILI9488_RST_PIN  54 //== EMIO pin 0
#define ILI9488_DC_PIN   55 //== EMIO pin 1

XGpioPs GpioInstance;
XSpiPs SpiInstance;

int initialize_PS_GPIO() {
	XGpioPs_Config *GpioConfig;
    int Status;

	GpioConfig = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	if(GpioConfig == NULL) {
		print("XGpioPs_LookupConfig failed\r\n");
		return XST_FAILURE;
	}

	Status = XGpioPs_CfgInitialize(&GpioInstance, GpioConfig, GpioConfig->BaseAddr);
	if(Status != XST_SUCCESS) {
		print("XGpioPs_CfgInitialize failed\r\n");
		return XST_FAILURE;
	}

	//initialize RST and DC pins and drive them low
	std::vector<int> outputPins = { ILI9488_RST_PIN, ILI9488_DC_PIN };
	for( auto p : outputPins ) {
		XGpioPs_SetDirectionPin(&GpioInstance, p, 1 /*output*/);
		XGpioPs_SetOutputEnablePin(&GpioInstance, p, 1 /*enable*/);
		XGpioPs_WritePin( &GpioInstance, p, 0 /*low*/ );
	}
	return 0;
} //initialize_PS_GPIO

int initialize_PS_SPI() {
	XSpiPs_Config *SpiConfig;
    int Status;

	SpiConfig = XSpiPs_LookupConfig(XPAR_PS7_SPI_0_DEVICE_ID);
	if(SpiConfig == NULL) {
		print("XSpiPs_LookupConfig failed\r\n");
		return XST_FAILURE;
	}

	Status = XSpiPs_CfgInitialize(&SpiInstance, SpiConfig, SpiConfig->BaseAddress);
	if(Status != XST_SUCCESS) {
		print("XSpiPs_CfgInitialize failed\r\n");
		return XST_FAILURE;
	}

	Status = XSpiPs_SelfTest(&SpiInstance);
	if(Status != XST_SUCCESS) {
		print("XSpiPs_SelfTest failed\r\n");
		return XST_FAILURE;
	}

	/* Set the SPI interface as Master.
	 * Set Force Slave Select option: The SPI_SS_outN signal indicated by the Slave Select Control bit is forced active (driven low)
	 * regardless of any transfers in progress.
	 */
	Status = XSpiPs_SetOptions(&SpiInstance, XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION);
	if(Status != XST_SUCCESS) {
		print("XSpiPs_SetOptions failed\r\n");
		return XST_FAILURE;
	}

	/* Select Slave 0 */
	Status = XSpiPs_SetSlaveSelect(&SpiInstance, 0);
	if(Status != XST_SUCCESS) {
		print("XSpiPs_SetSlaveSelect failed\r\n");
		return XST_FAILURE;
	}

	/* Setting SCK frequency for the PS SPI:
	 *
	 * ZYNQ7 Processing System default SPI clock is 166.666666 MHz
	 * Using XSPIPS_CLK_PRESCALE_16 -> SCK frequency 10.42 MHz = cycle duration 96 ns, this is unnecessarily slow
	 * Using XSPIPS_CLK_PRESCALE_8  -> SCK frequency 20.83 MHz = cycle duration 48 ns
	 *
	 * According to ILI9488 datasheet minimal SCK cycle for write operations is 50 ns (20 MHz).
	 * However my specimen worked well with SCK cycle duration 48 ns (20.83 MHz).
	 *
	 * SPI clock can be lowered to 150 MHz in the ZYNQ7 Processing System IP configuration,
	 * which gives SCK frequency 18.75 MHz. That is well within specification of ILI9488.
	 * You can do this when having issues on SCK 20.83 MHz.
	 */
	Status = XSpiPs_SetClkPrescaler(&SpiInstance, XSPIPS_CLK_PRESCALE_8);
	if(Status != XST_SUCCESS) {
		print("XSpiPs_SetClkPrescaler failed\r\n");
		return XST_FAILURE;
	}

	return 0;
} //initialize_PS_SPI

void testFillScreen( ILI9488 &tft ) {
	tft.fillScreen(ILI9488_BLACK); usleep( 300*1000 );
	tft.fillScreen(ILI9488_RED);   usleep( 300*1000 );
	tft.fillScreen(ILI9488_GREEN); usleep( 300*1000 );
	tft.fillScreen(ILI9488_BLUE);  usleep( 300*1000 );
	tft.fillScreen(ILI9488_BLACK); usleep( 300*1000 );
} //testFillScreen

void testImages(ILI9488 &tft) {
	for( int i = 0; i < tft.width() / Sun_png_width; i++ )
		for( int j = 0; j < tft.height() / Sun_png_height; j++ )
			tft.drawImage888( Sun_png_image888, i * Sun_png_width, j * Sun_png_height, Sun_png_width, Sun_png_height );

	sleep( 3 );
	tft.drawImage565( PragueCastle_png_image565, 0, 0, PragueCastle_png_width,  PragueCastle_png_height );
} //testImages

void testScroll(ILI9488 &tft) {
    tft.setRotation( 2 ); //scrolling works only along the long 480px edge
	tft.fillRect( 0, 0, 320, 20, ILI9488_BLUE);
	tft.fillRect( 0, 480-20, 320, 20, ILI9488_BLUE);

	tft.setScrollArea( 20, 20 );

	int i,j;
    uint8_t colVal = 255;
	uint16_t color1, color2;

	for( i = 1; i < (480-20) / 20; i++ ) {
		if( i % 2 ) {
			color1 = ILI9488::color565( colVal, 0, 0 );
			color2 = ILI9488::color565( 0, colVal, 0 );
		}
		else {
			color1 = ILI9488::color565( 0, colVal, 0 );
			color2 = ILI9488::color565( colVal, 0, 0 );
		}
		colVal += ( i < 12 ? -1 : 1 ) * 13;

		for( j = 0; j <  320 / 20; j += 2 ) {
			tft.fillRect( j*20,     i*20, 20, 20, color1 );
			tft.fillRect( (j+1)*20, i*20, 20, 20, color2 );
		}
	}

	for( i = 0; i < 2; i++ )
		for( j = 20; j <= 460; j++  ) {
			tft.scroll( j );
			usleep( 10*1000 );
		}
} //testScroll

void testText( ILI9488 &tft ) {
	tft.setFont(); //set default font
	tft.fillScreen(ILI9488_BLACK);
	tft.setCursor(0, 0);
	tft.setTextColor(ILI9488_WHITE);  tft.setTextSize(1);
	tft.println("Hello World!");
	tft.setTextColor(ILI9488_YELLOW); tft.setTextSize(2);
	tft.println(1234.56);
	tft.setTextColor(ILI9488_RED);    tft.setTextSize(3);
	tft.println(0xDEADBEEF, HEX);
	tft.println();
	tft.setTextColor(ILI9488_GREEN);
	tft.setTextSize(5);
	tft.println("Groop");
	tft.setTextSize(2);
	tft.println("I implore thee,");
	tft.setTextSize(1);
	tft.println("my foonting turlingdromes.");
	tft.println("And hooptiously drangle me");
	tft.println("with crinkly bindlewurdles,");
	tft.println("Or I will rend thee");
	tft.println("in the gobberwarts");
	tft.println("with my blurglecruncheon,");
	tft.println("see if I don't!");
} //testText

void testTextBigFont( ILI9488 &tft ) {
    tft.fillScreen( ILI9488_BLACK );

    tft.setFont( &FreeSansBold24pt7b ); //The font is defined in "Adafruit_GFX/Fonts/FreeSansBold24pt7b.h"

    tft.setTextColor( ILI9488_YELLOW );
    tft.setCursor( 0, 37 );

    for( int i = 0; i < 6; i++ )
    	tft.println("Hello World!");
} //testText


void testLines( ILI9488 &tft, uint16_t color) {
	int x1, y1, x2, y2,
        w = tft.width(),
        h = tft.height();

	tft.fillScreen(ILI9488_BLACK);

	x1 = y1 = 0;
	y2 = h - 1;
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2 = w - 1;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

	tft.fillScreen(ILI9488_BLACK);

	x1 = w - 1;
	y1 = 0;
	y2 = h - 1;
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2 = 0;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

	tft.fillScreen(ILI9488_BLACK);

	x1 = 0;
	y1 = h - 1;
	y2 = 0;
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2 = w - 1;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

	tft.fillScreen(ILI9488_BLACK);

	x1 = w - 1;
	y1 = h - 1;
	y2 = 0;
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2 = 0;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
} //testLines

void testFastLines(ILI9488 &tft, uint16_t color1, uint16_t color2) {
	int x, y, w = tft.width(), h = tft.height();
	tft.fillScreen(ILI9488_BLACK);

	for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
	for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);
} //testFastLines

void testRects(ILI9488 &tft, uint16_t color) {
	int n, i, i2,
        cx = tft.width()  / 2,
        cy = tft.height() / 2;

	tft.fillScreen(ILI9488_BLACK);
	n = std::min(tft.width(), tft.height());
	for(i=2; i<n; i+=6) {
		i2 = i / 2;
		tft.drawRect(cx-i2, cy-i2, i, i, color);
	}
} //testRects

void testFilledRects(ILI9488 &tft, uint16_t color1, uint16_t color2) {
	int n, i, i2,
        cx = tft.width()  / 2 - 1,
        cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9488_BLACK);
	n = std::min(tft.width(), tft.height());
	for(i=n; i>0; i-=6) {
		i2    = i / 2;
		tft.fillRect(cx-i2, cy-i2, i, i, color1);
		tft.drawRect(cx-i2, cy-i2, i, i, color2);
	}
} //testFilledRects

void testCircles(ILI9488 &tft, uint8_t radius, uint16_t color) {
	int x, y, r2 = radius * 2,
        w = tft.width()  + radius,
        h = tft.height() + radius;

	tft.fillScreen(ILI9488_BLACK);
	for(x=0; x<w; x+=r2) {
		for(y=0; y<h; y+=r2) {
			tft.drawCircle(x, y, radius, color);
		}
	}
} //testCircles

void testFilledCircles(ILI9488 &tft, uint8_t radius, uint16_t color) {
	int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

	tft.fillScreen(ILI9488_BLACK);
	for(x=radius; x<w; x+=r2) {
		for(y=radius; y<h; y+=r2) {
			tft.fillCircle(x, y, radius, color);
		}
	}
} //testFilledCircles

void testTriangles(ILI9488 &tft) {
	int n, i, cx = tft.width()  / 2 - 1,
        cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9488_BLACK);
	n = std::min(cx, cy);
	for(i=0; i<n; i+=5) {
		tft.drawTriangle(
		  cx    , cy - i, // peak
		  cx - i, cy + i, // bottom left
		  cx + i, cy + i, // bottom right
		  ILI9488::color565(i, i, i));
	}
}

void testFilledTriangles(ILI9488 &tft) {
	int i, cx = tft.width()  / 2 - 1,
        cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9488_BLACK);
	for(i=std::min(cx,cy); i>10; i-=5) {
		tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                         ILI9488::color565(0, i*10, i*10));
		tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                         ILI9488::color565(i*10, i*10, 0));
	}
} //testFilledTriangles

void testRoundRects(ILI9488 &tft) {
	int w, i, i2,
        cx = tft.width()  / 2 - 1,
        cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9488_BLACK);
	w = std::min(tft.width(), tft.height());
	for(i=0; i<w; i+=6) {
		i2 = i / 2;
		tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, ILI9488::color565(i, 0, 0));
	}
} //testRoundRects

void testFilledRoundRects(ILI9488 &tft) {
	int i, i2,
        cx = tft.width()  / 2 - 1,
        cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9488_BLACK);
	for(i=std::min(tft.width(), tft.height()); i>20; i-=6) {
		i2 = i / 2;
		tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, ILI9488::color565(0, i, 0));
	}
} //testFilledRoundRects

int main()
{
	print("*** ILI9488 DEMO START ***\r\n");

    /*** Initialize SPI driver ***/
    if( initialize_PS_SPI()  != XST_SUCCESS )
    	return XST_FAILURE;

    /*** Initialize GPIO driver ***/
    if( initialize_PS_GPIO()  != XST_SUCCESS )
    	return XST_FAILURE;

    ILI9488 display;
    display.init( &SpiInstance, &GpioInstance, ILI9488_RST_PIN, ILI9488_DC_PIN );
    display.setRotation( 3 );

    while(1) {
    	testFillScreen( display );
    	testImages( display );
    	sleep( 3 );

    	testScroll( display );
    	display.setRotation( 3 ); //testScroll changed the rotation
    	sleep ( 1 );

    	testText( display );
        sleep ( 3 );

        display.invertDisplay( true );
        usleep( 1500*1000 );
        display.invertDisplay( false );
        sleep( 1 );

        testTextBigFont( display );
        usleep( 1500*1000 );

        testLines( display, ILI9488_YELLOW );
		sleep ( 1 );
		testFastLines( display, ILI9488_YELLOW, ILI9488_WHITE );
		sleep ( 1 );

		testRects( display, ILI9488_YELLOW );
		sleep ( 1 );
		testFilledRects( display, ILI9488_YELLOW, ILI9488_RED );
		sleep ( 1 );

		testCircles( display, 10, ILI9488_YELLOW );
    	sleep ( 1 );
    	testFilledCircles( display, 10, ILI9488_YELLOW );
    	sleep ( 1 );

    	testTriangles( display );
    	sleep ( 1 );
    	testFilledTriangles( display );
    	sleep ( 1 );

    	testRoundRects( display );
    	sleep ( 1 );
    	testFilledRoundRects( display );
    	sleep( 1 );
    }

    print("graceful exit\r\n"); //never reached
    return 0;
} //main


