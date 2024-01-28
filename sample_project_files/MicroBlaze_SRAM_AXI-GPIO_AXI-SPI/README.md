:construction: :construction: :construction: :construction: :construction: **THIS README IS WORK IN PROGRESS**:construction: :construction: :construction: :construction: :construction: 

## HW design

The MicroBlaze and all peripherals are clocked on 100 MHz. (My attempts to try a higher clock were not successful. 100 MHz is the highest clock producing a stable design.)

In the Address Editor I must manually change size of /axi_emc_0/S_AXI_MEM to 512K (it appear twice in the Address Editor list).

> [!NOTE]
> It was important to set the Source to "No Buffer" in the Input Clock Information in the Clocking Wizard configuration.  
> Without the "No Buffer" setting, I was getting strange timing requirement warnings. In fact, we have no buffer between the input clock pin and the Clocking Wizard.

## Application setup in Vitis

In the C/C++ Build Settings, I set Optimization to "Optimize most (-O3)". The default optimization setting was -O2.

We have 256 MB of DDR3 SDRAM at our disposal, so I increased the Stack Size from the default 1 kB to 16 kB and the Heap Size from the default 2 kB to 32 kB.

Increasing the Stack Size from the MicroBlaze's default 1 kB is important.  
The method ILI9488::fillRect uses 768 B from the stack for a local array, which is used to prepare data to be sent to the display over SPI.  
The [demo application](../../ILI9488-Xilinx_library_demo_app) used in this project works with a 1 kB stack size, but more complex applications may not.

## The physical connection of the display

