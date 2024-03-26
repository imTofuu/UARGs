#include "Device.hpp"

Device device(/*0x1*/0x2, true);

void setup() {
  device.begin(/*&Serial2*/);
  Packet packet = {"hello", 134, 50, 0};
  uint64_t packetValue = device.getNumericalValue(packet);
  Packet packet2 = device.getPacketValue(packetValue);
  Serial.print("message: ");
  Serial.println(packet2.message);
  Serial.print("target: ");
  Serial.println(packet2.target);
  Serial.print("origin: ");
  Serial.println(packet2.origin);
  Serial.println("");

  device.sendPacket(Packet {"hello", 1, 2, 0});
}

void loop() {
  //Serial.println(device.getPacketValue(device.createPacket("hello", 0x0)), BIN);
  //Serial.println(device.getPacketValue(device.createPacket("hello", 0x0)));
  /*Device::Packet packet = device.getPacket();
  if(packet.origin != 0)
    Serial.println(packet.message);*/
}