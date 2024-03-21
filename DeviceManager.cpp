#include "DeviceManager.hpp"

Device DeviceManager::registerDevice(HardwareSerial *send, HardwareSerial *recv) {
    send->begin(57600); recv->begin(57600);

    Device::anonSendPacket(send, BEL + "", 0xff);
    uint8_t highestAddress = 0;
    for(int i = 0; i < 100; i++) {
        Device::Packet packet = Device::anonGetPacket(recv, 0xfe);
        if(packet.origin > highestAddress)
            highestAddress = packet.origin;
        delay(50);
    }
    Device device(send, recv, highestAddress + 1);
}