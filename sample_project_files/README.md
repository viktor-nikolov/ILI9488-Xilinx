# ILI9488 TFT display library sample projects

This folder includes several sample projects designed in Vivado 2023.1 and Vitis 2023.1, which show the use of the [ILI9488 library](https://github.com/viktor-nikolov/ILI9488-Xilinx) on Zynq-7000 and MicroBlaze.  
The projects include a standalone application running on Zynq ARM core or MicroBlaze soft CPU.

See the readme-files in the respective folders.

| Platform               | GPIO     | SPI     | Board                                                        | Folder                                                       |
| ---------------------- | -------- | ------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Zynq-7000              | PS GPIO  | PI SPI  | [Zybo Z7-20](https://digilent.com/shop/zybo-z7-zynq-7000-arm-fpga-soc-development-board/) | [Zynq_PS-GPIO_PS-SPI](Zynq_PS-GPIO_PS-SPI)                   |
| Zynq-7000              | PS GPIO  | AXI SPI | [Zybo Z7-20](https://digilent.com/shop/zybo-z7-zynq-7000-arm-fpga-soc-development-board/) | [Zynq_PS-GPIO_AXI-SPI](Zynq_PS-GPIO_AXI-SPI)                 |
| MicroBlaze, DDR3 SDRAM | AXI GPIO | AXI SPI | [Arty A7-35T](https://digilent.com/shop/arty-a7-100t-artix-7-fpga-development-board/) | [MicroBlaze_DDR3_AXI-GPIO_AXI-SPI](MicroBlaze_DDR3_AXI-GPIO_AXI-SPI) |

