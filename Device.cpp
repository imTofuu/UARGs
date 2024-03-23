#include "Device.hpp"

Device::Device(uint8_t address, bool logging) {
    this->address = address; this->logging = logging;
    if(logging)
        Serial.begin(115200);
}

void Device::update() {

}

//------------------------------------------------------------------------------------
// Normal packets
//------------------------------------------------------------------------------------

void Device::sendPacket(Packet packet) {
    log("Sending packet");
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
    if(recv->available())
        Serial.println(recv->peek());
    if(recv->available() && (char)recv->peek() == STX) {

        log("Compiling message");
        Packet packet = readPacket();

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
    recv->read();
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