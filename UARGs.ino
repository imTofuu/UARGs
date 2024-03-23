#include "Device.hpp"

Device device(/*0x1*/0x2, true);

void setup() {
  device.begin(/*&Serial2*/);
}

void loop() {
  //device.sendPacket(device.createPacket("im here", 0x2));
  Device::Packet packet = device.getPacket();
  if(packet.origin != 0)
    Serial.println(packet.message);
}