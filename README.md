![image](https://github.com/user-attachments/assets/bd00835a-e37b-415e-8666-484b4e5fd05e)


<a href="https://testerpresent.com.au/"><img src="https://img.shields.io/badge/Tester Present Specialist Automotive Solutions -Open Source Projects- blue" /></a>

# Isolated ABS Module CAN Interface

For a conversion where an XF Falcon is fitted with complete running gear from a mark 1 FG Falcon, running the 5.0 Miami Engine which has no matching ABS calibration in the older ABS module. To stop the error on the dash and get the ABS module to function, we are Man in the Middle attacking the CANbus, by isolating the ABS Module's CAN and intercepting all traffic sent to and from the Module, editing the data such that the ABS module does not go into fault mode and cause a warning light to appear on the instrumnent cluster. Wheel Speed Sensor and other data such as Steering Angle and Yaw Rate pass through the interface unchanged for use by the PCM. We are simulating a 5.4 litre engine configuration to the ABS module such that it will function normally.

## Problem
There is no matching ABS configuration for the very last of the mark 1 FG Falcons fitted with the 5.0 Miami Engine, in the context of a conversion where an older ABS module is used, the result is a C1991-60 Fault Code in the ABS (Module Calibration Failure) and the ABS subsequently goes into fault mode.

## Solution
Trick the ABS module into working by telling it is working with a Boss 5.4 Litre engine, by intercepting and editing the CAN traffic sent to the ABS Module.

## Schematic
The Man in the Middle attack isolates the ABS module CAN at the two red crosses:
![image](https://github.com/user-attachments/assets/7c59c8aa-a728-439b-baf1-bfa6478fa8a4)

## Hardware
Longan Labs CANBED Dual powered by RP2040 microcontroller, with dual CAN interfaces.  
Library: https://github.com/Longan-Labs/CANBedDual_Arduino_Lib  
![image](https://github.com/user-attachments/assets/166ab3f9-509d-491a-bf40-d8cc84eb1994)

## Vehicle
![image](https://github.com/user-attachments/assets/d15c7d22-bcca-4277-9383-27cd10da4402)

![image](https://github.com/user-attachments/assets/88baecad-9432-44ae-ac9f-5dc6e66a6231)







