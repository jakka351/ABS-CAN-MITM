#include <Wire.h>
#include <stdio.h>
#include "canbed_dual.h"

// Enable or disable debug output
#define DEBUG 1

// Create CAN bus objects
CANBedDual PCMCAN(0);
CANBedDual ABSCAN(1);

// Define ID arrays and constants (global scope)
static const uint32_t retransmitOntoPcmBusIDs[] = {
  0x70,   // YAW_MSG_1
  0x75,   // YAW_MSG_2
  0x80,   // SAS_MSG_1
  0x85,   // YAW_MSG_3
  0x90,   // SAS_MSG_2
  0x210,  // ABS_MSG_1  (edit)
  0x4B0,  // TCS_MSG_1
  0x768  
};

static const uint32_t retransmitOntoAbsBusIDs[] = { 
  0x97,    // PCM_MSG_1
  0xFC,    // PCM_MSG_2
  0x120,   // PCM_MSG_3
  0x12D,   // PCM_MSG_4
  0x200,   // PCM_MSG_5
  0x207,   // PCM_MSG_6
  0x230,   // PCM_MSG_7
  0x425,   // PCM_MSG_15
  0x623,   // PCM_MSG_11 (edit)
  0x640,   // PCM_MSG_12 (edit)
  0x650,   // PCM_MSG_13 (edit)
  0x760
};

static const uint32_t editPCM[] = {  // CAN IDs on PCM bus to edit
  0x623,  // PCM_MSG_11 (edit)
  0x640,  // PCM_MSG_12 (edit)
  0x650   // PCM_MSG_13 (edit)
};

static const uint32_t editABS[] = {  // CAN IDs on ABS bus to edit
  0x210   // PCM_MSG_1
};

static const int PCMIDs    = sizeof(retransmitOntoPcmBusIDs) / sizeof(retransmitOntoPcmBusIDs[0]);
static const int ABSIDs    = sizeof(retransmitOntoAbsBusIDs) / sizeof(retransmitOntoAbsBusIDs[0]);
static const int numPCMIDs = sizeof(editPCM) / sizeof(editPCM[0]);
static const int numABSIDs = sizeof(editABS) / sizeof(editABS[0]);

// Function prototypes
bool isEditPCM(uint32_t id);
bool isEditAbs(uint32_t id);
bool retransmitPcm(uint32_t id);
bool retransmitAbs(uint32_t id);
void editData(uint32_t id, uint8_t *data, uint8_t len);
void processCAN(CANBedDual &source, CANBedDual &dest, bool (*shouldRetransmit)(uint32_t), bool (*shouldEdit)(uint32_t));

// Global variable for alternating data for CAN ID 0x650
static int alter650 = 0;

void setup() {
  Serial.begin(115200);
  // Initialize both CAN buses (ABS CAN requires a 120 ohm termination resistor)
  PCMCAN.init(500000);  // 500 kb/s
  ABSCAN.init(500000);  // 500 kb/s
}

// Returns true if the given ID is found in the editPCM array.
bool isEditPCM(uint32_t id) {
  for (int i = 0; i < numPCMIDs; i++) {
    if (id == editPCM[i])
      return true;
  }
  return false;
}

// Returns true if the given ID is found in the editABS array.
bool isEditAbs(uint32_t id) {
  for (int i = 0; i < numABSIDs; i++) {
    if (id == editABS[i])
      return true;
  }
  return false;
}

// Returns true if the given ID is found in the retransmitOntoPcmBusIDs array.
bool retransmitPcm(uint32_t id) {
  for (int i = 0; i < PCMIDs; i++) {
    if (id == retransmitOntoPcmBusIDs[i])
      return true;
  }
  return false;
}

// Returns true if the given ID is found in the retransmitOntoAbsBusIDs array.
bool retransmitAbs(uint32_t id) {
  for (int i = 0; i < ABSIDs; i++) {
    if (id == retransmitOntoAbsBusIDs[i])
      return true;
  }
  return false;
}

// Edit the data in place based on the CAN ID.
void editData(uint32_t id, uint8_t *data, uint8_t len) {
  if (len < 8) return;  // Ensure full 8-byte message for editing

  if (id == 0x210) {  // ABS_MSG_1
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
  }
  else if (id == 0x623) {  // PCM_MSG_11
    data[0] = 0x36;
    data[1] = 0x44;
    data[2] = 0x01;
    data[3] = 0x06;
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00;
  }
  else if (id == 0x640) {  // PCM_MSG_12
    data[0] = 0x44;
    data[1] = 0x60;
    data[2] = 0x00;
    data[3] = 0x78;
    data[4] = 0x00;
    data[5] = 0x0C;
    data[6] = 0x86;
    data[7] = 0x00;
  }
  else if (id == 0x650) {  // PCM_MSG_13: Alternate configuration for XR8 match-up
    if (alter650 == 0) {
      data[0] = 0x10;
      data[1] = 0x36;
      data[2] = 0x44;
      data[3] = 0x01;
      data[4] = 0x06;
      data[5] = 0x00;
      data[6] = 0x00;
      data[7] = 0x00;
      alter650 = 1;
    } else {
      data[0] = 0x11;
      data[1] = 0x42;
      data[2] = 0x00;
      data[3] = 0x00;
      data[4] = 0x00;
      data[5] = 0x00;
      data[6] = 0x00;
      data[7] = 0x00;
      alter650 = 0;
    }
  }
}

// Generalized function that reads a message from the source bus and, if applicable,
// edits and retransmits it to the destination bus.
void processCAN(CANBedDual &source, CANBedDual &dest, bool (*shouldRetransmit)(uint32_t), bool (*shouldEdit)(uint32_t)) {
  unsigned long id = 0;
  int ext = 0, rtr = 0, fd = 0, len = 0;
  uint8_t data[8] = {0};

  if (source.read(&id, &ext, &rtr, &fd, &len, data)) {
#ifdef DEBUG
    Serial.print("Received CAN msg: id=0x");
    Serial.print(id, HEX);
    Serial.print(" len=");
    Serial.println(len);
    for (int i = 0; i < len; i++) {
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
#endif
    if (shouldRetransmit(id)) {
      if (shouldEdit(id)) {
        editData(id, data, len);
      }
      dest.send(id, 0, 0, 0, len, data);
    }
  }
}

void loop() {
  // Process messages from PCM bus to ABS bus.
  processCAN(PCMCAN, ABSCAN, retransmitAbs, isEditPCM);
  // Process messages from ABS bus to PCM bus.
  processCAN(ABSCAN, PCMCAN, retransmitPcm, isEditAbs);
}
