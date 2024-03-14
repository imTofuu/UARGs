#include "src/ArgDevice.hpp"

ArgDevice device(true);

void setup() {
  device.begin(&Serial2);

  //device.sendArgs("http arg1 arg2");

  device.addEventListener("http", http);
  device.addEventListener("http2", http);
  device.addEventListener("http3", http);
  device.addEventListener("http4", http);
  Serial.println("startimportant");
  device.addEventListener("http5", http);
  Serial.println("deleting now");
  device.removeEventListener("http", http);
}
void loop() {
  device.update();
}

void http(Args args) {
  Serial.println("http");
}