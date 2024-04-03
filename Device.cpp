#include "Device.hpp"

Device::Device(uint8_t address, bool logging) {
    this->address = address; this->logging = logging;
}

void Device::begin(HardwareSerial *send, HardwareSerial *recv) {
    send->begin(57600); recv->begin(57600); this->send = send; this->recv = recv;
    if(logging)
        Serial.begin(57200);
}

void Device::update() {
    for(int i = 0; i < sending.size(); i++) {
        if(sending[i].dataLeft()) {
            //TODO wait for response
            sending[i].sendNext();
        } else {
            //TODO say that its done
            sending.erase(sending.begin() + i);
        }
    }

    while(recv->available()) {
        unfinished.push_back(recv->read());
    }

    while(std::count(unfinished.begin(), unfinished.end(), ETX) > 0) {
        std::vector<char>::iterator findit = find(unfinished.begin(), unfinished.end(), ETX);
        if(findit != unfinished.end()
            && unfinished[(findit - unfinished.begin()) - 8] == STX) {
            uint64_t numerical = unfinished[(findit - unfinished.begin()) - 8];
            
        }
    }

}

//------------------------------------------------------------------------------------
// Normal packets
//------------------------------------------------------------------------------------

Device::Message Device::createMessage(void *data, uint8_t dataLen, uint16_t port) {
    char *dt = reinterpret_cast<char*>(data);
    return Message(dt, dataLen, port, this);
}

bool Device::sendMessage(Message message) {
    if(message.dataLeft()) {
        sending.push_back(message);
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------------
//Normal responses
//------------------------------------------------------------------------------------

void Device::sendResponse(const Response &response) {
    send->print(SOH);

    uint16_t numerical = 0;
    numerical += ((uint16_t)(response.code & 15) << 12);
    numerical += ((uint16_t)(response.port & 63) << 6);
    numerical += ((uint16_t)(response.id & 63));

    send->write(numerical);
    send->println(ETX);   
}

Response Device::getResponse() {
    if(recv->available() && recv->peek() == SOH) {
        Response response = readResponse();
        if(response.port == address + 128) 
            return response;
    }
    return NULLRSP;
}

Response Device::readResponse() {
    recv->read();
    uint16_t numerical = 0;
    for(int i = 0; i < 2; i++) {
        uint16_t currentByte = recv->read();
        if(currentByte == ETX) {
            return NULLRSP;
        }
        numerical += (currentByte >> (i * 8));
    }

    Response response;
    response.code = (numerical & 61440) >> 12;
    response.port = (numerical & 4032) >> 6;
    response.id = (numerical & 63);

    recv->flush();
    return response;
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

uint16_t Device::openPort(uint8_t with) {
    uint16_t port = (address * 256) + with;
    return port;
}

HardwareSerial* Device::getSend() {
    return send;
}

HardwareSerial* Device::getRecv() {
    return recv;
}