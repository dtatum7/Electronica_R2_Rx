#include <ADF7242.h>
#include <SPI.h>

//#define DEBUG // Comment out this line to disable DEBUG mode

// Variables
unsigned char roll = 0;
unsigned char pitch = 0;
unsigned char yaw = 0;
unsigned char serialSyncWord = 0xFF; // Used to synchronize serial data received by GUI on PC

ADF7242 Rx(10); // Instantiate ADF7242 Rx(Chip Select)

void setup() {

  // For serial communication to the PC via USB
  Serial.begin(9600); // Baud rate was set arbitrarily

  // Reminder in terminal that you're in DEBUG mode
  #ifdef DEBUG
    Serial.println("**********DEBUG MODE**********");
  #endif

  Rx.reset(); // Reset ADF7242 radio controller during cold start up
  Rx.idle(); // Idle ADF7242 radio controller after cold start up
  // Initialize settings for GFSK/FSK and set data rate
  Rx.initFSK(2); // Data rate [ 1=50kbps, 2=62.5kbps, 3=100kbps, 4=125kbps, 5=250kbps, 6=500kbps, 7=1Mbps, 8=2Mbps ]
  Rx.setMode(0x04); // Set operating mode to GFSK/FSK packet mode
  Rx.chFreq(2450); // Set operating frequency in MHz
  Rx.syncWord(0x00, 0x00); // Set sync word // sync word currently hardcoded
  Rx.cfgPA(3, 0, 7); // Configure power amplifier (power, high power mode, ramp rate)
  Rx.cfgAFC(80); // Writes AFC configuration for GFSK / FSK
  Rx.cfgPB(0x080, 0x000); // Sets Tx/Rx packet buffer pointers
  Rx.cfgCRC(0); // CRC - Disable automatic CRC = 1, else 0
//  Rx.cfgPreamble(0, 0, 0, 1); // FSK preamble configuration
  Rx.PHY_RDY(); // System calibration

  // Clear receive buffer to all 0x00
  for(int i = 0x000; i < 0x005; ++i) {
    Rx.regWrite(i, 0x00);
  }

  Rx.receive(); // for receiving
}

void loop() {
  if((irq1_src1 & 0x08) != 0x00) {

    // Write IMU data to serial port
    #ifndef DEBUG
      roll = Rx.regRead(0x002);
      pitch = Rx.regRead(0x003);
      yaw = Rx.regRead(0x004);
      Serial.write(roll); // Write roll data to serial connection
      Serial.write(pitch); // Write pitch data to serial connection
      Serial.write(yaw); // Write yaw data to serial connection
      Serial.write(0xFF); // Write synchronization word to serial connection
    #endif

    // Receive IMU data via ADF7242
//    delay(1);
    Rx.receive();
  }
  #ifdef DEBUG
    Rx.dumpISB();
    for(int i = 0x000; i < 0x005; ++i) {
      int recPac = Rx.regRead(i);
//      if(recPac != 0x00) {
        Serial.print("Packet buffer contencts for address 0x");
        Serial.print(i, HEX);
        Serial.print(" : 0x");
        Serial.println(recPac, HEX);
//      }
    }
  #endif
}
