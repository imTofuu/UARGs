#include <Arduino.h>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>

#define SOH (char)1
#define STX (char)2
#define ETX (char)3
#define BEL (char)7

#define PUBLIC 0xffff

#define R_OK 0xff
#define R_HASH 0xfe
#define R_NRECV 0xfd

typedef struct Response {
    uint8_t code;
    uint8_t port;
    uint8_t id;  
} rsp;

#define NULLRSP Response {0, 0, 0}

class Device {
public:
    class Message {
    public:
        Message(void* data, uint8_t dataLen, uint8_t target, Device *origin);

        bool sendNext();

        bool dataLeft();
    private:
        uint8_t port;

        uint8_t sent = 0;

        Device *origin;

        char* data;
        uint8_t dataLen;

    };

    Device(uint8_t address) : Device(address, false) {}
    Device(uint8_t address, bool logging);

    uint8_t getAddress() {return address;}

    void update();

    void begin() {begin((HardwareSerial*)&Serial);}
    void begin(HardwareSerial *port) {begin(port, port);}
    void begin(HardwareSerial *send, HardwareSerial *recv);

    HardwareSerial* getSend();
    HardwareSerial* getRecv();

    bool sendMessage(Message message);

    Message createMessage(void *data, uint8_t dataLen, uint16_t port);

    uint16_t openPort(uint8_t with);
protected:

    void sendResponse(const Response &response);
    Response getResponse();
    Response readResponse();
private:
    HardwareSerial *send, *recv;
    uint8_t address;
    bool logging;

    std::vector<Message> sending;
    std::vector<Message> recieving;
    std::vector<Message> recieved;
    std::vector<char> unfinished;

    void log(String message);
    uint32_t generateHash(const char *message);
};