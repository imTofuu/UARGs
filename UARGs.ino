#include "Device.hpp"

Device device(0x1);

void setup() {
  Serial.begin(115200);
  Serial.println("0");
  device.begin(&Serial2);
  Serial.println("got here2");
  //Serial.println("here");
  //Serial.println(device.getAddress());
  //device.sendPacket(device.createPacket("im here", 0x2));
}

void loop() {
  Serial.println("loop");
  //Serial.println(device.getAddress());
}