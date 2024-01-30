# Tutorial: TFT SPI display on Xilinx Zynq-7000 SoC

:construction: :construction: :construction: :construction: :construction: **UNDER CONSTRUCTION** :construction: :construction: :construction: :construction: :construction: 

This tutorial describes how to use a TFT SPI display on the AMD Xilinx Zynq-7000 SoC platform.

## Hardware

We will use the [3.5&Prime; ILI9488 TFT SPI](http://www.lcdwiki.com/3.5inch_SPI_Module_ILI9488_SKU:MSP3520) 480x320 pixels display (which can be purchased on [Amazon](https://www.amazon.com/Hosyond-Display-Compatible-Mega2560-Development/dp/B0BWJHK4M6/ref=sr_1_1?crid=1JH7HIUGZU29J&keywords=3%2C5%22%2B480x320%2BSPI%2BTFT%2BILI9488&qid=1691653179&sprefix=3%2C5%2B480x320%2Bspi%2Btft%2Bili9488%2Caps%2C212&sr=8-1&th=1) or on [AliExpress](https://www.aliexpress.com/item/32995839609.html); I'm not affiliated in any way).  
The reasons for selecting this particular display are simple: I like its size (it is not too small nor too big), and I prepared a SW library for it. :smiley:

![](pictures/display_MSP3520.jpg)

As a representative of the AMD Xilinx Zynq-7000 SoC platform, we will use the [Digilent Cora Z7](https://digilent.com/shop/cora-z7-zynq-7000-single-core-for-arm-fpga-soc-development/) development board.  
It's a nice small-factor, low-cost Zynq-7000 SoC board with excellent documentation and good board files available for Xilinx Vivado. It comes with a 667 MHz single-core Cortex-A9 processor with integrated programmable logic equivalent to Artix A7 FPGA and 512 MB of DDR3 memory. It is not the most powerful Zynq board, but it is more than adequate for the standalone single-thread application we will build in this project.

![](pictures/cora-obl-600.png)

## Connecting the display

The 3.5&Prime; ILI9488 TFT SPI display is controlled by an SPI bus with a clock frequency of 20 MHz.  
In addition to the SPI, it has to be connected to two GPIO pins (reset and Data/Command selection signals).

We need to connect the display pins highlighted in the photo below.

Logic IO pins accept a 3.3 V voltage level (TTL). VCC and LED (the backlight control) pins need to be connected to a 3.3 V power source.

- Please note that we do not need to connect the "SDO (MISO)" pin of the display to SPI because we are not reading any data from the display.

[<img src="https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/ILI9488_TFT_display_pins.png?raw=true" title="" alt="ILI9488 TFT display pins" width="230">](https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/ILI9488_TFT_display_pins.png)

TODO

## HW design in Vivado

Start Vivado 2023.1. Click Create Project. Click Next.  
Enter the project name and directory. Click Next.  
Select "RTL Project" and "Do not specify sources at this time". Click Next.

Now, we select the board. Click on "Boards" and type "cora" in the Search prompt.  
Select the version of the Cora Z7 you have. Cora Z7-07S is the board in production as of January 2024 (version -10 has been discontinued).  
Click on the download icon in the Status column if this is the very first time you use the board in Vivado.

<img src="pictures/board_selection.png" title="" alt="" width="600">

Click Next and Finish. The Vivado IDE will open an empty project.

Click Create Block Design, and name the design "system":

<img title="" src="pictures/name_bd.png" alt="" width="310">

An empty block design window opens.

Before we use output ports in the block design, we need to define them in a constraints file (.XDC).  
Download Cora Z7 [Master XDC](https://github.com/Digilent/digilent-xdc/blob/master/Cora-Z7-07S-Master.xdc) from Digilent GitHub and import it as a constraints source file by clicking "+" icon in the Sources window; then follow the wizard.

<img title="" src="pictures/add_constraints_source.png" alt="" width="400">

Open the imported constraints file in the editor.  
We are going to use Cora Z7 pins marked IO0..IO4 for connecting the display. They are named ports ck_io0..ck_io4  in the master XDC file. We, therefore, need to uncomment the following lines in the constraints file:

```
## ChipKit Outer Digital Header
#set_property -dict { PACKAGE_PIN U14   IOSTANDARD LVCMOS33 } [get_ports { ck_io0 }]; #IO_L11P_T1_SRCC_34 Sch=ck_io[0]
#set_property -dict { PACKAGE_PIN V13   IOSTANDARD LVCMOS33 } [get_ports { ck_io1 }]; #IO_L3N_T0_DQS_34 Sch=ck_io[1]
#set_property -dict { PACKAGE_PIN T14   IOSTANDARD LVCMOS33 } [get_ports { ck_io2 }]; #IO_L5P_T0_34 Sch=ck_io[2]
#set_property -dict { PACKAGE_PIN T15   IOSTANDARD LVCMOS33 } [get_ports { ck_io3 }]; #IO_L5N_T0_34 Sch=ck_io[3]
#set_property -dict { PACKAGE_PIN V17   IOSTANDARD LVCMOS33 } [get_ports { ck_io4 }]; #IO_L21P_T3_DQS_34 Sch=ck_io[4]
```

For clarity, it's good to add the name of the display pin we connect the port to as a port name suffix. The final content of our constraints file will then look as follows:

```
## ChipKit Outer Digital Header
set_property -dict { PACKAGE_PIN U14   IOSTANDARD LVCMOS33 } [get_ports { ck_io0_SCK  }]; #IO_L11P_T1_SRCC_34 Sch=ck_io[0]
set_property -dict { PACKAGE_PIN V13   IOSTANDARD LVCMOS33 } [get_ports { ck_io1_MOSI }]; #IO_L3N_T0_DQS_34 Sch=ck_io[1]
set_property -dict { PACKAGE_PIN T14   IOSTANDARD LVCMOS33 } [get_ports { ck_io2_DC   }]; #IO_L5P_T0_34 Sch=ck_io[2]
set_property -dict { PACKAGE_PIN T15   IOSTANDARD LVCMOS33 } [get_ports { ck_io3_RST  }]; #IO_L5N_T0_34 Sch=ck_io[3]
set_property -dict { PACKAGE_PIN V17   IOSTANDARD LVCMOS33 } [get_ports { ck_io4_CS   }]; #IO_L21P_T3_DQS_34 Sch=ck_io[4]
```

Now we can add the ports to the Block Design.  
Right-click on the empty diagram space and select Create Port for each of the ports.  
We specify all ports as Output and Type "Other".

<img title="" src="pictures/add_port.png" alt="" width="400">q

Now we add to the diagram an IP representing the Zynq Processing System.  
Search for "zynq" in the IP Catalog window and drag the "ZYNQ7 Processing System" to the diagram.

Now, we let Vivado make basic connections for us. Click Run Block Automation, which appeared at the top of the Diagram window. We accept all block automation defaults, so we just click OK.

This the results of the steps we did so far:

<img title="" src="pictures/zynq_added.png" alt="" width="450">

We must now configure the Zynq to expose GPIO and SPI interfaces.  
Double-click on the Zynq PS IP and click on the "SPI 0" on the top left of Zynq Block Design.

<img title="" src="pictures/zynq_bd.png" alt="" width="650">

Enable SPI 0 in the list of MIO Configuration items.

<img title="" src="pictures/spi_enable.png" alt="" width="550">

Then, scroll down the list and enable EMIO GPIO. Set the width to 2 (we need two GPIO signals for the display).

<img title="" src="pictures/emio_enable.png" alt="" width="650">

Because we are not using Zynq AXI interface in our design, we must switch it off in the Zynq configuration (othewise we would get a critical warning that we do not provide clock signal to the AXI interface).  
Go back to the Zynq Block Design and click on the "32b GP AXI Master Ports" at the bottom. Disable the "M AXI GP0 interface".

<img title="" src="pictures/axi_disable.png" alt="" width="500">

Click OK.  
You will probably get a critical warning message [PSU-1] and [PSU-2] about two parameters having a negative value. Ignore the warning. It doesn't have a negative impact on the functionality of the HW design.

New signal groups GPIO_0 and SPI_0 appeared on the Zynq PS IP.  
Let's start with connecting the SPI. When you expand the SPI_0, you will see a lot of signals. We need to connect just three of them (all with the suffix "_O") in the way shown in the following screenshot. (We don't need to connect the SPI0_MISO_I signal because we don't read any data from the display.)

<img title="" src="pictures/spi_connected.png" alt="" width="500">

