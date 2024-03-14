#include "src/ArgDevice.hpp"

ArgDevice device(true);

void setup() {
  device.begin(&Serial2);

  //device.sendArgs("http arg1 arg2");

  device.addEventListener("http", http);
}
void loop() {
  device.update();
}

void http(Args args) {
  Serial.println("http");
}