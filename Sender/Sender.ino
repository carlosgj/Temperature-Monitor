#include <Adafruit_SleepyDog.h>
#include <OneWire.h>
#include <SPI.h>
#include <RH_RF69.h>

//#define DO_SERIAL
#define DO_SLEEP

#define PERIOD 30000

#ifdef DO_SLEEP
#undef DO_SERIAL
#endif

#if defined (__AVR_ATmega32U4__) // Feather 32u4 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     7
  #define RFM69_RST     4
  #define LED           13
#endif

#define RF69_FREQ 915.0

RH_RF69 rf69(RFM69_CS, RFM69_INT);
OneWire myWire(11);

uint16_t packetnum = 0;  // packet counter, we increment per xmission
uint8_t oneWireAddr[8];

void setup() {
  pinMode(LED, OUTPUT);
  Blink(LED, 500, 2);
#ifdef DO_SERIAL
  Serial.begin(115200);
  while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer
#endif

  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  #ifdef DO_SERIAL
  Serial.println("Feather RFM69 TX Test!");
  Serial.println();
  #endif

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69.init()) {
    #ifdef DO_SERIAL
    Serial.println("RFM69 radio init failed");
    #endif
    while (1);
  }
  #ifdef DO_SERIAL
  Serial.println("RFM69 radio init OK!");
  #endif
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    #ifdef DO_SERIAL
    Serial.println("setFrequency failed");
    #endif
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x06, 0x06,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x06, 0x06};
  rf69.setEncryptionKey(key);
  
  #ifdef DO_SERIAL
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
  #endif

  for(uint8_t i=0; i<8; i++){
    oneWireAddr[i] = 0;
  }
  myWire.reset_search();
  myWire.search(oneWireAddr);
  #ifdef DO_SERIAL
  Serial.print("Found address: ");
  for(uint8_t i=0; i<8; i++){
    Serial.print(oneWireAddr[i], HEX);
  }
  Serial.println();
  #endif
}

void loop() {
  for(uint8_t i=0; i<8; i++){
    oneWireAddr[i] = 0;
  }
  myWire.reset_search();
  myWire.search(oneWireAddr);
  #ifdef DO_SERIAL
  Serial.print("Found address: ");
  for(uint8_t i=0; i<8; i++){
    Serial.print(oneWireAddr[i], HEX);
  }
  Serial.println();
  #endif


  
  uint16_t temp = readTemperature();
  uint16_t battVoltage = analogRead(A9);
  uint16_t solarVoltage = analogRead(A0);
  uint16_t intTemp = analogRead(A2);

  unsigned char radiopacket[11];
  //itoa(packetnum++, radiopacket+13, 10);
  *((uint16_t *)(&(radiopacket[0]))) = packetnum++;
  *((uint16_t *)(&(radiopacket[2]))) = temp;
  *((uint16_t *)(&(radiopacket[4]))) = battVoltage;
  *((uint16_t *)(&(radiopacket[6]))) = solarVoltage;
  *((uint16_t *)(&(radiopacket[8]))) = intTemp;

  //Generate checksum
  uint8_t checksum = 0;
  for(uint8_t i=0; i<10; i++){
    checksum ^= radiopacket[i];
  }
  radiopacket[10] = checksum;

  #ifdef DO_SERIAL
  Serial.print("Sending ");
  char buf2[4];
  for(unsigned char i=0; i<11; i++){
    itoa(radiopacket[i], buf2, 16);
    if(buf2[1] == 0){
      //One character
      buf2[1] = ' ';
      buf2[2] = 0;
    }
    else{
      buf2[2] = ' ';
      buf2[3] = 0;
    }
    Serial.print(buf2);
  }
  Serial.println();
  #endif
  
  // Send a message!
  rf69.send((uint8_t *)radiopacket, 11);
  rf69.waitPacketSent();

  // Now wait for a reply
  uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf69.waitAvailableTimeout(1000))  { 
    // Should be a reply message for us now   
    if (rf69.recv(buf, &len)) {
      uint16_t reply;
      reply = *((uint16_t *)buf);
      if(reply == (packetnum-1)){
        #ifdef DO_SERIAL
        Serial.println("Good ACK");
        #endif
        Blink(LED, 50, 2); //blink LED 3 times, 50ms between blinks
      }
      else{
        #ifdef DO_SERIAL
        Serial.print("Incorrect ACK ");
        Serial.println(reply);
        #endif
      }
    } else {
      #ifdef DO_SERIAL
      Serial.println("Receive failed");
      #endif
    }
  } else {
    #ifdef DO_SERIAL
    Serial.println("No reply, is another RFM69 listening?");
    #endif
  }

#ifdef DO_SLEEP
  rf69.sleep();
  Watchdog.sleep(PERIOD);
#else
  delay(PERIOD);  // Wait 1 second between transmits, could also 'sleep' here!
#endif
}

void Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i=0; i<loops; i++)  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}

uint16_t readTemperature(void){
  myWire.reset();
  myWire.select(oneWireAddr);
  myWire.write(0x44);
  delay(1000);
  myWire.reset();
  myWire.select(oneWireAddr);
  myWire.write(0xbe);
  uint8_t result[9];
  for(uint8_t i=0; i<9; i++){
    result[i] = myWire.read();
  }
  #ifdef DO_SERIAL
  Serial.print("Got: ");
  for(uint8_t i=0; i<9; i++){
    Serial.print(result[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  #endif
  
  int16_t temp; 
  temp = *((int16_t *)result);
  #ifdef DO_SERIAL
  Serial.print("Temp: ");
  Serial.println(temp, HEX);
  #endif
  return temp;
}
