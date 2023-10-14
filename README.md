# ILI9488 TFT display library for Xilinx FPGA and SoC

This is the port of Jaret Burkett's [ILI9488 Arduino library](https://github.com/jaretburkett/ILI9488) to AMD Xilinx FPGA and SoC.

I removed all Arduino-specific code, optimized SPI writes for the DMA capabilities
of Xilinx SPI libraries and did other modifications.

I must also port the [Adafruit_GFX library](https://github.com/adafruit/Adafruit-GFX-Library).

Tested on AMD Xilinx Zynq-7000 SoC and Artix-7 FPGA (using MicroBlaze CPU) with
the following display: [3.5" SPI Module ILI9488 SKU:MSP3520](http://www.lcdwiki.com/3.5inch_SPI_Module_ILI9488_SKU:MSP3520) (can be purchased [on Amazon](https://www.amazon.com/Hosyond-Display-Compatible-Mega2560-Development/dp/B0BWJHK4M6/ref=sr_1_1?crid=1JH7HIUGZU29J&keywords=3%2C5%22%2B480x320%2BSPI%2BTFT%2BILI9488&qid=1691653179&sprefix=3%2C5%2B480x320%2Bspi%2Btft%2Bili9488%2Caps%2C212&sr=8-1&th=1)).

Uses a port of Adafruit_GFX library: https://github.com/adafruit/Adafruit-GFX-Library
