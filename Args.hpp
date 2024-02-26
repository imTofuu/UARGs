#include <stdint.h>
#include <Arduino.h>

struct Arg {
  String v;
};

class Args {
public:
  Args(String command);

  String getCommand();
  void addArg(String s);
  Arg* operator[](uint16_t i);


private:
  String command;
  Arg* args;
  uint16_t amount;
};