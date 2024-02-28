#include <Arduino.h>
#define startOfTransmission (char)2
#define endOfTransmission (char)4
#define acknowledge (char)6

enum DeviceStatus {
  D_IDLE,
  D_BEGUN,
  D_INUSE
};

enum SendStatus {
  AD_OK = 1,
  AD_ERR = 0
};

struct Arg {
  String v;
};

struct Args {
  String command;
  Arg* args;
  uint16_t amount;
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

  void addArg(Args *args, String string);

  void logItem(String s);


  

private:
  HardwareSerial *send, *recv;
  bool logging;
};