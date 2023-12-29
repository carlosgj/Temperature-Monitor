// rf69 demo tx rx.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem
// configuration

#include <SPI.h>
#include <RH_RF69.h>

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0

#if defined (__AVR_ATmega32U4__) // Feather 32u4 w/Radio
#define RFM69_CS      8
#define RFM69_INT     7
#define RFM69_RST     4
#define LED           13
#endif

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetCount = 0;  // packet counter, we increment per xmission

void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600);
  //while (!Serial) {
  //  delay(1);  // wait until serial console is open, remove if not tethered to computer
 // }

  pinMode(LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 RX Test!");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x06, 0x06,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x06, 0x06
                  };
  rf69.setEncryptionKey(key);

  pinMode(LED, OUTPUT);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}


void loop() {
  if (rf69.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;
      //Serial.print("Received [");
      //Serial.print(len);
      //Serial.print("]: ");
      //char buf2[4];
      //for (unsigned char i = 0; i < len; i++) {
      //  itoa(buf[i], buf2, 16);
      //  if (buf2[1] == 0) {
      //    //One character
      //    buf2[1] = ' ';
      //    buf2[2] = 0;
      //  }
      //  else {
      //    buf2[2] = ' ';
      //    buf2[3] = 0;
      //  }
      //  Serial.print(buf2);
      //}
      //Serial.print("RSSI: ");
      //Serial.println(rf69.lastRssi(), DEC);

      //Validate packet
      if (len == 11) {
        uint8_t checksum = 0;
        for (uint8_t i = 0; i < 10; i++) {
          checksum ^= buf[i];
        }

        if (buf[10] == checksum) {
          //Serial.println("Good packet");
          packetCount++;
          uint16_t senderPacketNum;
          senderPacketNum = *((uint16_t *)(buf));
          Serial.write(buf, 10);
          Serial1.write(buf, 10);
          Serial.write((uint8_t *)(&packetCount), 2);
          Serial1.write((uint8_t *)(&packetCount), 2);
          Serial.write(rf69.lastRssi());
          Serial1.write(rf69.lastRssi());
          //Serial.print('#');
          //Serial.println(packetNum);
          // Send an ACK: packet num
          //uint8_t data[2];
          //*((uint16_t *)data) = senderPacketNum;
          //rf69.send(data, sizeof(data));
          //rf69.waitPacketSent();
          //Serial.println("Sent a reply");
          Blink(LED, 40, 3); //blink LED 3 times, 40ms between blinks
        }
        else{
          Serial.println("Bad checksum");
        }
      }
      else {
        //Serial.println("Bad length");
      }

    } else {
      Serial.println("Receive failed");
    }
  }
}


void Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i = 0; i < loops; i++)  {
    digitalWrite(PIN, HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN, LOW);
    delay(DELAY_MS);
  }
}
