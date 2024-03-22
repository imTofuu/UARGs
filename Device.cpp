#include "Device.hpp"

Device::Device(HardwareSerial *send, HardwareSerial *recv, uint8_t address) {
    this->send = send; this ->recv = recv;
}

//------------------------------------------------------------------------------------
//Anonymous packets
//------------------------------------------------------------------------------------

void Device::anonSendPacket(HardwareSerial *send, HardwareSerial *recv, const char message[8], uint8_t target) {
    Packet packet = {message, target, 0x0, sgenerateHash(message)};
    send->print(STX);
    send->write((char*)&packet, sizeof(Packet));
    send->println(ETX);
    for(int i = 0; i < 500; i++) {
        delay(10);
        Response response = anonGetResponse(send, recv, ANON_RESPONSE);
        if(response.code == 0) continue;
        if(response.code == ROK) {
            break;
        } else {
            anonSendPacket(send, recv, message, target);
            break;
        }
    }
    send->flush();
}

const Device::Packet Device::anonGetPacket(HardwareSerial *send, HardwareSerial *recv, uint8_t address) {
    if(recv->available() && recv->peek() == STX) {

        Packet packet = anonReadPacket(recv);

        if(packet.hash != sgenerateHash(packet.message)) {
            anonSendResponse(send, recv, {RHASH, packet.origin});
            for(int i = 0; i < 500; i++) {
                delay(10);
                const Packet packet2 = anonGetPacket(send, recv, address);
                if(packet2.origin != 0)
                    return packet2;
            }
            return NULLPKT;
        }
        if(packet.target == address) {
            anonSendResponse(send, recv, {ROK, packet.origin});
            return packet;
        }
    }
    recv->flush();
    return NULLPKT;
}

const Device::Packet Device::anonReadPacket(HardwareSerial *recv) {
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

//------------------------------------------------------------------------------------
//Anonymous responses
//------------------------------------------------------------------------------------

void Device::anonSendResponse(HardwareSerial *send, HardwareSerial *recv, Response response) {
    send->print(STX);
    send->write((char*)&response, sizeof(Response));
    send->println(ETX);
}

const Device::Response Device::anonGetResponse(HardwareSerial *send, HardwareSerial *recv, uint8_t address) {
    
}

const Device::Response Device::anonReadResponse(HardwareSerial *recv) {
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
// Normal packets
//------------------------------------------------------------------------------------

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