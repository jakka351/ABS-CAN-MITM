![TPSASlogotrans](https://github.com/user-attachments/assets/05cb5cdc-1c9e-474e-89eb-074462c9b788)

# Isolated ABS Module CAN Interface

For a conversion where an XF Falcon is fitted with complete running gear from a mark 1 FG Falcon, running the 5.0 Miami Engine which has no matching ABS calibration in the older ABS module. To stop the error on the dash and get the ABS module to function, we are Man in the Middle attacking the CANbus, by isolating the ABS Module's CAN and intercepting all traffic sent to and from the Module, editing the data such that the ABS module does not go into fault mode and cause a warning light to appear on the instrumnent cluster. Wheel Speed Sensor and other data such as Steering Angle and Yaw Rate pass through the interface unchanged for use by the PCM. We are simulating a 5.4 litre engine configuration to the ABS module such that it will function normally.

## Problem
There is no matching ABS configuration for the very last of the mark 1 FG Falcons fitted with the 5.0 Miami Engine, in the context of a conversion where an older ABS module is used, the result is a C1991-60 Fault Code in the ABS (Module Calibration Failure) and the ABS subsequently goes into fault mode.

## Solution
Trick the ABS module into working by telling it is working with a Boss 5.4 Litre engine, by intercepting and editing the CAN traffic sent to the ABS Module.

## Hardware
Longan Labs CANBED Dual powered by RP2040 microcontroller, with dual CAN interfaces.  
Library: https://github.com/Longan-Labs/CANBedDual_Arduino_Lib  
![image](https://github.com/user-attachments/assets/60db1c4e-7604-4362-bfe3-b005f517b732)




