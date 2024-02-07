# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: C:\CoraZ7_tutorial\ILI9488_TFT_on_CoraZ7_sw\ILI9488_CoraZ7_system\_ide\scripts\debugger_ili9488_coraz7-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source C:\CoraZ7_tutorial\ILI9488_TFT_on_CoraZ7_sw\ILI9488_CoraZ7_system\_ide\scripts\debugger_ili9488_coraz7-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Cora Z7 - 7007S 210370B9F18FA" && level==0 && jtag_device_ctx=="jsn-Cora Z7 - 7007S-210370B9F18FA-13723093-0"}
fpga -file C:/CoraZ7_tutorial/ILI9488_TFT_on_CoraZ7_sw/ILI9488_CoraZ7/_ide/bitstream/system_wrapper.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw C:/CoraZ7_tutorial/ILI9488_TFT_on_CoraZ7_sw/system/export/system/hw/system_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source C:/CoraZ7_tutorial/ILI9488_TFT_on_CoraZ7_sw/ILI9488_CoraZ7/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow C:/CoraZ7_tutorial/ILI9488_TFT_on_CoraZ7_sw/ILI9488_CoraZ7/Debug/ILI9488_CoraZ7.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
