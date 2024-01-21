/*
This is a user-defined header, which specifies ILI9488_Xil library settings
for given application.

YOU NEED TO REVIEW AND MODIFY SETTINGS BELOW TO SUIT THE HW DESIGN USED.

Most importantly, the type of SPI and GPIO used needs to be configured.
There are following options on Xilinx Zynq SoC and FPGA:

SPI:
1. SPI of Zynq Processing Systems is used.
2. AXI Quad SPI IP is used.

GPIO:
1. EMIO GPIO of Zynq Processing Systems is used.
2. AXI GPIO IP is used.

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


#ifndef _ILI9488_XIL_setup_H
#define _ILI9488_XIL_setup_H

/**** select one of the SPI types used by given application ****/
//#define ILI9488_SPI_PS  //SPI of Zynq Processing Systems is used.
#define ILI9488_SPI_AXI //AXI Quad SPI IP is used.

/**** select one of the GPIO types used by given application ****/
//#define ILI9488_GPIO_PS  //EMIO GPIO of Zynq Processing Systems is used.
#define ILI9488_GPIO_AXI //AXI GPIO IP is used.


//checking consistency of the settings:
#if !defined(ILI9488_SPI_PS) && !defined(ILI9488_SPI_AXI)
	#error "You must define one of the macros ILI9488_SPI_PS and ILI9488_SPI_AXI in ILI9488_Xil_setup.h"
#endif
#if defined(ILI9488_SPI_PS) && defined(ILI9488_SPI_AXI)
	#error "Only one of the macros ILI9488_SPI_PS and ILI9488_SPI_AXI can be defined in ILI9488_Xil_setup.h"
#endif
#if !defined(ILI9488_GPIO_PS) && !defined(ILI9488_GPIO_AXI)
	#error "You must define one of the macros ILI9488_GPIO_PS and ILI9488_GPIO_AXI in ILI9488_Xil_setup.h"
#endif
#if defined(ILI9488_GPIO_PS) && defined(ILI9488_GPIO_AXI)
	#error "Only one of the macros ILI9488_GPIO_PS and ILI9488_GPIO_AXI can be defined in ILI9488_Xil_setup.h"
#endif

#ifdef __MICROBLAZE__
	/* When AXI Timer is not used, the functions sleep and usleep are implemented as a loop of instructions of known duration.
	 * This doesn't work on a MicroBlaze without instruction and data cache, because the loop takes way longer than expected.
	 * That is a problem for the ILI9488 library because ILI9488::init requires use of usleep and therefore call to ILI9488::init
	 * would take a long time without caches (it may seem to the user that the program crashed on start).
	 */
	#include "xparameters.h"

	#if !defined(XPAR_MICROBLAZE_USE_ICACHE) || !defined(XPAR_MICROBLAZE_USE_DCACHE)
	//Macros XPAR_MICROBLAZE_USE_ICACHE/DCACHE are defined in xparameters.h when MicroBlaze has an instruction/data cache configured
		#ifndef XSLEEP_TIMER_IS_AXI_TIMER //Macro XSLEEP_TIMER_IS_AXI_TIMER is defined in xparameters.h when AXI Timer is used in the HW design
			#error "Functions sleep and usleep don't work correctly on MicroBlaze without cache when AXI Timer is not used."
			#error "Method ILI9488::init requires use of usleep."
		#endif
	#endif
#endif //ifdef __MICROBLAZE__

#endif // _ILI9488_XIL_setup_H
