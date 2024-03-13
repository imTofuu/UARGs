#include "src/ArgDevice.hpp"

ArgDevice device(false);

void setup() {
  device.begin();

  device.addEventListener("http", http);
}
void loop() {
  device.update();
}

void http(Args args) {
  Serial.println("http");
}