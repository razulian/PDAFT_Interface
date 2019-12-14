//BUTTON PROCESSING/////////////////////////////////
uint8_t brightnessChangeDelay = 1;

enum ButtonInputs {
  triangleInput = 2,
  squareInput = 4,
  crossInput = 7,
  circleInput = 8,
  startInput = 10
};

enum LightOutputs {
  triangleLight = 3,
  squareLight = 5,
  crossLight = 6,
  circleLight = 9,
  none = 255
};


struct ButtonData {
  uint8_t inputPin;
  uint8_t lightOutputPin;
  boolean state;
  boolean lastState;
  int brightness;
  long brightnessTimer;
};

struct ButtonData buttons[] = {
  {triangleInput, triangleLight, false, false, 0},
  {squareInput, squareLight, false, false, 0},
  {crossInput, crossLight, false, false, 0},
  {circleInput, circleLight, false, false, 0},
  {startInput, none, false, false, 0}
};

uint8_t numButtons = 5;

void initButtons() {
  for (uint8_t i = 0; i < numButtons; i++) {
    pinMode(buttons[i].inputPin, INPUT_PULLUP);
    pinMode(buttons[i].lightOutputPin, OUTPUT);
  }
}



void readButtons() {
  for (uint8_t i = 0; i < numButtons; i++) {
    buttons[i].state = digitalRead(buttons[i].inputPin);
    outputs[i] = !buttons[i].state; //Set outputs
  }

  animateLights();
}

void animateLights() {
  for (uint8_t i = 0; i < numButtons - 1; i++) {
    if ( buttons[i].lastState != buttons[i].state ) {
      if (!buttons[i].state) buttons[i].brightness = 0;
      buttons[i].lastState = buttons[i].state;
    }

    else {
      if (millis() - buttons[i].brightnessTimer > brightnessChangeDelay && buttons[i].brightness < 255) {
        if (buttons[i].state) {
          buttons[i].brightness += 7;
          if (buttons[i].brightness > 255) buttons[i].brightness = 255;
        }
        analogWrite(buttons[i].lightOutputPin, buttons[i].brightness);
        buttons[i].brightnessTimer = millis();
      }
    }
  }
}
