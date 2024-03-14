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

void ArgDevice::update() {
  commandListener.dispatchEvents(receiveArgs());
}

inline void ArgDevice::logItem(String s) {
  if(logging) {
    Serial.println(log + s);
  }
}

SendStatus ArgDevice::sendArgs(String s) {
  SendStatus ret = AD_OK;
  logItem("Resetting buffer...");
  send->flush();
  logItem("Sending message...");
  send->println(String(startOfTransmission) += s += String(endOfTransmission));
  int c = 0;
  recv->setTimeout(10);
  logItem("Waiting for acknowledgement response...");
  while(!(recv->available() && recv->find(acknowledge))) {
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
      logItem("Waiting for whole message...");
      String newBuffer = getBuffer(buffer);
      buffer = newBuffer;
      delay(10);
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
  delete [] args->args;

  logItem("Putting memory back...");
  args->args = newMemory;
  args->amount++;
}

void ArgDevice::addEventListener(String command, void (*callback)(Args)) {
  CommandListener::RegisteredEvent *newMemory = new CommandListener::RegisteredEvent[commandListener.numRegisteredEvents + 1];
  memcpy(newMemory, commandListener.registeredEvents, sizeof(CommandListener::RegisteredEvent) * commandListener.numRegisteredEvents);
  newMemory[commandListener.numRegisteredEvents].command = command;
  newMemory[commandListener.numRegisteredEvents].callback = callback;
  delete [] commandListener.registeredEvents;

  commandListener.registeredEvents = newMemory;
  commandListener.numRegisteredEvents++;

  for(int i = 0; i < commandListener.numRegisteredEvents; i++) {
    Serial.println(commandListener.registeredEvents[i].command);
  }
}

void ArgDevice::removeEventListener(String command, void(*callback)(Args)) {
  for(int i = 0; i < commandListener.numRegisteredEvents; i++) {
    if(commandListener.registeredEvents[i].command == command &&
      commandListener.registeredEvents[i].callback == callback) {
        logItem("found arg to delete");

        logItem("Making new array");
        CommandListener::RegisteredEvent *newMemory = new CommandListener::RegisteredEvent[commandListener.numRegisteredEvents - 1];

        int index = 0;
        for(int x = 0; x < commandListener.numRegisteredEvents; x++) {
          if(x == i) {
            continue;
            if(x >= commandListener.numRegisteredEvents) break;
          }
          logItem("Copying item: ");
          logItem(String(x));
          Serial.println(commandListener.registeredEvents[x].command);
          newMemory[index] = commandListener.registeredEvents[x];
          index++;
        }

        logItem("Deleting old array");
        delete [] commandListener.registeredEvents;

        logItem("Assigning variable to new memory");
        commandListener.registeredEvents = newMemory;
        commandListener.numRegisteredEvents--;
      }
  }
  for(int i = 0; i < commandListener.numRegisteredEvents; i++) {
    Serial.println(commandListener.registeredEvents[i].command);
  }
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

void ArgDevice::CommandListener::dispatchEvents(Args args) {
  for(int i = 0; i < numRegisteredEvents; i++) {
      if(args.command == registeredEvents[i].command) {
        registeredEvents[i].callback(args);
      }
  }
}