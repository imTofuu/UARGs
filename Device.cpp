#include "Device.hpp"

Device::Device(HardwareSerial *send, HardwareSerial *recv, uint8_t address) {
    this->send = send; this ->recv = recv;
}

void Device::anonSendPacket(HardwareSerial *send, HardwareSerial *recv, const char message[8], uint8_t target) {
    Packet packet = {message, target, 0x0, sgenerateHash(message)};
    send->print(STX);
    send->write((char*)&packet, sizeof(Packet));
    send->println(ETX);
    for(int i = 0; i < 500; i++) {
        Packet packet = anonGetPacket(send, recv, 0xfd);
        if(packet.origin)
        delay(10);
    }
}

const Device::Packet Device::anonGetPacket(HardwareSerial *send, HardwareSerial *recv, uint8_t address) {
    char *buffer;
    recv->read();
    recv->readBytes(buffer, sizeof(Packet));
    recv->flush();
    Packet packet = *((Packet*)buffer);
    if(packet.hash != sgenerateHash(packet.message)) {
        
    }
    if(packet.target == address) {
        return packet;
    }
}

void Device::anonSendResponse(HardwareSerial *send, HardwareSerial *recv, Response response) {
    
}

void Device::sendPacket(Packet packet) {
    send->print(STX);
    send->write((char*)&packet, sizeof(Packet));
    send->println(ETX);
}

const Device::Packet Device::getPacket() {
    char *buffer;
    recv->read();
    recv->readBytes(buffer, sizeof(Packet));
    recv->flush();
    Packet packet = *((Packet*)buffer);
    if(packet.target == 0xff || packet.target == address) {
        return packet;
    }
}

const Device::Packet Device::createPacket(const char message[8], uint8_t target) {
    return Packet {message, target, address, generateHash(message)};
}

uint32_t Device::generateHash(const char *message) {
    int hash = 0;
    for(int i = 0; i < 8; i++) {
        hash += message[i] * pow(31, i);
    }
    return hash;
}

uint32_t Device::sgenerateHash(const char *message) {
    int hash = 0;
    for(int i = 0; i < 8; i++) {
        hash += message[i] * pow(31, i);
    }
    return hash;
}