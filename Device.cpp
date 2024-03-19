#include "Device.hpp"

Device::Device(HardwareSerial *send, HardwareSerial *recv) {
    this->send = send; this ->recv = recv;
    send->begin(57600); recv->begin(57600);
}

void Device::sendPacket(Packet packet) {
    send->print(STX);
    send->write((char*)&packet, sizeof(Packet));
    send->println(ETX);
}

const Device::Packet Device::getPacket() {
    
    char *buffer;
    recv->readBytes(buffer, sizeof(Packet));
    return *((Packet*)buffer);
}

const Device::Packet Device::createPacket(const char message[8], uint8_t target) {
    return Packet {message, target, generateHash(message)};
}

uint32_t Device::generateHash(const char *message) {
    int hash = 0;
    for(int i = 0; i < 8; i++) {
        hash += message[i] * pow(31, i);
    }
    return hash;
}