#include "Device.hpp"

Device::Message::Message(void* data, uint8_t dataLen, const uint8_t port, Device *origin) {
    this->data = reinterpret_cast<char*>(data);
    this->dataLen = dataLen;
    this->origin = origin;
    this->port = port;
}

bool Device::Message::dataLeft() {
    return sent < dataLen;
}

bool Device::Message::sendNext() {
    HardwareSerial *send = origin->getSend();
    uint64_t numerical = 0;

    if(sent + 6 <= dataLen) {
        for(int i = 0; i < 6; i++) {
            numerical += (((uint64_t)data[sent]) << (56 - (i * 8)));
            sent++;
        }
    } else {
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