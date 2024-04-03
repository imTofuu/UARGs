#include "Device.hpp"

/**
* \brief Devices are used to send and recieve messages.
* \param address The address that the device will be registered with. The 
* device will listen for any messages sent with the address as the target. As 
* of v2.0.0, multiple devices should not be registered with the same 
* address.
*/
Device::Device(uint8_t address) : Device(address, false) {}

/**
* \brief Devices are used to send and recieve messages.
* \param address The address that the device will be registered with. The 
* device will listen for any messages sent with the address as the target. As 
* of v2.0.0, multiple devicesshould not be registered with the same 
* address.
* \param logging Enables robust logging for debugging purposes.
*/
Device::Device(uint8_t address, bool logging) {
    this->address = address; this->logging = logging;
}

/**
* \brief Begins given send and recieve serial ports, default serial port (if 
* logging is enabled) and sets local variables.
* \param send The serial port that will be sending data on. Ensure any other 
* Arduinos are connected to the TXx (where x is the port) pin otherwise no data 
* will be recieved by any other devices.
* \param recv The serial port that will be recieving data on. Ensure any other 
* Arduinos are connected to the RXx (where x is the port) pin otherwise no data 
* will recieved.
*/
void Device::begin(HardwareSerial *send, HardwareSerial *recv) {
    send->begin(57600); recv->begin(57600); 
    this->send = send; this->recv = recv;
    if(logging)
        Serial.begin(57200);
}

/**
* \brief Sends and recieves data. 1 new packet (containing 6 bytes of data) 
* for every message to be sent for every call of update will be sent if the
* message has recieved a response from the target. Every packet recieved will
* send a response to the origin and add the data to a buffer, either
* Device::unfinished, if the message has not yet been fully sent,
* or Device::recieved, if the message has been fully sent.
*/
void Device::update() {
    for(int i = 0; i < sending.size(); i++) {
        if(sending[i].dataLeft()) {
            log("Sending packet");
            //TODO wait for response
            sending[i].sendNext();
        } else {
            log("Whole message sent, removing from list.");
            //TODO say that its done
            sending.erase(sending.begin() + i);
        }
    }

    /*
    while(recv->available()) {
        unfinished.push_back(recv->read());
    }

    while(std::count(unfinished.begin(), unfinished.end(), ETX) > 0) {
        std::vector<char>::iterator findit = find(unfinished.begin(), unfinished.end(), ETX);
        if(findit != unfinished.end()
            && unfinished[(findit - unfinished.begin()) - 8] == STX) {
            uint64_t numerical = unfinished[(findit - unfinished.begin()) - 8];
            
        }
    }*/

}

//------------------------------------------------------------------------------
// Normal packets
//------------------------------------------------------------------------------
/**
* \brief Convenience function to make a message. Used for sending.
* \param data Data to be sent. Data may be implicitly casted to void*.
* \param dataLen Length in bytes for the data.
* \param to The address of the device for the message to be sent.
* \returns A message object containing the given values.
*/
Device::Message Device::createMessage(void *data, 
        uint8_t dataLen, uint8_t to) {
    return Message((char*)data, dataLen, openPort(to), this);
}

/**
* \brief Adds message object to list of messages to be sent. On every
* Device::update() call, 6 new bytes of data will be sent until the whole
* message is sent.
* \param message The message object to be added to the list. Use
* Device::createMessage(void *, uint8_t, uint8_t) to get the message object.
* \returns The success of the message being added to the list. As of v2.0.0,
* only a message with no data will result in failure.
*/
bool Device::sendMessage(Message message) {
    if(message.dataLeft()) {
        log("Added message to be sent.");
        sending.push_back(message);
        return true;
    }
    log("Message has no data");
    return false;
}
//------------------------------------------------------------------------------
//Normal responses
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
//Other stuff
//------------------------------------------------------------------------------

void Device::log(String message) {
    if(logging)
        Serial.println(message);
}

void Device::lognnl(String message) {
    if(logging)
        Serial.print(message);
}

void Device::logi(int val) {
    if(logging)
        Serial.print(val);
}

uint32_t Device::generateHash(const char *message) {
    int hash = 0;
    for(int i = 0; i < 8; i++) {
        hash += message[i] * pow(31, i);
    }
    return hash;
}

uint16_t Device::openPort(uint8_t with) {
    return ((uint16_t)address * 256) + with;
}

HardwareSerial* Device::getSend() {
    return send;
}

HardwareSerial* Device::getRecv() {
    return recv;
}