uint8_t sliderScanReceivedByteCounter = 0;
uint8_t  sliderCommand[3] = { 0xFF, 0x01, 0x20};

//SLIDER DATA ACQUISITION ///////////////////////////////
void readSliderData() {
  uint8_t newSliderData[32]; //First 3 bytes are the command bytes, the subsequent 32 bytes is the slider data, the last byte is the checksum

  if (Serial1.available()) {
    switch (sliderScanReceivedByteCounter) {
      case 0:
        if (readRS232() == sliderCommand[0]) sliderScanReceivedByteCounter++;
        else sliderScanReceivedByteCounter = 0;
        break;
      case 1:
        if (readRS232() == sliderCommand[1]) sliderScanReceivedByteCounter++;
        else sliderScanReceivedByteCounter = 0;
        break;
      case 2:
        if (readRS232() == sliderCommand[2]) sliderScanReceivedByteCounter++;
        else sliderScanReceivedByteCounter = 0;
        break;

      default:
        for (uint8_t i = 0; i < 32; ) {
          if (Serial1.available()) {
            newSliderData[i] = readRS232();
            i++;
          }
        }

        uint8_t checksum;
        boolean chkReceived = false;
        while (!chkReceived) {
          if (Serial1.available()) {
            checksum = readRS232(); //Save the received cheksum
            chkReceived = true;
          }
        }
        if (checksum == calculateReceivedChecksum(sliderCommand, newSliderData)) {
          for (uint8_t i = 0; i < 32; i++) {
            if (newSliderData[i] > 222) newSliderData[i] = 0;
            sliderData[i] = newSliderData[i];


           // Serial.print(sliderData[i]);
            //Serial.print(" ");
          }
          //Serial.println();
          whenScanReceived();
          sliderScanReceivedByteCounter = 0;
        }
        break;
    }
  }
}


//SLIDER ANALYSIS ///////////////////////////////
boolean sliderInUse = false;
int currPosA = 255;
int currPosB = 255;
int lastPosA = 255;
int lastPosB = 255;

long timerA, timerB; //The timer is reset every time a move occurs and the trigger delay is calculated by multiplayer decisionDelay with moveTime
int decisionDelay = 200;

int8_t movementDirectionA = 0;
int8_t movementDirectionB = 0;

void sliderAnalysis() {
  boolean sliderInUse_ = false;

  //Check if slider is in use
  for (uint8_t i = 0; i < 32; i++) {
    if (sliderData[i] > 5) {
      sliderData[i] = 1;
      sliderInUse_ = true;
      sliderInUse = true;
    } else {
      sliderData[i] = 0;
    }
  }
  if (!sliderInUse_) sliderInUse = false;

  if (sliderInUse) {

    //Scan left to right to find position A
    for (uint8_t i = 0; i < 32; i++) {
      if (sliderData[i]) {
        if (getDistance(i, currPosB) > 1 && i != currPosB) {
          lastPosA = currPosA;
          currPosA = i;
          break;
        }
      }
    }

    //Scan right to left to find position B
    for (int8_t i = 31; i >= 0; i--) {
      if (sliderData[i]) {
        if (getDistance(i, currPosA) > 1 && i != currPosA) {
          lastPosB = currPosB;
          currPosB = i;
          break;
        }
      }
    }

    //Get Direction A
    if (lastPosA != currPosA && currPosA != 255 && lastPosA != 255 && getDistance(lastPosA, currPosA) < 2) {
      if (lastPosA < currPosA) movementDirectionA = 1;
      else movementDirectionA = -1;
      timerA = millis();
    } else {
      if (millis() - timerA > decisionDelay) {
        movementDirectionA = 0;
      }
    }

    //Get Direction B
    if (lastPosB != currPosB && currPosB != 255 && lastPosB != 255 && getDistance(lastPosB, currPosB) < 2 ) {
      if (lastPosB < currPosB) movementDirectionB = 1;
      else movementDirectionB = -1;
      timerB = millis();
    } else {
      if (millis() - timerB > decisionDelay) {
        movementDirectionB = 0;
      }
    }

    //Make sure to move B to A when only one touchpoint is being used.
    int8_t touchPointA = -1;
    int8_t touchPointB = -1;
    if (currPosA != 255 && currPosB != 255) {
      for (uint8_t i = 0; i < 32; i++) {
        if (sliderData[i]) {

          if (touchPointA < 0) {
            touchPointA = i;
          }
          else {
            if (getDistance(touchPointA , i)) {
              touchPointB = i;
            }
          }
        }
      }
      if (touchPointB == -1 && currPosB != 255) {
        movementDirectionA = movementDirectionB;
        currPosA = currPosB;
        lastPosA = lastPosB;
        timerA = timerB;
        currPosB = 255;
        lastPosB = 255;
      }
    }
  }

  //Reset if slider is not being used
  if (!sliderInUse) {
    currPosA = 255;
    lastPosA = 255;
    currPosB = 255;
    lastPosB = 255;
    movementDirectionA = 0;
    movementDirectionB = 0;
  }

  outputs[5] = movementDirectionA;
  outputs[6] = movementDirectionB;
}

int getDistance(int a, int b) {
  int distance = a - b;
  if (distance < 0) distance = -distance;
  return distance;
}
