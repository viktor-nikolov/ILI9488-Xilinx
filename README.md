# ILI9488 TFT display library for Xilinx FPGA and SoC

This is the port of Jaret Burkett's [ILI9488 Arduino library](https://github.com/jaretburkett/ILI9488) to AMD Xilinx FPGA and SoC.

I removed all Arduino-specific code, optimized SPI writes for the DMA capabilities
of Xilinx SPI libraries and did other modifications.

I must also port the [Adafruit_GFX library](https://github.com/adafruit/Adafruit-GFX-Library).

Tested on AMD Xilinx Zynq-7000 SoC and Artix-7 FPGA (using MicroBlaze CPU) with
the following display: [3.5" SPI Module ILI9488 SKU:MSP3520](http://www.lcdwiki.com/3.5inch_SPI_Module_ILI9488_SKU:MSP3520) (can be purchased [on Amazon](https://www.amazon.com/Hosyond-Display-Compatible-Mega2560-Development/dp/B0BWJHK4M6/ref=sr_1_1?crid=1JH7HIUGZU29J&keywords=3%2C5%22%2B480x320%2BSPI%2BTFT%2BILI9488&qid=1691653179&sprefix=3%2C5%2B480x320%2Bspi%2Btft%2Bili9488%2Caps%2C212&sr=8-1&th=1) or [on AliExpress](https://www.aliexpress.com/item/32995839609.html)).

**TODO:**

- For MicroBlaze AXI Quad SPI FIFO 256 for performance reasons. Transaction width must be 8 bits.

- Increase stack size for the MicroBlaze.

- Add folders src/Adafruit_GFX and src/Adafruit_GFX/Fonts as include paths. Without it the IDE is not able to generate Outlines of the source files.  
  Properties of the application project, C/C++ General, Paths and Symbols, Includes. Specify it as a directory within the workspace.

- Peripherals are overclocked in the demo design:
  
  - AXI GPIO max AXI-lite clock on Artix-7 (slowest speed grade) is 120 MHz in [PG144](https://docs.xilinx.com/v/u/en-US/pg144-axi-gpio), [Table 2-1](https://docs.xilinx.com/pdf-viewer?file=https%3A%2F%2Fdocs.xilinx.com%2Fapi%2Fkhub%2Fdocuments%2F0c0ItRCmnYkoHpcYUCPkEA%2Fcontent%3FFt-Calling-App%3Dft%252Fturnkey-portal%26Ft-Calling-App-Version%3D4.2.26%26filename%3Dpg144-axi-gpio.pdf#G5.306784).
  - AXI Quad SPI max AXI-lite clock on Artix-7 (slowest speed grade) is 120 MHz in [PG153](https://docs.xilinx.com/r/en-US/pg153-axi-quad-spi), chapter [Performance](https://docs.xilinx.com/r/en-US/pg153-axi-quad-spi/Performance).
  - AXI UART lie max AXI-lite clock on Artix-7 (slowest speed grade) is 120 MHz in [PG142](https://docs.xilinx.com/v/u/en-US/pg142-axi-uartlite), [Table 2-1](https://docs.xilinx.com/pdf-viewer?file=https%3A%2F%2Fdocs.xilinx.com%2Fapi%2Fkhub%2Fdocuments%2FdB1MAeh~uLG7FE62a5_QbA%2Fcontent%3FFt-Calling-App%3Dft%252Fturnkey-portal%26Ft-Calling-App-Version%3D4.2.26%26filename%3Dpg142-axi-uartlite.pdf#G5.309065).
