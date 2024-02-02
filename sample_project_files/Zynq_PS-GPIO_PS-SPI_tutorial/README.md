# Tutorial: TFT SPI display on Xilinx Zynq-7000 SoC

:construction: :construction: :construction: :construction: :construction: **UNDER CONSTRUCTION** :construction: :construction: :construction: :construction: :construction: 

This tutorial describes how to use a TFT SPI display on the AMD Xilinx Zynq-7000 SoC platform.

## Hardware

We will use the [3.5&Prime; ILI9488 TFT SPI](http://www.lcdwiki.com/3.5inch_SPI_Module_ILI9488_SKU:MSP3520) 480x320 pixels display (which can be purchased on [Amazon](https://www.amazon.com/Hosyond-Display-Compatible-Mega2560-Development/dp/B0BWJHK4M6/ref=sr_1_1?crid=1JH7HIUGZU29J&keywords=3%2C5%22%2B480x320%2BSPI%2BTFT%2BILI9488&qid=1691653179&sprefix=3%2C5%2B480x320%2Bspi%2Btft%2Bili9488%2Caps%2C212&sr=8-1&th=1) or on [AliExpress](https://www.aliexpress.com/item/32995839609.html); I'm not affiliated in any way).  
The reasons for selecting this particular display are simple: I like its size (it is not too small nor too big), and I prepared a SW library for it. :smiley:

![](pictures/display_MSP3520.jpg)

As a representative of the AMD Xilinx Zynq-7000 SoC platform, we will use the [Digilent Cora Z7](https://digilent.com/shop/cora-z7-zynq-7000-single-core-for-arm-fpga-soc-development/) development board.  
It's a nice small-factor, low-cost Zynq-7000 SoC board with excellent documentation and good board files available for Xilinx Vivado. It comes with a 667 MHz single-core Cortex-A9 processor with integrated programmable logic equivalent to Artix A7 FPGA and 512 MB of DDR3 memory.  It can accept standard Arduino shields (the ones that run on 3.3 V).
It is not the most powerful Zynq board, but it is more than adequate for the standalone single-thread application we will build in this project.

Please note that most steps in this tutorial are valid also for any other Zynq-7000 SoC board.

![](pictures/cora-obl-600.png)

## Connecting the display

The 3.5&Prime; ILI9488 TFT SPI display is controlled by an SPI bus with a clock frequency of 20 MHz.  
In addition to the SPI, it has to be connected to two GPIO pins (reset and Data/Command selection signals).

We need to connect the display pins highlighted in the photo below.

Logic IO pins accept a 3.3 V voltage level (TTL). VCC and LED (the backlight control) pins need to be connected to a 3.3 V power source.

- Please note that we do not need to connect the "SDO (MISO)" pin of the display to SPI because we are not reading any data from the display.

[<img src="https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/ILI9488_TFT_display_pins.png?raw=true" title="" alt="ILI9488 TFT display pins" width="230">](https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/ILI9488_TFT_display_pins.png)

TODO

[<img src="https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/ILI9488_with_Cora_Z7.jpg?raw=true" title="" alt="" width="650">](https://github.com/viktor-nikolov/ILI9488-Xilinx/blob/main/pictures/ILI9488_with_Cora_Z7.jpg)

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

<img title="" src="pictures/add_port.png" alt="" width="400">

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

The next step is to connect GPIO_O[1:0] to the ck_io3_RST and ck_io2_DC ports.  
GPIO_O[1:0] is a vector of two signals. We will use Slice elements to "cut" the vector to the two scalar signals.

Search for "slice" in the IP Catalog and drag Slice two times to the diagram.  
Double-click on xlslice_0 and configure it to extract bit 0 from a 2-bit input:

<img title="" src="pictures/xlslice_0.png" alt="" width="350">

Next, configure xlslice_1 to extract bit 1 from the 2-bit input:

<img title="" src="pictures/xlslice_1.png" alt="" width="350">

Connect inputs of both slices to the GPIO_O[1:0]. Output of xlslice_0 goes to port ck_io3_RST. xlslice_1 drives port ck_io2_DC.  
We now have our final diagram.

<img title="" src="pictures/final_diagram.png" alt="" width="550">

To make sure that nothing was missed, click the Validate Design button in the toolbar of the diagram window (or press F6). There should be no errors.

HDL Wrapper for the diagram needs to be created: Go to Sources|Design Sources, right-click on "system", select "Create HDL Wrapper", and select "Let Vivado manage wrapper".

Now, we create the design outputs: Click "Generate Bitstream" in the Flow Navigator on the left. Synthesis and Implementation will be run automatically before bitstream generation.

There should be no errors or critical warnings. (Some standard warnings will be generated, though.)

Last but not least, we need to export the hardware specification. It is necessary for the development of the SW app for the Zynq ARM core in Vitis IDE.  
Go to File|Export|Export Hardware, select "Include Bitstream". Set the XSA file name as "system_wrapper", as suggested by the Export Hardware wizard.

## SW application in Vitis

Start Vitis 2023.1.  
Enter the path to a directory where you want to store the workspace and click "Launch."

The first step is to create a platform project based on the HW design we created in Vivado.  
Click on "Create Platform Project". Name the project "system". On the next screen, we select the XSA file, which we created by "Export Hardware" in Vivado.  
Click on Browse and go to the root folder of your HW project (where the .xpr file is). Here you should find the "system_wrapper.xsa". The wizard will read the XSA file. Set the Operating system to "standalone" and click Finish. The Vitis IDE will open the platform project "system".

<img title="" src="pictures/new_platform_proj.png" alt="" width="500">

With the platform created, we now create the application project.  
Select File|New|Application project. Skip the first page of the wizard.   
On the next screen, we select the platform project "system", which we created a moment ago. Click Next.

<img title="" src="pictures/new_app_project.png" alt="" width="700">

On the next screen, fill the "Application project name" with a name, for example, "ILI9488_CoraZ7". The rest of the fields will be filled automatically. Click Next.

<img title="" src="pictures/app_proj_name.png" alt="" width="450">

On the next screen, we just confirm that we are using standalone_domain. Then, on the last screen, select the project template "Empty Application (**C++**)".  
We now have an empty application project created.

<img title="" src="pictures/app_proj_created.png" alt="" width="700">


> [!TIP]
>
> If you don't have git installed on your computer, the easy way to download content of a folder from a GitHub repository is to use [download-directory.github.io](https://download-directory.github.io/).
> To get the ILI9488-Xilinx_library folder content, enter the path `https://github.com/viktor-nikolov/ILI9488-Xilinx/tree/main/ILI9488-Xilinx_library`.

