#include "Device.hpp"

Device device(0x01, true);

void setup() {
  device.begin();

  Serial.println("");
  Serial.println("");

  char message[] = "hello";

  device.sendMessage(device.createMessage(message, sizeof(message), 0x02));
}

void loop() {
  device.update();
}