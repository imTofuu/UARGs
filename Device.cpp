#include "Device.hpp"

Device::Device(uint8_t address) {
    this->address = address;
}

void Device::update() {

}

//------------------------------------------------------------------------------------
// Normal packets
//------------------------------------------------------------------------------------

void Device::sendPacket(Packet packet) {
    send->print(STX);
    send->write((char*)&packet, sizeof(Packet));
    send->println(ETX);
    for(int i = 0; i < 500; i++) {
        delay(10);
        Response response = getResponse();
        if(response.code == 0) continue;
        if(response.code == ROK) {
            break;
        } else {
            sendPacket(createPacket(packet.message, packet.target));
            break;
        }
    }
    send->flush();
}

const Device::Packet Device::getPacket() {
    if(recv->available() && recv->peek() == STX) {

        Packet packet = readPacket();

        if(packet.hash != generateHash(packet.message)) {
            sendResponse({RHASH, packet.origin});
            for(int i = 0; i < 500; i++) {
                delay(10);
                const Packet packet2 = getPacket();
                if(packet2.origin != 0)
                    return packet2;
            }
            return NULLPKT;
        }
        if(packet.target == address) {
            uint8_t responseAddress = packet.origin + 128;
            sendResponse({ROK, responseAddress});
            return packet;
        }
    }
    recv->flush();
    return NULLPKT;
}

const Device::Packet Device::readPacket() {
    recv->read();
    char buffer[sizeof(Packet)];
    for(int i = 0; i < sizeof(Packet); i++) {
        char currentByte = recv->read();
        if(currentByte == ETX) {
            return NULLPKT;
        }
        buffer[i] = currentByte;
    }
    recv->flush();
    return *((Packet*)buffer);
}

const Device::Packet Device::createPacket(const char message[8], uint8_t target) {
    return Packet {message, target, address, generateHash(message)};
}

//------------------------------------------------------------------------------------
//Normal responses
//------------------------------------------------------------------------------------

void Device::sendResponse(Response response) {
    send->print(SOH);
    send->write((char*)&response, sizeof(Response));
    send->println(ETX);   
}

const Device::Response Device::getResponse() {
    if(recv->available() && recv->peek() == SOH) {
        Response response = readResponse();
        if(response.target == address + 128) 
            return response;
    }
    return NULLRSP;
}

const Device::Response Device::readResponse() {
    recv->read();
    char buffer[sizeof(Response)];
    for(int i = 0; i < sizeof(Response); i++) {
        char currentByte = recv->read();
        if(currentByte == ETX) {
            return NULLRSP;
        }
        buffer[i] = currentByte;
    }
    recv->flush();
    return *((Response*)buffer);
}

//------------------------------------------------------------------------------------
//Other stuff
//------------------------------------------------------------------------------------

uint32_t Device::generateHash(const char *message) {
    int hash = 0;
    for(int i = 0; i < 8; i++) {
        hash += message[i] * pow(31, i);
    }
    return hash;
}