#include "DeviceManager.hpp"

Device DeviceManager::registerDevice(HardwareSerial *send, HardwareSerial *recv) {
    Device device(send, recv);
    device.sendPacket(BEL + "", 0xff);
}