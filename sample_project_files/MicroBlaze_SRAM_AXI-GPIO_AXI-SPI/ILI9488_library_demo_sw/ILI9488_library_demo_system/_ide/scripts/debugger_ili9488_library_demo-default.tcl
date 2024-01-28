# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: C:\Temp\for_GitHub\ILI9488_library_demo_sw\ILI9488_library_demo_system\_ide\scripts\debugger_ili9488_library_demo-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source C:\Temp\for_GitHub\ILI9488_library_demo_sw\ILI9488_library_demo_system\_ide\scripts\debugger_ili9488_library_demo-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -filter {jtag_cable_name =~ "Digilent Cmod A7 - 35T 210328B857E3A" && level==0 && jtag_device_ctx=="jsn-Cmod A7 - 35T-210328B857E3A-0362d093-0"}
fpga -file C:/Temp/for_GitHub/ILI9488_library_demo_sw/ILI9488_library_demo/_ide/bitstream/download.bit
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
loadhw -hw C:/Temp/for_GitHub/ILI9488_library_demo_sw/system/export/system/hw/system_wrapper.xsa -regs
configparams mdm-detect-bscan-mask 2
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
rst -system
after 3000
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
dow C:/Temp/for_GitHub/ILI9488_library_demo_sw/ILI9488_library_demo/Release/ILI9488_library_demo.elf
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
con
