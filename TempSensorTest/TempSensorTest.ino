#include <OneWire.h>

OneWire myWire(11);

#define SERIAL_ENABLE

uint8_t addr[8];

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
  for(uint8_t i=0; i<8; i++){
    addr[i] = 0;
  }
  myWire.reset_search();
  myWire.search(addr);

  Serial.print("Found address: ");
  for(uint8_t i=0; i<8; i++){
    Serial.print(addr[i], HEX);
  }
  Serial.println();

  //Write config register
  myWire.reset();
  myWire.select(addr);
  

}

void loop() {
  myWire.reset();
  myWire.select(addr);
  myWire.write(0x44);
  delay(1000);
  myWire.reset();
  myWire.select(addr);
  myWire.write(0xbe);
  uint8_t result[9];
  for(uint8_t i=0; i<9; i++){
    result[i] = myWire.read();
  }
  Serial.print("Got: ");
  for(uint8_t i=0; i<9; i++){
    Serial.print(result[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  int16_t temp; 
  temp = *((int16_t *)result);
  Serial.print("Temp: ");
  Serial.println(temp, HEX);
}
