#include "ArgDevice.hpp"

ArgDevice device(true);

void setup() {

  device.begin();

  //delay(1000);
  //device.sendArgs("command arg arg2");
}
void loop() {

  Args args = device.receiveArgs();
  if(args.amount > 0) {
    Serial.println("notnull");
    Serial.println(args.command);
    Serial.println(device.getArg(&args, 1).v);
  }
}