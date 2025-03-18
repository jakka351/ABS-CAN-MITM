![TPSASlogotrans](https://github.com/user-attachments/assets/05cb5cdc-1c9e-474e-89eb-074462c9b788)

# Isolated ABS Module CAN Interface

For a conversion where an XF Falcon is fitted with complete running gear from a mark 1 FG Falcon, running the 5.0 Miami Engine which has no matchign ABS calibration in the older ABS module. To stop the error on the dash and get the ABS module to fucntion we are Man in the Middle attacking the CANbus, by isolating the ABS Module's CAN and intercepting all traffic sent to and from the Module, and editing the data such that the ABS module does not go into fault mode and cause a warning light to appear on the instrumnent cluster. Wheel Speed Sensor and other data such as Steering Angle and Yaw Rate pass through the interface unchanged for use by the PCM. We are simulating a 5.4 litre engine configuration to the ABS module such that it will function normally.

