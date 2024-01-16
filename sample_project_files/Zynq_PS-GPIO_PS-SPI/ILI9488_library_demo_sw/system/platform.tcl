# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\Temp\for_GitHub\ILI9488_library_demo_sw\system\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\Temp\for_GitHub\ILI9488_library_demo_sw\system\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {system}\
-hw {C:\Temp\for_GitHub\ZyboZ7_PS-GPIO_PS-SPI_hw\system_wrapper.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -out {C:/Temp/for_GitHub/ILI9488_library_demo_sw}

platform write
platform generate -domains 
platform active {system}
platform generate
platform clean
platform generate
platform active {system}
platform clean
platform generate
platform clean
