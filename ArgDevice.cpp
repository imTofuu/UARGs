#include "ArgDevice.hpp"
#include <Arduino.h>
#include <string.h>

ArgDevice::ArgDevice(bool logging) {
  ArgDevice::logging = logging;
}

void ArgDevice::begin() {
  ArgDevice::begin(&Serial);
}
void ArgDevice::begin(HardwareSerial *serial) {
  ArgDevice::begin(serial, serial);
}
void ArgDevice::begin(HardwareSerial *send, HardwareSerial *recv) {
  ArgDevice::send = send; ArgDevice::recv = recv;
  if(ArgDevice::logging) {
    Serial.begin(115200);
  }
  send->begin(115200);
  recv->begin(115200);
}

void ArgDevice::logItem(String s) {
  if(ArgDevice::logging) {
    Serial.println(s);
  }
}

SendStatus ArgDevice::sendArgs(String s) {
  SendStatus ret = AD_OK;
  ArgDevice::send->flush();
  ArgDevice::send->println(String(startOfTransmission) += String(' ') += s += String(endOfTransmission));
  int c = 0;
  ArgDevice::recv->setTimeout(10000);
  while(!(ArgDevice::recv->available() && ArgDevice::recv->find(acknowledge))) {
    delay(10);
    c++;
    if(c >= 500) {
      ArgDevice::logItem("No response recieved in 5 seconds.");
      ret = AD_ERR;
      break;
    }
  }
  ArgDevice::send->flush();
  ArgDevice::recv->flush();
  return ret;
}

Args* ArgDevice::receiveArgs() {
  ArgDevice::recv->setTimeout(50);
  if(ArgDevice::recv->available() && ArgDevice::recv->find(endOfTransmission)) {
    ArgDevice::logItem("Found args to recieve");
    ArgDevice::recv->readStringUntil(' ');
    ArgDevice::recv->read();
    Args *args = (Args*)malloc(sizeof(Args)));
    const char* command = ArgDevice::recv->readStringUntil(' ').c_str();
    args->command = *(new String(command));
    while(true) {
      String arg;
      if(ArgDevice::recv->find(' ')) {
        arg = ArgDevice::recv->readStringUntil(' ');
        ArgDevice::addArg(args, arg);
      } else {
        arg = ArgDevice::recv->readStringUntil(endOfTransmission);
        ArgDevice::addArg(args, arg);
        break;
      }
    }
    Serial.println("here");
    ArgDevice::recv->println(acknowledge);
    Serial.println("returning");
    return args;
  }
  return nullptr;
}

void ArgDevice::addArg(Args *args, String string) {
  Arg* temp = (Arg*)malloc(sizeof(Arg) * args->amount + 1);
  Arg* newMem = (Arg*)memmove(temp, args->args, sizeof(Arg) * args->amount);
  free(args->args);
  newMem[args->amount].v = string;

  args->args = newMem;
  args->amount++;
}