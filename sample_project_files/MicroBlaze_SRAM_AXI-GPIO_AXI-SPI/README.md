:construction: :construction: :construction: :construction: :construction: **THIS README IS WORK IN PROGRESS**:construction: :construction: :construction: :construction: :construction: 

This folder contains a sample project for using the [ILI9488 TFT display library](https://github.com/viktor-nikolov/ILI9488-Xilinx) on the [MicroBlaze](https://www.xilinx.com/products/design-tools/microblaze.html) soft CPU using SRAM on the [Cmod A7-35T](https://digilent.com/shop/cmod-a7-35t-breadboardable-artix-7-fpga-module/) board.

The design was made in Vivado 2023.1 and Vitis 2023.1.

## HW design

HW design for the [Cmod A7-35T](https://digilent.com/shop/cmod-a7-35t-breadboardable-artix-7-fpga-module/) was quite straightforward thanks to the board files provided by Digilent.  
I dragged "Call RAM" and "USB UART" components from the Board window onto the Block Diagram. Then I manually created AXI GPIO and AXI Quad SPI IPs and added the MicroBlaze. Vivado connection automation then did vast majority of connections, including creation of AXI Interconnectg and AXI SmartConnect IPs.

The MicroBlaze and all peripherals are clocked on 100 MHz. (My attempts to try a higher clock were not successful. 100 MHz is the highest clock producing a stable design.)

In the Address Editor, I must manually change the size of /axi_emc_0/S_AXI_MEM to 512K (note that it appears twice in the Address Editor list).

> [!NOTE]
> It was important to set the Source to "No Buffer" in the Input Clock Information in the Clocking Wizard configuration.  
> Without the "No Buffer" setting, I was getting strange timing requirement warnings. In fact, we have no buffer between the input clock pin and the Clocking Wizard.

[<img src="https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/MicroBlaze_SRAM_AXI-GPIO_AXI-SPI_diagram.png?raw=true" title="" alt="">](https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/MicroBlaze_SRAM_AXI-GPIO_AXI-SPI_diagram.png)

## Application setup in Vitis

In the C/C++ Build Settings, I set Optimization to "Optimize most (-O3)". The default optimization setting was -O2.

We have 512 KB of SRAM at our disposal (however, 229 KB is consumed by the code of the demo application).  I increased the Stack Size from the default 1 KB to 8 KB and the Heap Size from the default 2 KB to 16 KB in lscript.ld.

Increasing the Stack Size from the MicroBlaze's default 1 KB is important.  
The method ILI9488::fillRect uses 768 B from the stack for a local array, which is used to prepare data to be sent to the display over SPI.  
The [demo application](../../ILI9488-Xilinx_library_demo_app) used in this project works with a 1 KB stack size, but more complex applications may not.

> [!NOTE]
> Having only 512 KB of SRAM on Cmod A7 significantly limits our ability to work with larger RGB bitmaps on this platform.  
> The size of the demo application is 229 KB (which includes 19 KB of a small sample RGB bitmap [Sun_png_image888](../../ILI9488-Xilinx_library_demo_app/demo_image2.h)). That leaves 283 KB available. The demo application on Cmod A7, therefore, can't display from memory a full-screen bitmap 480x320 because that is 300 KB big (in the color coding R:G:B 5b:6b:5b).

## The physical connection of the display

[<img src="https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/CmodA7_connection_schematics.png?raw=true" title="" alt="" width="600">](https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/CmodA7_connection_schematics.png)

[<img src="https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/cmoda7_b_dip.png?raw=true" title="" alt="" width="180">](https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/cmoda7_b_dip.png)

[<img src="https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/ILI9488_with_Cmod_A7.jpg?raw=true" title="" alt="" width="500">](https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/ILI9488_with_Cmod_A7.jpg)