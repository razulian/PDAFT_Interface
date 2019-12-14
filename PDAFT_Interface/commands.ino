//RS232 COMMANDS //////////////////////////
uint8_t readRS232() {
  uint8_t incomingByte = Serial1.read();
  return incomingByte;
}

void singleSliderScan() {
  Serial1.write(0xFF);
  Serial1.write(0x01);
  Serial1.write(0x00);
  Serial1.write(0x00);
}

void enableAutoSliderScan() {
  Serial1.write(0xFF);
  Serial1.write(0x03);
  Serial1.write(0x00);
  Serial1.write(0xFE);
}

void disableAutoSliderScan() {
  Serial1.write(0xFF);
  Serial1.write(0x04);
  Serial1.write(0x00);
  Serial1.write(0xFD);
}

uint8_t calculateReceivedChecksum(uint8_t sliderCommand[3], uint8_t data[32]) {
  uint8_t checksum = 0;

  for (uint8_t i = 0; i < 3; i++) {
    checksum += sliderCommand[i];
  }

  for (uint8_t i = 0; i < 32; i++) {
    checksum += data[i];
  }
  checksum = ~checksum; //Take 2's complement of sum
  checksum++;
  return checksum;
}

uint8_t calculateChecksum(uint8_t data[101]) {
  uint8_t checksum = 0;

  for (uint8_t i = 0; i < 100; i++) {
    checksum += data[i];
  }
  checksum = ~checksum; //Take 2's complement of sum
  checksum++;
  return checksum;
}
