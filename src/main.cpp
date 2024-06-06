#include "Arduino.h"
#include "AccelStepper.h"
#include <capsule.h>

#define CAPSULE_ID_POSITION 0x15

#define X_STEP_PIN 12
#define X_DIR_PIN 13

#define HOME_SWITCH_X 7

AccelStepper stepperX(1, X_STEP_PIN, X_DIR_PIN);
void handlePacket(uint8_t, uint8_t*, uint32_t);
CapsuleStatic capsuleRaspberry(handlePacket);

const int stepIncrement = 3;  // Define el tamaño del paso para movimientos incrementales
const long int limite_safe = 5600;  // Límite seguro para el movimiento del motor

void homing(); 


struct dataStruct {
  int position;
};

dataStruct lastCmd;

void setup() {
  Serial.begin(9600);
  pinMode(HOME_SWITCH_X, INPUT_PULLUP);

  stepperX.setMaxSpeed(300);
  stepperX.setAcceleration(100);
  homing();
}

void loop() {
  while(Serial.available()) {
    capsuleRaspberry.decode(Serial.read());
  }
}

void homing() {
  while (digitalRead(HOME_SWITCH_X) == LOW) {
    stepperX.moveTo(stepperX.currentPosition() - 1);
    stepperX.run();
    delay(1);
  }
  stepperX.setCurrentPosition(0);
  Serial.println("Homing completado");
}


void handlePacket(uint8_t id, uint8_t* data, uint32_t size) {

  memcpy(&lastCmd, data, sizeof(dataStruct));

  switch (id) {
    case CAPSULE_ID_POSITION:
      Serial.println("Received packet from Raspberry");
      if (lastCmd.position >= 0 && lastCmd.position <= limite_safe) {
        stepperX.moveTo(lastCmd.position);
        while (stepperX.distanceToGo() != 0) {
          stepperX.run();
        }
        Serial.println("Movimiento completado a posición: " + String(lastCmd.position));
      } else {
        Serial.println("Error: Posición fuera de límites");
      }
    break;
  }
}