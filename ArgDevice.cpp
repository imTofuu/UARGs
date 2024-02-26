#include "ArgDevice.hpp"
#include <Arduino.h>

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
  send->setTimeout(10000);
  recv->setTimeout(10000);
  ArgDevice::status = D_BEGUN;
}

SendStatus ArgDevice::sendArgs(String s) {
  ArgDevice::send->flush();
  ArgDevice::send->println(String('STX') += s += String('EOT'));
  int c = 0;
  while(!(ArgDevice::recv->available() && ArgDevice::recv->find('ACK'))) {
    delay(10);
    c++;
    if(c >= 200) {
      if(ArgDevice::logging) {
        Serial.println("No response recieved in 2 seconds.");
      }
      break;
    }
  }
  ArgDevice::send->flush();
  ArgDevice::recv->flush();
  return AD_OK;
}


Args* ArgDevice::receiveArgs() {
  if(ArgDevice::recv->available() && ArgDevice::recv->find('STX')) {
    Serial.println(1);
    ArgDevice::recv->read();
    Args *args = new Args(ArgDevice::recv->readStringUntil(' '));
    while(true) {
      String arg;
      if(ArgDevice::recv->find(' ')) {
        arg = ArgDevice::recv->readStringUntil(' ');
        args->addArg(arg);
      } else {
        arg = ArgDevice::recv->readStringUntil('EOT');
        args->addArg(arg);
        break;
      }
    }
    ArgDevice::recv->flush();
    ArgDevice::send->println('ACK');
    return args;
  }
  return nullptr;
}