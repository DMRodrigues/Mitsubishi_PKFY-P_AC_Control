/*
  IRsendDemo: demonstrates sending IR codes to PKFY-P.
  The commands follow this order: "state;mode;temp;fan;vane".
  Example: "1;1;17;1;1" => state: ON, mode: HVAC_COLD, temperature: 17, fan: FAN_SPEED_1, vane: VANNE_H1
  An IR LED must be connected to Arduino PWM pin 3 (with resistor).
  Version 1.0 March, 2017
  Copyright 2017 Diogo Rodrigues
*/

#include <Mitsubishi_PKFY-P.h>


#define INPUT_SIZE 9
char input[INPUT_SIZE + 1]; // because of "strtok" function


/* global variables that are udpated and maintained */
boolean state = false;                /* On - Off */
HvacMode mode_state = HVAC_COLD;      /* 1 - 5 */
uint8_t tempAC = 21;                  /* 17 - 30 */
HvacFanMode fan_state = FAN_SPEED_1;  /* 1 - 4 */
HvacVanneMode vane_state = VANNE_H1;  /* 1 - 4, 5(AUTO) */


void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to Mitsubishi PKFY-P controller");
}

void loop() {
  if (Serial.available()) {
    byte size = Serial.readBytes(input, INPUT_SIZE);
    if (size == INPUT_SIZE) {
      input[size] = '\0';
      processCommandInt(input);
      IRsend.sendMitsubishi(state, mode_state, tempAC, fan_state, vane_state);
    }
  }

  /* other things */
}


void processCommandInt(char *input) {
  // Read each command pair
  char* command = strtok(input, ";");
  setCommandState(command);
  command = strtok(NULL, ";"); // Find the next command in input string
  setCommandMode(command);
  command = strtok(NULL, ";"); // Find the next command in input string
  setCommandTemp(command);
  command = strtok(NULL, ";"); // Find the next command in input string
  setCommandFan(command);
  command = strtok(NULL, ";"); // Find the next command in input string
  setCommandVane(command);
}


void setCommandState(char* s) {
  if (atol(s) == 1)
    state = true;
  else
    state = false;
}


/* Mapping
    mode_state = 1 = Cold;
    mode_state = 2 = Dry;
    mode_state = 3 = Auto;
    mode_state = 4 = Fan;
    mode_state = 5 = Heat;
*/
void setCommandMode(char *s) {
  switch (atol(s)) {
    case 1:
      mode_state = HVAC_COLD;
      break;
    case 2:
      mode_state = HVAC_DRY;
      break;
    case 3:
      mode_state = HVAC_AUTO;
      break;
    case 4:
      mode_state = HVAC_FAN;
      break;
    case 5:
      mode_state = HVAC_HOT;
      break;
    default:
      mode_state = HVAC_AUTO;
      break;
  }
}


void setCommandTemp(char *s) {
  tempAC = atol(s);
}


/* Mapping
    fan_state = 1 = FAN_SPEED_1;
    fan_state = 2 = FAN_SPEED_2;
    fan_state = 3 = FAN_SPEED_3;
    fan_state = 4 = FAN_SPEED_4;
*/
void setCommandFan(char *s) {
  switch (atol(s)) {
    case 1:
      fan_state = FAN_SPEED_1;
      break;
    case 2:
      fan_state = FAN_SPEED_2;
      break;
    case 3:
      fan_state = FAN_SPEED_3;
      break;
    case 4:
      fan_state = FAN_SPEED_4;
      break;
    default:
      fan_state = FAN_SPEED_1;
      break;
  }
}


/* Mapping
    vane_state = 1 = Cold;
    vane_state = 2 = Dry;
    vane_state = 3 = Auto;
    vane_state = 4 = Fan;
    vane_state = 5 = Heat;
*/
void setCommandVane(char *s) {
  switch (atol(s)) {
    case 1:
      vane_state = VANNE_H1;
      break;
    case 2:
      vane_state = VANNE_H2;
      break;
    case 3:
      vane_state = VANNE_H3;
      break;
    case 4:
      vane_state = VANNE_H4;
      break;
    case 5:
      vane_state = VANNE_AUTO_MOVE;
      break;
    default:
      vane_state = VANNE_H1;
      break;
  }
}

