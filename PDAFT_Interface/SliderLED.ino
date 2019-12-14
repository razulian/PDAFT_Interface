//ANIMATION VARIABLES/////////////////////////////////
boolean scanFinished = false;
uint8_t LEDbrightness = 255;
boolean touching = false;

long idleTimer;
uint16_t timeUntilIdle = 3000;
uint8_t oldLEDData[101];

void showSliderLED() {
  LEDData[100] = calculateChecksum(LEDData);
  for (int i = 0; i < 101; i++) {
    Serial1.write(LEDData[i]);
  }
}

void setSliderLED(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
  i = 3 * i;
  LEDData[i + 4] = b;
  LEDData[i + 5] = r;
  LEDData[i + 6] = g;
}

void setSliderLED(uint16_t n, uint32_t c) {
  uint8_t *p,
          r = (uint8_t)(c >> 16),
          g = (uint8_t)(c >>  8),
          b = (uint8_t)c;
  setSliderLED(n, r, g, b);
}

//TOUCH ANIMATION CHOOSER/////////////////////////////////
void sliderBarTouchReadAndAnimate() {
  if (!touching && millis() - idleTimer > timeUntilIdle ) {
    rainbowCycle();
  } else {
    breathe();
  }


  touchLEDReact();

  if (touching) {
    LEDbrightness = 50;
    idleTimer = millis();
  } else {
    LEDbrightness = 255;
  }


  if (oldLEDData != LEDData) {
    showSliderLED();
    for(uint8_t i = 0; i < 101; i++){
      oldLEDData[i] = LEDData[i];
    }
  }

}



//TOUCH ANIMATION/////////////////////////////////
uint8_t animStartPos;
uint8_t animStopPos;
uint8_t animWidth = 2;
uint8_t animWidthAdd = 1;
uint8_t animWidthMin = 3;
uint8_t animWidthMax = 10;

long animTimer;
uint8_t animDelay = 20;

void touchLEDReact() {
  touching = false;
  for (uint8_t i = 0; i < 32; i++) {
    if (sliderData[i] > 10) {
      setSliderLED(i, 40, 100, 100);
      if (!touching) {
        animStartPos = i;
        animStopPos = animStartPos;
      }
      if (i > animStartPos) animStopPos = i;

      animStartPos = i - animWidthMin;
      animStopPos = i + animWidthMin;

      touching = true;
    }
  }

  if (touching) {

    if (millis() - animTimer > animDelay) {
      if (animWidth > animWidthMax || animWidth < 0) animWidthAdd = -animWidthAdd;
      animWidth += animWidthAdd;
      animTimer = millis();
    }

    animStartPos = animStartPos - animWidth;
    if (animStartPos <  0 || animStartPos >  31) animStartPos = 0;

    animStopPos = animStopPos + animWidth;
    if (animStopPos > 31) animStopPos = 31;

    for (uint8_t i = animStartPos; i <= animStopPos; i++) {
      setSliderLED(i, 3, 8, 4);
    }

    for (uint8_t i = 0; i < 32; i++) {
      if (sliderData[i] > 10) {
        setSliderLED(i, 120, 150, 150);
      }
    }

  }
}

//BREATHE/////////////////////////////////
long breatheTimer = 0;
uint8_t breatheDelay = 20;
uint8_t breatheBrightnessMax = 170;
uint8_t breatheBrightnessMin = 40;
uint8_t breatheBrightness = 180;
uint8_t breatheBrightnessAdd = 2;
void breathe() {

  if (millis() - breatheTimer > breatheDelay) {
    for (uint8_t i = 0; i < 32; i++) {
      setSliderLED(i, 130 * breatheBrightness / 255 * LEDbrightness / 255, 80 * breatheBrightness / 255 * LEDbrightness / 255, 60 * breatheBrightness / 255 * LEDbrightness / 255);
    }

    breatheBrightness += breatheBrightnessAdd;

    if (breatheBrightness < breatheBrightnessMin) breatheBrightness = breatheBrightnessMin;
    if (breatheBrightness > breatheBrightnessMax) breatheBrightness = breatheBrightnessMax;

    if (breatheBrightness <= breatheBrightnessMin || breatheBrightness >= breatheBrightnessMax) {
      breatheBrightnessAdd = -breatheBrightnessAdd;
    }


    breatheTimer = millis();
  }
}

//RAINBOW/////////////////////////////////
uint8_t rainbowBrightness = 255;
const uint8_t rainbowWaitTime = 3;
long rainbowTimer = 0;
uint16_t rainboxWheelCounter = 0;
void rainbowCycle() {

  if (millis() - rainbowTimer > rainbowWaitTime) {
    if (rainboxWheelCounter < 256) {
      for (uint8_t i = 0; i < 32; i++) {
        setSliderLED(i, Wheel(((i * 8) + rainboxWheelCounter) & 255));

        uint8_t x = 3 * i;
        LEDData[x + 4] = LEDData[x + 4];
        LEDData[x + 5] = LEDData[x + 5] + LEDData[x + 6];
        LEDData[x + 6] = LEDData[x + 6] + LEDData[x + 4];

        LEDData[x + 4] = LEDData[x + 4] / 4 * rainbowBrightness / 255 * LEDbrightness / 255;
        LEDData[x + 5] = LEDData[x + 5] / 2 * rainbowBrightness / 255 * LEDbrightness / 255;
        LEDData[x + 6] = LEDData[x + 6] / 2 * rainbowBrightness / 255 * LEDbrightness / 255;
      }
      rainboxWheelCounter++;
    }
    else {
      rainboxWheelCounter = 0;
    }
    rainbowTimer = millis();
  }
}


uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return packedColor(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return packedColor(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return packedColor(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint32_t packedColor(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}
