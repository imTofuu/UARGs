#include <Arduino.h>
#include "Args.hpp"

#define startOfTransmission 'STX'
#define endOfTransmission 'EOT'
#define acknowledge 'ACK'

enum DeviceStatus {
  D_IDLE,
  D_BEGUN,
  D_INUSE
};

enum SendStatus {
  AD_OK = 1,
  AD_ERR = 0
};

class ArgDevice {
public:
  //Does not log errors by default.
  ArgDevice() : ArgDevice(false) {}
  //Set whether logs are sent through the computer port to be seen in the serial monitor.
  ArgDevice(bool logging);

  //Uses Serial1 for sending and receiving
  void begin();
  //Uses &serial for sending and receiving
  void begin(HardwareSerial *serial);
  //Uses &send for sending and &recv for receiving
  void begin(HardwareSerial *send, HardwareSerial *recv);

  SendStatus sendArgs(String s);
  Args* receiveArgs();
  

private:
  HardwareSerial *send, *recv;
  bool status = D_IDLE, logging;
};