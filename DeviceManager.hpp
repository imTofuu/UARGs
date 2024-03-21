#include "Device.hpp"
#include <Arduino.h>

class DeviceManager {
public:
    static Device registerDevice() {registerDevice((HardwareSerial*)&Serial);}
    static Device registerDevice(HardwareSerial *port) {registerDevice(port, port);}
    static Device registerDevice(HardwareSerial *send, HardwareSerial *recv);
private:

    DeviceManager();
};