#include "Args.hpp"
#include <string.h>

Args::Args(String command) {
  Args::command = command;
}

Arg* Args::operator[](uint16_t i) {
  return i < Args::amount ? &Args::args[i] : nullptr;
}

void Args::addArg(String s) {
  Args::amount++;
  Arg* temp = (Arg*)malloc(sizeof(Arg) * amount);
  memcpy(temp, Args::args, sizeof(Arg) * (amount - 1));
  free(Args::args);
  temp[amount - 1] = *(Arg*)malloc(sizeof(Arg));
  temp[amount - 1].v = s;
  Args::args = temp;
}

String Args::getCommand() {
  return Args::command;
}