////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                             
//                             ___________              __                 __________                                      __                         
//                             \__    ___/___   _______/  |_  ___________  \______   \_______   ____   ______ ____   _____/  |_                       
//                               |    |_/ __ \ /  ___/\   __\/ __ \_  __ \  |     ___/\_  __ \_/ __ \ /  ___// __ \ /    \   __\                      
//                               |    |\  ___/ \___ \  |  | \  ___/|  | \/  |    |     |  | \/\  ___/ \___ \\  ___/|   |  \  |                        
//                               |____| \___  >____  > |__|  \___  >__|     |____|     |__|    \___  >____  >\___  >___|  /__|                        
//                                          \/     \/            \/                                \/     \/     \/     \/                            
//                               _________                    .__       .__  .__          __                                                          
//                              /   _____/_____   ____   ____ |__|____  |  | |__| _______/  |_                                                        
//                              \_____  \\____ \_/ __ \_/ ___\|  \__  \ |  | |  |/  ___/\   __\                                                       
//                              /        \  |_> >  ___/\  \___|  |/ __ \|  |_|  |\___ \  |  |                                                         
//                             /_______  /   __/ \___  >\___  >__(____  /____/__/____  > |__|                                                         
//                                     \/|__|        \/     \/        \/             \/                                                               
//                                _____          __                         __  .__                                                                   
//                               /  _  \  __ ___/  |_  ____   _____   _____/  |_|__|__  __ ____                                                       
//                              /  /_\  \|  |  \   __\/  _ \ /     \ /  _ \   __\  \  \/ // __ \                                                      
//                             /    |    \  |  /|  | (  <_> )  Y Y  (  <_> )  | |  |\   /\  ___/                                                      
//                             \____|__  /____/ |__|  \____/|__|_|  /\____/|__| |__| \_/  \___  >                                                     
//                                     \/                         \/                          \/                                                      
//                               _________      .__          __  .__                                                                                  
//                              /   _____/ ____ |  |  __ ___/  |_|__| ____   ____   ______                                                            
//                              \_____  \ /  _ \|  | |  |  \   __\  |/  _ \ /    \ /  ___/                                                            
//                              /        (  <_> )  |_|  |  /|  | |  (  <_> )   |  \\___ \                                                              
//                             /_______  /\____/|____/____/ |__| |__|\____/|___|  /____  >                                                           
//                                     \/                                       \/     \/                                                             
//
//
//
//                              Tester Present Specialist Automotive Solutions Open Source License (TPSAS-OSL)
//                              Version 1.0, 2025
//
//                              Copyright (c) 2025 Tester Present Specialist Automotive Solutions
//
//                              Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
//                              documentation files (the "Software"), to use, modify, and distribute the Software subject to the following conditions:
//
//          1. Attribution.
//            - The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//            - Credit must be given to "Tester Present Specialist Automotive Solutions" in any documentation, public notices, or product materials 
//              that acknowledge the use of the Software.//
//
//          2. Open Source Use Only.
//            - The Software, including any modifications or derivative works (collectively, “Derivatives”), must be distributed only as open source software.
//            - Derivatives or distributions of the Software must be licensed under this license or another license approved by the Open Source Initiative 
//              (OSI) that maintains equivalent open source freedoms.
//            - The Software and any Derivatives may not be used as a component in any closed source or proprietary software product.
//
//          3. Redistribution.
//            - Redistributions of the Software, in source or binary form, must reproduce the above attribution, this license, and the disclaimer below 
//              in the documentation and/or other materials provided with the distribution.
//            - Any modifications made to the Software must be clearly marked as such, and the modified files must carry prominent notices stating that changes have been made.
//
//          4. No Warranty.
//            - THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//              FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT.
//            - IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
//              TORT, OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//          5. Termination.
//            - This license and the rights granted hereunder will terminate automatically upon any breach of the above conditions.
//            - Upon termination, you must cease all use and distribution of the Software and any Derivatives.
//
//          6. Entire Agreement.
//            - This license constitutes the entire agreement between you and Tester Present Specialist Automotive Solutions regarding the Software 
//              and supersedes any prior understandings or agreements, whether written or oral, regarding the subject matter herein.
//
//          By using, modifying, or distributing the Software, you agree to abide by all the terms and conditions of this license.
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                             
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
//////////////////////////////////////////////////////
#include <Wire.h>
#include <stdio.h>
#include "canbed_dual.h"
//////////////////////////////////////////////////////
CANBedDual PCMCAN(0);
CANBedDual ABSCAN(1);
//////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////
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
        Serial.println("PCMCAN");
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
        if (retransmitOntoAbsIDs(rxId)) {
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
        if (retransmitOntoPcmBusIDs(id)) {
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
//////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////
const int PCMIDs = sizeof(retransmitOntoPcmBusIDs) / sizeof(retransmitOntoPcmBusIDs[0]);
const int ABSIDs = sizeof(retransmitOntoAbsBusIDs) / sizeof(retransmitOntoAbsBusIDs[0]);
const int numPCMIDs = sizeof(editPCM) / sizeof(editPCM[0]);
const int numABSIDs = sizeof(editABS) / sizeof(editABS[0]);
//////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////
int alter650 = 0;
//////////////////////////////////////////////////////
void editData(uint32_t id, unsigned char *data, unsigned char len) {
  if (len < 8) return;  // Ensure we have 8 bytes
  // Use a 64-bit container to simplify bit operations.
  if (id == 0x210) {  // ABS_MSG_1 (CAN ID: 0x210)
    // --- Decode 1-bit signals from the incoming message ---
    data[0] = data[0];
    data[1] = data[1];
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = data[5];
    data[6] = data[6];
    data[7] = data[7;]
  }
  else if (id == 0x623) {  // PCM_MSG_11 (CAN ID: 0x623)
    data[0] = 0x36;;
    data[1] = 0x44;
    data[2] = 0x01;
    data[3] = 0x06;
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00
  }
  else if (id == 0x640) {  // PCM_MSG_12 (CAN ID: 0x640)
    data[0] = 0x44;
    data[1] = 0x60;
    data[2] = 0x00;
    data[3] = 0x78;
    data[4] = 0x00;
    data[5] = 0x0C;
    data[6] = 0x86;
    data[7] = 0x00
  }
  else if (id == 0x650) {  // ABS CONFIGURATON NEEDS TO BE SET TO XR8 FOR THIS TO MATCH UP
    switch(alter650)
    {
      case 0:
        data[0] = 0x10;
        data[1] = 0x36;
        data[2] = 0x44;
        data[3] = 0x01;
        data[4] = 0x06;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        alter650++;
        break;
      case 1: 
        data[0] = 0x11;
        data[1] = 0x42;
        data[2] = 0x00;
        data[3] = 0x00;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        alter650 = 0;
        break;

    }
  }
}



// ENDIF
