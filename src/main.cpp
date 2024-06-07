#include <Arduino.h>
#include <AccelStepper.h>
#include <capsule.h>

#define CAPSULE_ID_POSITION 0x15
#define CAPSULE_ID_GUSTAVO_CUSTOM 0x21

#define X_STEP_PIN 12
#define X_DIR_PIN 11

#define HOME_SWITCH_X 7

AccelStepper stepperX(1, X_STEP_PIN, X_DIR_PIN);
void handlePacket(uint8_t, uint8_t*, uint32_t);
CapsuleStatic capsuleRaspberry(handlePacket);

// const int stepIncrement = 3;  // Define el tamaño del paso para movimientos incrementales
const long int limite_safe = 5600;  // Límite seguro para el movimiento del motor

void homing(); 

struct dataStruct {
  int position;
};
 
struct dataStructGustavoCustom {
  int val1;
  int val2;
  float val3;
};

dataStruct lastCmd;
dataStructGustavoCustom lastCmdGustavoCustom;

void setup() {
  Serial.begin(115200);
  pinMode(HOME_SWITCH_X, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  stepperX.setMaxSpeed(5000);
  stepperX.setAcceleration(100);

  // stepperX.moveTo(0);
  // stepperX.runToPosition();
  // delay(1000);
  // stepperX.moveTo(10000);
  // stepperX.runToPosition();
  // delay(1000);

  homing();
}

void loop() {
  while(Serial.available()) {
    capsuleRaspberry.decode(Serial.read());
  }
}

void homing() {
  // Serial.println("Starting homing");
  while (digitalRead(HOME_SWITCH_X) == LOW) {
    // Serial.println("Homing...");
    // stepperX.moveTo(stepperX.currentPosition() - 10);
    stepperX.setSpeed(-5000);
    stepperX.runSpeed();
    delay(1);
  }
  stepperX.setCurrentPosition(0);
  Serial.println("Homing completed");
}


void handlePacket(uint8_t id, uint8_t* data, uint32_t size) {

  digitalWrite(LED_BUILTIN, HIGH);

  switch (id) {
    case CAPSULE_ID_POSITION:
      memcpy(&lastCmd, data, sizeof(dataStruct));
      Serial.println("Received packet from Raspberry");
      if (lastCmd.position >= 0 && lastCmd.position <= limite_safe) {
        stepperX.moveTo(lastCmd.position);
        while (stepperX.distanceToGo() != 0) {
          stepperX.run();
        }
        // Serial.println("Movimiento completado a posición: " + String(lastCmd.position));
      } else {
        // Serial.println("Error: Posición fuera de límites");
      }
    break;

    case CAPSULE_ID_GUSTAVO_CUSTOM:
      memcpy(&lastCmdGustavoCustom, data, sizeof(dataStructGustavoCustom));
      Serial.println("Received packet from Raspberry Custom Gustavo");
      Serial.println("Val1: " + String(lastCmdGustavoCustom.val1));
      Serial.println("Val2: " + String(lastCmdGustavoCustom.val2));
      Serial.println("Val3: " + String(lastCmdGustavoCustom.val3));
    break;
  }

  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
}