#include <Arduino.h>

#define SOH (char)1
#define STX (char)2
#define ETX (char)3
#define BEL (char)7

class Device {
public:
    Device(HardwareSerial *send, HardwareSerial *recv, uint8_t address);

    struct Packet {
        const char* message;
        uint8_t target;
        uint8_t origin;
        uint32_t hash;
    };

    struct Response {
        uint8_t code;
        uint8_t target;
    };

    static void anonSendPacket(HardwareSerial *send, HardwareSerial *recv, const char message[8], uint8_t target);
    static const Packet anonGetPacket(HardwareSerial *send, HardwareSerial *recv, uint8_t address);
    void sendPacket(Packet packet);
    void sendPacket(const char message[8], uint8_t address) {sendPacket(createPacket(message, address));}
    const Packet createPacket(const char message[8], uint8_t target);

    static void anonSendResponse(HardwareSerial *send, HardwareSerial *recv, Response response);
    static const Response anonGetResponse(HardwareSerial *send, HardwareSerial *recv, uint8_t address);

    const Packet getPacket();
private:
    HardwareSerial *send, *recv;
    uint8_t address;

    uint32_t generateHash(const char *message);
    static uint32_t sgenerateHash(const char *message);
};