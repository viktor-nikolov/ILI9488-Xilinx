:construction: :construction: :construction: :construction: :construction: **THIS README IS WORK IN PROGRESS**:construction: :construction: :construction: :construction: :construction: 

## HW design

The MicroBlaze and all peripherals are clocked on 100 MHz. (My attempts to try a higher clock were not successful. 100 MHz is the highest clock producing a stable design.)

> [!NOTE]
> It was important to set the Source to "No Buffer" in the Input Clock Information in the Clocking Wizard configuration.  
> Without the "No Buffer" setting, I was getting strange timing requirement warnings. In fact, we have no buffer between the input clock pin and the Clocking Wizard.

## Application setup in Vitis

## The physical connection of the display

