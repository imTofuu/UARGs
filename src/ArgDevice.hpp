#include <Arduino.h>
#define startOfTransmission (char)2
#define endOfTransmission (char)4
#define acknowledge (char)6
#define log (char)7

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

  //Returned when there are no args to be gotten.
  const Args NULLARGS = {
    "NULLCOMMAND",
    &nullarghelper,
    0
  };

  //Uses Serial1 for sending and receiving
  void begin();
  //Uses &serial for sending and receiving
  void begin(HardwareSerial *serial);
  //Uses &send for sending and &recv for receiving
  void begin(HardwareSerial *send, HardwareSerial *recv);

  /*Sends an argument to all devices connected to the UART line. Message 
  should be formatted like: "command arg1 arg2" (with spaces seperating)*/
  SendStatus sendArgs(String s);
  //Checks and gets if there any pending messages.
  Args receiveArgs();

  Arg getArg(Args *args, int index);

  //No events will be called unless this is called
  void update();

  void addEventListener(String command, void (*callback)(Args));
  void removeEventListener(String command, void (*callback)(Args));
  
protected:

  class CommandListener {
  public:
    void dispatchEvents(Args args);

    struct RegisteredEvent {
      String command;
      void (*callback)(Args);
    };

    RegisteredEvent *registeredEvents;
    uint16_t numRegisteredEvents;
  };

private:

  HardwareSerial *send, *recv;
  bool logging;
  Arg nullarghelper = {"NULLARG"};
  CommandListener commandListener;

  int findFrom(char character, int start, String string);
  String readChunk(String string, int start, int end);
  String getBuffer(String buffer);
  void addArg(Args *args, String string);
  void logItem(String s);
};