#include "Device.hpp"

Device::Device(uint8_t address, bool logging) {
    this->address = address; this->logging = logging;
}

void Device::update() {

}

//------------------------------------------------------------------------------------
// Normal packets
//------------------------------------------------------------------------------------

void Device::sendPacket(Packet packet) {
    send->flush();
    send->print(STX);
    send->flush();
    uint64_t numValue = getNumericalValue(packet);
    uint32_t *vals = (uint32_t*)&numValue;
    Serial.println(vals[0], BIN);
    Serial.println(vals[1], BIN);
    send->write(vals[0]);
    send->write(vals[1]);
    send->flush();
    send->print(ETX);
    send->flush();
}

/*
void Device::sendPacket(Packet packet) {
    send->flush();
    //Serial.println(send->availableForWrite());
    /*log("Sending packet");
    send->print(STX);
    send->write((char*)&packet, sizeof(Packet));
    send->println(ETX);
    log("Waiting for response");
    for(int i = 0; i < 500; i++) {
        delay(10);
        Response response = getResponse();
        if(i >= 499)
            log("Never recieved response");
        if(response.code == 0) continue;
        if(response.code == ROK) {
            log("Response ok");
            break;
        } else {
            log("Invalid response, trying again");
            sendPacket(createPacket(packet.message, packet.target));
            break;
        }
    }
    send->flush();
}

const Device::Packet Device::getPacket() {
    if(recv->available() && (char)recv->peek() == STX) {

        log("Compiling message");
        Packet packet = readPacket();
        Serial.println(*((unsigned int*)&packet));

        if(packet.target == address) {
            if(packet.hash != generateHash(packet.message)) {
                log("Invalid message, asking for another one");
                sendResponse({RHASH, packet.origin});
                for(int i = 0; i < 500; i++) {
                    delay(10);
                    const Packet packet2 = getPacket();
                    if(packet2.origin != 0)
                        log("Got valid packet");
                        return packet2;
                }
                log("Never recieved replacement packet");
                return NULLPKT;
            }
            log("Valid packet recieved");
            uint8_t responseAddress = packet.origin + 128;
            sendResponse({ROK, responseAddress});
            return packet;
        }
        log("Packet not sent to this address");
        return NULLPKT;
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
            recv->read();
            return NULLPKT;
        }
        buffer[i] = currentByte;
    }
    recv->readString();
    return *((Packet*)buffer);
}

const Device::Packet Device::createPacket(const char message[8], uint8_t target) {
    return Packet {message, target, address, generateHash(message)};
}
*/
const uint64_t Device::getNumericalValue(Packet packet) {
    uint64_t num = 0;

    for(int i = 0; i < 6; i++) {
        uint64_t shiftedNum = (uint64_t)(packet.message[i]) << (56 - (i * 8));
        num += shiftedNum;
    }
    uint64_t shiftedTarget = (uint64_t)((packet.target & 127)) << 9;
    num += shiftedTarget;
    uint64_t shiftedOrigin = (uint64_t)((packet.origin & 127)) << 2;
    num += shiftedOrigin;

    return num;
}

const Packet Device::getPacketValue(uint64_t num) {
    return Packet {{
        (char)((num & 18374686479671623680) >> 56),
        (char)((num & 71776119061217280) >> 48),
        (char)((num & 280375465082880) >> 40),
        (char)((num & 1095216660480) >> 32),
        (char)((num & 4278190080) >> 24),
        (char)((num & 16711680) >> 16)
    }, (uint8_t)((num & 65024) >> 9), (uint8_t)((num & 508) >> 2), 0};
}

//------------------------------------------------------------------------------------
//Normal responses
//------------------------------------------------------------------------------------

void Device::sendResponse(Response response) {
    send->print(SOH);
    send->write((char*)&response, sizeof(Response));
    send->println(ETX);   
}

const Response Device::getResponse() {
    if(recv->available() && recv->peek() == SOH) {
        Response response = readResponse();
        if(response.target == address + 128) 
            return response;
    }
    return NULLRSP;
}

const Response Device::readResponse() {
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

void Device::log(String message) {
    if(logging)
        Serial.println(message);
}

uint32_t Device::generateHash(const char *message) {
    int hash = 0;
    for(int i = 0; i < 8; i++) {
        hash += message[i] * pow(31, i);
    }
    return hash;
}