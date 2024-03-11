#include "ArgDevice.hpp"
#include <Arduino.h>
#include <string.h>

// TODO:
/*
* Checksum
* Check for buffer overflow
*/

ArgDevice::ArgDevice(bool logging) {
  this->logging = logging;
}

void ArgDevice::begin() {
  begin((HardwareSerial*)&Serial);
}
void ArgDevice::begin(HardwareSerial *serial) {
  begin(serial, serial);
}
void ArgDevice::begin(HardwareSerial *send, HardwareSerial *recv) {
  this->send = send; this->recv = recv;

  if(logging) {
    Serial.begin(115200);
  }
  send->begin(115200);
  recv->begin(115200);
}

inline void ArgDevice::logItem(String s) {
  if(logging) {
    Serial.println(s);
  }
}

SendStatus ArgDevice::sendArgs(String s) {
  SendStatus ret = AD_OK;
  send->flush();
  send->println(String(startOfTransmission) += s += String(endOfTransmission));
  int c = 0;
  recv->setTimeout(10000);
  while(!(recv->available() && recv->find(acknowledge))) {
    delay(10);
    c++;
    if(c >= 500) {
      logItem("No response recieved in 5 seconds.");
      ret = AD_ERR;
      break;
    }
  }
  send->flush();
  recv->flush();
  return ret;
}

inline String ArgDevice::getBuffer(String buffer) {
  if(recv->available()) {
    String newBuffer = buffer += recv->readString();
    return newBuffer;
  }
  return buffer;
}

String buffer = "";
Args ArgDevice::receiveArgs() {
  if((recv->available() && recv->peek() == startOfTransmission)) {

    String trueBuffer = "";
    while(buffer.charAt(buffer.length() - 3) != endOfTransmission) {
      String newBuffer = getBuffer(buffer);
      buffer = newBuffer;
    }

    logItem("Message recieved.");
    Args args;
    args.amount = 0;

    logItem("Getting command...");
    int lastEnd = findFrom(' ', 0, buffer);
    String command = readChunk(buffer, 1, lastEnd);
    logItem("Saving command...");
    args.command = command;


    logItem("Getting arguments.");
    while(true) {
      int currentEnd = findFrom(' ', lastEnd + 1, buffer);
      if(currentEnd > 0) {
        logItem("Found argument.");
        logItem("Reading argument...");
        String arg = readChunk(buffer, lastEnd + 1, currentEnd);
        logItem("Adding argument...");
        addArg(&args, arg);
        logItem("Added argument.");
      } else {
        logItem("Found last argument.");
        logItem("Reading argument...");
        String arg = readChunk(buffer, lastEnd + 1, buffer.length() - 3);
        logItem("Adding argument...");
        addArg(&args, arg);
        logItem("Added argument");
        break;
      }
      lastEnd = currentEnd;
    }
    recv->println(acknowledge);
    buffer = "";
    return args;
  }
  return NULLARGS;
}

void ArgDevice::addArg(Args *args, String string) {
  logItem("Creating new array...");
  Arg* newMemory = new Arg[args->amount + 1];
  logItem("Copying old memory...");
  memcpy(newMemory, args->args, sizeof(Arg) * args->amount);
  newMemory[args->amount].v = string;
  logItem("Freeing old memory...");
  free(args->args);

  args->args = newMemory;
  args->amount++;
}

Arg ArgDevice::getArg(Args *args, int index) {
  if(index >= 0 && index < args->amount) {
    return args->args[index];
  }
  return NULLARGS.args[0];
}

int ArgDevice::findFrom(char character, int start, String string) {
  for(int i = start; i < string.length(); i++) {
    if(string[i] == character) return i;
  }
  return -1;
}

String ArgDevice::readChunk(String string, int start, int end) {
  return string.substring(start, end);
}