# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\CoraZ7_tutorial\ILI9488_TFT_on_CoraZ7_sw\system\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\CoraZ7_tutorial\ILI9488_TFT_on_CoraZ7_sw\system\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {system}\
-hw {C:\CoraZ7_tutorial\ILI9488_TFT_on_CoraZ7_hw\system_wrapper.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -out {C:/CoraZ7_tutorial/ILI9488_TFT_on_CoraZ7_sw}

platform write
platform generate -domains 
platform active {system}
domain active {zynq_fsbl}
bsp reload
bsp reload
bsp config stdin "ps7_uart_0"
bsp reload
bsp reload
platform generate
platform generate
platform clean
platform generate
platform clean
