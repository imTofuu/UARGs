#include "Device.hpp"

Device::Message::Message(char *data, uint8_t dataLen, const uint8_t port, Device *origin) {
    this->data = new char[dataLen];
    memcpy(this->data, data, dataLen);
    this->dataLen = dataLen;
    this->origin = origin;
    this->port = port;
}

Device::Message::~Message() {
    delete [] this->data;
}

bool Device::Message::dataLeft() {
    return sent < dataLen;
}

bool Device::Message::sendNext() {
    HardwareSerial *send = origin->getSend();
    uint64_t numerical = 0;

    Serial.println(this->data);
    Serial.println(data);

    Serial.println(data[0]);

    origin->log("Sent packet:");
    origin->log("   Data: ");
    if(sent + 6 <= dataLen) {
        for(int i = 0; i < 6; i++) {
            if(origin->logging)
                Serial.print(data[sent]);
            numerical += (((uint64_t)data[sent]) << (56 - (i * 8)));
            sent++;
        }
    } else {
        if(origin->logging)
                Serial.print(data[sent]);
        for(int i = 0; i < dataLen - sent; i++) {
            numerical += (((uint64_t)data[sent]) << (56 - (i * 8)));
            sent++;
        }
    }
    numerical += port;


    uint32_t *split = (uint32_t*)&numerical;

    send->flush();
    send->write(STX);
    send->flush();
    send->write(split[1]);
    send->write(split[0]);
    send->flush();
    send->write(ETX);

    return true;
}