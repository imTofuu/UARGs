#include <Arduino.h>

#define SOH (char)1
#define STX (char)2
#define ETX (char)3
#define BEL (char)7

#define PUBLIC 0xff

#define ROK 0xff
#define RHASH 0xfe

struct Packet {
        const char message[6];
        uint8_t target;
        uint8_t origin;
        uint32_t hash;
    };

    struct Response {
        uint8_t code;
        uint8_t target;
    };

class Device {
public:
    Device(uint8_t address) : Device(address, false) {}
    Device(uint8_t address, bool logging);

    static constexpr Packet NULLPKT = {"NLPKT", 0x0, 0x0, 0};
    static constexpr Response NULLRSP = {0x0, 0x0};

    uint8_t getAddress() {return address;}

    const uint64_t getNumericalValue(Packet packet);
    const Packet getPacketValue(uint64_t num);

    void sendResponse(Response response);
    const Response getResponse();
    const Response readResponse();

    void update();

    void begin() {begin((HardwareSerial*)&Serial);}
    void begin(HardwareSerial *port) {begin(port, port);}
    void begin(HardwareSerial *send, HardwareSerial *recv) {
        send->begin(57600); recv->begin(57600); this->send = send; this->recv = recv;
        if(logging)
            Serial.begin(57200);
    }

    void sendPacket(Packet packet);

private:
    HardwareSerial *send, *recv;
    uint8_t address;
    bool logging;

    Packet *pending;
    uint16_t numPending;

    void log(String message);
    uint32_t generateHash(const char *message);
};