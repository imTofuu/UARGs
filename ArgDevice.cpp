#include "ArgDevice.hpp"
#include <Arduino.h>

void ArgDevice::begin() {
  ArgDevice::begin(&Serial);
}

void ArgDevice::begin(HardwareSerial *serial) {
  ArgDevice::begin(serial, serial);
}

void ArgDevice::begin(HardwareSerial *send, HardwareSerial *recv) {
  ArgDevice::send = send; ArgDevice::recv = recv;
  send->begin(115200);
  recv->begin(115200);
  ArgDevice::status = D_BEGUN;
}

SendStatus ArgDevice::sendArgs(String s) {
  ArgDevice::send->flush();
  ArgDevice::send->print('STX');
  ArgDevice::send->print(s);
  ArgDevice::send->println('EOT');
  while(!(ArgDevice::recv->available() && ArgDevice::recv->find('ACK'))) {
    delay(10);
  }
  ArgDevice::send->flush();
  ArgDevice::recv->flush();
  return AD_OK;
}


Args* ArgDevice::receiveArgs() {
  Serial.println(ArgDevice::recv->peek());
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
    ArgDevice::send->write('ACK');
    return args;
  }
  return nullptr;
}