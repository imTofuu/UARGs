#include <Arduino.h>

#define SOH (char)1
#define STX (char)2
#define ETX (char)3
#define BEL (char)7

class Device {
public:
    Device(HardwareSerial *send, HardwareSerial *recv);

    struct Packet {
        const char* message;
        uint8_t target;
        uint32_t hash;
    };

    void sendPacket(Packet packet);
    void sendPacket(const char message[8], uint8_t address) {sendPacket(createPacket(message, address));}
    const Packet createPacket(const char message[8], uint8_t target);

    const Packet getPacket();
private:
    HardwareSerial *send, *recv;

    uint32_t generateHash(const char *message);
};