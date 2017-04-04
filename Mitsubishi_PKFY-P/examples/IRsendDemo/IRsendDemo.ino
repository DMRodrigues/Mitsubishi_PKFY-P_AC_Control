/*
  IRsendDemo: demonstrates sending IR codes to PKFY-P series.
  An IR LED must be connected to Arduino PWM pin 3 (with resistor).
  Version 1.0 March, 2017
  Copyright 2017 Diogo Rodrigues
*/

#include <Mitsubishi_PKFY-P.h>

void setup() {
  Serial.begin(115200);
  Serial.println("IR Demo send a cmd To HVAC Mitsubishi PKFY-P");
}

void loop() {
  Serial.println("Switch OFF and Wait 10 Seconds to send ON.");
  IRsend.sendMitsubishi(false, HVAC_HOT, 21, FAN_SPEED_1, VANNE_AUTO_MOVE);
  delay(10000);

  Serial.println("Turn ON: 'HVAC_HOT, 21, FAN_SPEED_1, VANNE_AUTO_MOVE'");
  IRsend.sendMitsubishi(true, HVAC_HOT, 21, FAN_SPEED_1, VANNE_AUTO_MOVE);
  delay(10000);

  Serial.println("Turn ON: 'HVAC_HOT, 25, FAN_SPEED_4, VANNE_H1'");
  IRsend.sendMitsubishi(true, HVAC_HOT, 25, FAN_SPEED_4, VANNE_H1);
  delay(10000);

  Serial.println("Turn ON: 'HVAC_COLD, 17, FAN_SPEED_4, VANNE_H4'");
  IRsend.sendMitsubishi(true, HVAC_COLD, 17, FAN_SPEED_4, VANNE_H4);
  delay(5000);

  Serial.println("Command 'HVAC_AUTO, 30, FAN_SPEED_1, VANNE_AUTO_MOVE'");
  IRsend.sendMitsubishi(true, HVAC_AUTO, 30, FAN_SPEED_1, VANNE_AUTO_MOVE);
  delay(10000);

  Serial.println("Turn OFF: End Of sketch, loop in 15 seconds");
  IRsend.sendMitsubishi(false, HVAC_HOT, 21, FAN_SPEED_2, VANNE_AUTO_MOVE);
  delay(15000); // Let HVAC sleeping
}
