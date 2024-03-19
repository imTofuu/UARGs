#include "DeviceManager.hpp"

void setup() {
  Device device = DeviceManager::registerDevice();
  device.sendPacket("hello", 0xff);
}

void loop() {

}