#include "ArgDevice.hpp"

ArgDevice device(false);

void setup() {

  device.begin();

  //delay(1000);
  //device.sendArgs("i lovemygirlfriend");
}
void loop() {
  Args args = device.receiveArgs();
  if(args.amount > 0) {
    Serial.println(args.command);
    Serial.println(device.getArg(&args, 0).v);
  }
}