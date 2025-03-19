//Isolated ABS Module CAN Interface
//For a conversion where an XF Falcon is fitted with complete running gear from a mark 1 FG Falcon, running the 5.0 Miami Engine which has no matching ABS calibration in the older ABS module. 
//To stop the error on the dash and get the ABS module to function, we are Man in the Middle attacking the CANbus, by isolating the ABS Module's CAN and intercepting all traffic sent to and from
// the Module, editing the data such that the ABS module does not go into fault mode and cause a warning light to appear on the instrumnent cluster. Wheel Speed Sensor and other data such as Steering
// Angle and Yaw Rate pass through the interface unchanged for use by the PCM. We are simulating a 5.4 litre engine configuration to the ABS module such that it will function normally.//

//Problem
//There is no matching ABS configuration for the very last of the mark 1 FG Falcons fitted with the 5.0 Miami Engine, in the context of a conversion where an older ABS module is used, the result 
//is a C1991-60 Fault Code in the ABS (Module Calibration Failure) and the ABS subsequently goes into fault mode.//

//Solution
//Trick the ABS module into working by telling it is working with a Boss 5.4 Litre engine, by intercepting and editing the CAN traffic sent to the ABS Module.
#include <Wire.h>
#include <stdio.h>
#include "canbed_dual.h"

CANBedDual PCMCAN(0);
CANBedDual ABSCAN(1);


void setup()
{
    Serial.begin(115200);
    pinMode(18, OUTPUT);  
    Wire1.setSDA(6);
    Wire1.setSCL(7);
    Wire1.begin();
    //SETUP CANBUS PCMCAN and ABSCAN // ABS CAN  NEEDS A TERMINATING RESISTOR 120 OHM
    PCMCAN.init(500000);          // CAN0 baudrate: 500kb/s
    ABSCAN.init(500000);          // CAN1 baudrate: 500kb/s
}

void loop()
{
    unsigned long id = 0;
    int ext = 0;
    int rtr = 0;
    int fd = 0;
    int len = 0;    
    unsigned char dtaGet[100];
    unsigned char dtaGet2[100];
    //READ PCM CAN BUS
    if(PCMCAN.read(&id, &ext, &rtr, &fd, &len, dtaGet))
    {
        Serial.println("ABSCAN");
        Serial.print("id = ");
        Serial.println(id);
        Serial.print("ext = ");
        Serial.println(ext);
        Serial.print("rtr = ");
        Serial.println(rtr);
        Serial.print("fd = ");
        Serial.println(fd);
        Serial.print("len = ");
        Serial.println(len);

        for(int i=0; i<len; i++)
        {
            Serial.print(dtaGet[i]);
            Serial.t("\t");
        }
        Serial.println();
        if (retransmitOntoPcmBusIDs(rxId)) {
        if (isEditPCM(rxId)) {
          unsigned char newData[8];
          memcpy(newData, buf, len);
          editData(id, dtaGet, len);
          //ABS_CAN.sendMsgBuf(rxId, 0, len, newData);
          ABSCAN.send(id, 0, 0, 0, len, newData);

        } 
        else {
          //ABS_CAN.sendMsgBuf(rxId, 0, len, buf);
          ABSCAN.send(id, 0, 0, 0, len, buf);

        }

    }
    // READ ABS CANBUS
    if(ABSCAN.read(&id, &ext, &rtr, &fd, &len, dtaGet2))
    {
        Serial.println("ABSCAN");
        Serial.print("id = ");
        Serial.println(id);
        Serial.print("ext = ");
        Serial.println(ext);
        Serial.print("rtr = ");
        Serial.println(rtr);
        Serial.print("fd = ");
        Serial.println(fd);
        Serial.print("len = ");
        Serial.println(len);

        for(int i=0; i<len; i++)
        {
            Serial.print(dtaGet[i]);
            Serial.t("\t");
        }
        Serial.println();
        if (retransmitOntoAbsBusIDs(id)) {
        if (isEditAbs(id)) {
          unsigned char newData[8];
          memcpy(newData, buf, len);
          editData(rxId, dtaGet2, len);
          //PCM_CAN.sendMsgBuf(rxId, 0, len, newData);
          PCMCAN.send(id, 0, 0, 0, len, newData);
        } 
        else {
          //PCM_CAN.sendMsgBuf(rxId, 0, len, buf);
          PCMCAN.send(id, 0, 0, 0, len, buf);
        }

    }
}
const uint32_t retransmitOntoPcmBusIDs[] = {  // IDS THAT ARE RETRANSMITTED ONTO THE PCM BUS
  0x70,   // YAW_MSG_1
  0x75,   // YAW_MSG_2
  0x80,   // SAS_MSG_1
  0x85,   // YAW_MSG_3
  0x90,   // SAS_MSG_2
  0x210,  // ABS_MSG_1  (edit)
  0x4B0,  // TCS_MSG_1
  0x760  //
};
const uint32_t retransmitOntoAbsBusIDs[] = { // IDS THAT ARE RETRANSMITTED ONTO THE ABS BUS
  0x97    // PCM_MSG_1
  0xFC,   // PCM_MSG_2
  0x120,  // PCM_MSG_3
  0x12D,  // PCM_MSG_4
  0x200,  // PCM_MSG_5
  0x207,  // PCM_MSG_6
  0x230,  // PCM_MSG_7
  0x425,  // PCM_MSG_15
  0x623,  // PCM_MSG_11 (edit)
  0x640,  // PCM_MSG_12 (edit)
  0x650,  // PCM_MSG_13 (edit)
  0x768   //
};
const uint32_t editPCM[] = {  //INTEREPT IDS PCMCAN
  0x623,  // PCM_MSG_11 (edit)
  0x640,  // PCM_MSG_12 (edit)
  0x650,  // PCM_MSG_13 (edit)
};
const uint32_t editABS[] = {  // INTERCEPT IDS ABSCAN
  0x210    // PCM_MSG_1
};
const int PCMIDs = sizeof(retransmitOntoPcmBusIDs) / sizeof(retransmitOntoPcmBusIDs[0]);
const int ABSIDs = sizeof(retransmitOntoAbsBusIDs) / sizeof(retransmitOntoAbsBusIDs[0]);
const int numPCMIDs = sizeof(editPCM) / sizeof(editPCM[0]);
const int numABSIDs = sizeof(editABS) / sizeof(editABS[0]);
// Check if a CAN ID should be edited for which bus...
bool isEditPCM(uint32_t id) {
  for (int i = 0; i < numPCMIDs; i++) {
    if (id == editPCM[i])
      return true;
  }
  return false;
}
bool isEditAbs(uint32_t id) {
  for (int i = 0; i < numABSIDS; i++) {
    if (id == editABS[i])
      return true;
  }
  return false;
}
bool retransmitPcm(uint32_t id) {
  for (int i = 0; i < PCMIDS; i++) {
    if (id == retransmitOntoPcmBusIDs[i])
      return true;
  }
  return false;
}
bool retransmitAbs(uint32_t id) {
  for (int i = 0; i < ABSIDS; i++) {
    if (id == retransmitOntoAbsBusIDs[i])
      return true;
  }
  return false;
}
// Edit the data for specific CAN IDs with example modifications.
// The method first decodes the incoming 8-byte message into its individual fields
// (as defined in the DBC), optionally modifies some parameters, and then reassembles
// the message back into the 8-byte array.
// 
// Supported CAN IDs (in hexadecimal):
//   0x210: ABS_MSG_1
//   0x623: PCM_MSG_11
//   0x640: PCM_MSG_12
//   0x650: PCM_MSG_13 (m16 group used in this example)
void editData(uint32_t id, unsigned char *data, unsigned char len) {
  if (len < 8) return;  // Ensure we have 8 bytes
  // Use a 64-bit container to simplify bit operations.
  uint64_t full = 0;
  for (int i = 0; i < 8; i++) {
    full |= ((uint64_t)data[i]) << (8 * i);
  }
  if (id == 0x210) {  // ABS_MSG_1 (CAN ID: 0x210)
    // --- Decode 1-bit signals from the incoming message ---
    bool gearShiftInhibit    = (data[4] >> 3) & 0x01;  // Bit 35 (data[4] bit 3)
    bool btmClearEngOffTimer = (data[4] >> 4) & 0x01;  // Bit 36
    bool hdcFailed           = (data[4] >> 5) & 0x01;  // Bit 37
    bool hdcActive           = (data[4] >> 6) & 0x01;  // Bit 38
    bool hdcEnabled          = (data[4] >> 7) & 0x01;  // Bit 39
    bool vdcFailed           = (data[3] >> 0) & 0x01;  // Bit 24 (data[3] bit 0)
    bool vdcActive           = (data[3] >> 1) & 0x01;  // Bit 25
    bool vdcDisabledByUser   = (data[3] >> 2) & 0x01;  // Bit 26
    bool tcsFailed           = (data[3] >> 3) & 0x01;  // Bit 27
    bool tcsActive           = (data[3] >> 4) & 0x01;  // Bit 28
    bool tcsDisabledByUser   = (data[3] >> 5) & 0x01;  // Bit 29
    bool absFailed           = (data[3] >> 6) & 0x01;  // Bit 30
    bool ebdFailed           = (data[3] >> 7) & 0x01;  // Bit 31
    bool absActive           = (data[2] >> 0) & 0x01;  // Bit 16 (data[2] bit 0)
    bool splitSlowFastTorqueReq = (data[2] >> 1) & 0x01; // Bit 17
    bool abdFitted           = (data[2] >> 2) & 0x01;  // Bit 18
    bool vdcFitted           = (data[2] >> 3) & 0x01;  // Bit 19
    bool tcsFitted           = (data[2] >> 4) & 0x01;  // Bit 20
    bool absFitted           = (data[2] >> 5) & 0x01;  // Bit 21
    bool ebdFitted           = (data[2] >> 6) & 0x01;  // Bit 22
    bool torqueType          = (data[2] >> 7) & 0x01;  // Bit 23
    // --- Decode 16-bit signals ---
    uint16_t desiredTorqueCommandSlow = (full >> 47) & 0xFFFF; // Bits 47..62
    uint16_t desiredTorqueCommand = (full >> 7) & 0xFFFF;        // Bits 7..22
    // --- Example Modification ---
    // Invert the gearShiftInhibit flag (this is just an example)
    gearShiftInhibit = !gearShiftInhibit;
    // --- Reassemble the message ---
    full = 0;
    full |= ((uint64_t)(gearShiftInhibit ? 1 : 0)) << 35;
    full |= ((uint64_t)(btmClearEngOffTimer ? 1 : 0)) << 36;
    full |= ((uint64_t)(hdcFailed ? 1 : 0)) << 37;
    full |= ((uint64_t)(hdcActive ? 1 : 0)) << 38;
    full |= ((uint64_t)(hdcEnabled ? 1 : 0)) << 39;
    full |= ((uint64_t)(vdcFailed ? 1 : 0)) << 24;
    full |= ((uint64_t)(vdcActive ? 1 : 0)) << 25;
    full |= ((uint64_t)(vdcDisabledByUser ? 1 : 0)) << 26;
    full |= ((uint64_t)(tcsFailed ? 1 : 0)) << 27;
    full |= ((uint64_t)(tcsActive ? 1 : 0)) << 28;
    full |= ((uint64_t)(tcsDisabledByUser ? 1 : 0)) << 29;
    full |= ((uint64_t)(absFailed ? 1 : 0)) << 30;
    full |= ((uint64_t)(ebdFailed ? 1 : 0)) << 31;
    full |= ((uint64_t)(absActive ? 1 : 0)) << 16;
    full |= ((uint64_t)(splitSlowFastTorqueReq ? 1 : 0)) << 17;
    full |= ((uint64_t)(abdFitted ? 1 : 0)) << 18;
    full |= ((uint64_t)(vdcFitted ? 1 : 0)) << 19;
    full |= ((uint64_t)(tcsFitted ? 1 : 0)) << 20;
    full |= ((uint64_t)(absFitted ? 1 : 0)) << 21;
    full |= ((uint64_t)(ebdFitted ? 1 : 0)) << 22;
    full |= ((uint64_t)(torqueType ? 1 : 0)) << 23;
    full |= ((uint64_t)(desiredTorqueCommandSlow & 0xFFFF)) << 47;
    full |= ((uint64_t)(desiredTorqueCommand & 0xFFFF)) << 7;
    // Write the reassembled 64-bit value back into the data array (little-endian)
    for (int i = 0; i < 8; i++) {
      data[i] = (full >> (8 * i)) & 0xFF;
    }
  }
  else if (id == 0x623) {  // PCM_MSG_11 (CAN ID: 0x623)
    // --- Decode fields ---
    // Recombine the 8 bytes into a 64-bit value.
    full = 0;
    for (int i = 0; i < 8; i++) {
      full |= ((uint64_t)data[i]) << (8 * i);
    }
    uint8_t engineDisp = (full >> 7) & 0xFF;          // Engine_Displacement: bits 7-14
    uint8_t valveCount = (full >> 10) & 0x07;           // Valve_Count: bits 10-12
    uint8_t cylinderCount = (full >> 15) & 0x1F;        // Cylinder_Count: bits 15-19
    uint8_t inductionType = (full >> 19) & 0x0F;        // Induction_Type: bits 19-22
    uint8_t fuelTypeDelivery = (full >> 23) & 0x0F;     // Fuel_Type_And_Delivery: bits 23-26
    uint8_t peakEngineTorque = (full >> 31) & 0xFF;     // Peak_Engine_Torque: bits 31-38
    uint8_t auxVehicleId = (full >> 39) & 0xFF;         // Aux_Vehicle_Id: bits 39-46
    bool rwdFlag = ((full >> 40) & 0x01) != 0;          // RWD_Flag: bit 40

    // --- Example Modification ---
    // Toggle the RWD flag.
    rwdFlag = !rwdFlag;

    // --- Reassemble ---
    full = 0;
    full |= ((uint64_t)(engineDisp & 0xFF)) << 7;
    full |= ((uint64_t)(valveCount & 0x07)) << 10;
    full |= ((uint64_t)(cylinderCount & 0x1F)) << 15;
    full |= ((uint64_t)(inductionType & 0x0F)) << 19;
    full |= ((uint64_t)(fuelTypeDelivery & 0x0F)) << 23;
    full |= ((uint64_t)(peakEngineTorque & 0xFF)) << 31;
    full |= ((uint64_t)(auxVehicleId & 0xFF)) << 39;
    full |= ((uint64_t)(rwdFlag ? 1 : 0)) << 40;

    for (int i = 0; i < 8; i++) {
      data[i] = (full >> (8 * i)) & 0xFF;
    }
  }
  else if (id == 0x640) {  // PCM_MSG_12 (CAN ID: 0x640)
    // --- Decode fields ---
    full = 0;
    for (int i = 0; i < 8; i++) {
      full |= ((uint64_t)data[i]) << (8 * i);
    }
    bool autoTransTCMControlFlag = ((full >> 2) & 0x01) != 0;
    bool autoTransPCMControlFlag = ((full >> 3) & 0x01) != 0;
    bool manualTransFlag = ((full >> 4) & 0x01) != 0;
    bool awdFlag = ((full >> 5) & 0x01) != 0;
    bool rwdFlag = ((full >> 6) & 0x01) != 0;
    bool fwdFlag = ((full >> 7) & 0x01) != 0;
    uint16_t fractionPortionRatio = (full >> 15) & 0xFFFF; // Bits 15-30
    uint16_t axleRatio = (full >> 31) & 0xFFFF;            // Bits 31-46
    uint8_t baseTorqueSplit = (full >> 47) & 0xFF;           // Bits 47-54
    uint8_t forwardGearCount = (full >> 51) & 0x0F;          // Bits 51-54
    bool transmissionConfiguration = ((full >> 54) & 0x01) != 0;

    // --- Example Modification ---
    // Increase forwardGearCount by 1 (wrap around at 16).
    forwardGearCount = (forwardGearCount + 1) & 0x0F;

    // --- Reassemble ---
    full = 0;
    if (autoTransTCMControlFlag) full |= ((uint64_t)1) << 2;
    if (autoTransPCMControlFlag) full |= ((uint64_t)1) << 3;
    if (manualTransFlag)         full |= ((uint64_t)1) << 4;
    if (awdFlag)                 full |= ((uint64_t)1) << 5;
    if (rwdFlag)                 full |= ((uint64_t)1) << 6;
    if (fwdFlag)                 full |= ((uint64_t)1) << 7;
    full |= ((uint64_t)(fractionPortionRatio & 0xFFFF)) << 15;
    full |= ((uint64_t)(axleRatio & 0xFFFF)) << 31;
    full |= ((uint64_t)(baseTorqueSplit & 0xFF)) << 47;
    full |= ((uint64_t)(forwardGearCount & 0x0F)) << 51;
    if (transmissionConfiguration) full |= ((uint64_t)1) << 54;

    for (int i = 0; i < 8; i++) {
      data[i] = (full >> (8 * i)) & 0xFF;
    }
  }
  else if (id == 0x650) {  // PCM_MSG_13 (CAN ID: 0x650) - m16 Group
    // --- Decode fields for the m16 group ---
    full = 0;
    for (int i = 0; i < 8; i++) {
      full |= ((uint64_t)data[i]) << (8 * i);
    }
    uint8_t multiplexID = (full >> 7) & 0xFF;             // Multiplex_Header_ID M (bits 7-14)
    uint8_t engineDisp = (full >> 15) & 0xFF;               // Engine_Displacement m16 (bits 15-22)
    uint8_t valveCount = (full >> 18) & 0x07;               // Valve_Count m16 (bits 18-20)
    uint8_t cylinderCount = (full >> 15) & 0x1F;            // Cylinder_Count m16 (bits 15-19) [overlap example]
    uint8_t inductionType = (full >> 27) & 0x0F;            // Induction_Type m16 (bits 27-30)
    uint8_t tcmCalLevel = (full >> 39) & 0xFF;              // TCM_Cal_Level m16 (bits 39-46)

    // --- Example Modification ---
    // Add 5 to the TCM calibration level (with wrap-around).
    tcmCalLevel = (tcmCalLevel + 5) & 0xFF;

    // --- Reassemble the m16 group ---
    full = 0;
    full |= ((uint64_t)(multiplexID & 0xFF)) << 7;
    full |= ((uint64_t)(engineDisp & 0xFF)) << 15;
    full |= ((uint64_t)(valveCount & 0x07)) << 18;
    full |= ((uint64_t)(cylinderCount & 0x1F)) << 15;  // Note: Overlap with engineDisp (multiplexed design)
    full |= ((uint64_t)(inductionType & 0x0F)) << 27;
    full |= ((uint64_t)(tcmCalLevel & 0xFF)) << 39;

    for (int i = 0; i < 8; i++) {
      data[i] = (full >> (8 * i)) & 0xFF;
    }
  }
}



// ENDIF
