#include "src/ArgDevice.hpp"
#include <HTTP.h>

ArgDevice device(false);

void setup() {
  device.begin(&Serial2);
  HTTP::begin("Bryans", "ABCDEFGH");

  Serial.begin(115200);

  //device.sendArgs("http arg1 arg2");

  device.addEventListener("http", http);
}
void loop() {
  HTTP::update();
  device.update();
}

void http(Args args) {
  Serial.println("got here");

  String url = device.getArg(&args, 0).v;
  String obj = device.getArg(&args, 1).v;

  JsonDocument doc;
  HTTP::makeRequest(url, &doc);

  String objval = doc[obj];

  Serial.println(objval);
  device.sendArgs("http " + objval);
  
}

/*#include "src/ArgDevice.hpp"

ArgDevice device(false);

void setup() {
  device.begin(&Serial);

  device.sendArgs("http https://api.polygon.io/v2/aggs/ticker/AAPL/range/1/day/2023-01-09/2023-01-09?adjusted=true&sort=asc&limit=120&apiKey=s8vjoJaJMQHSGvOMxgBGnwAhl1TU0FLz ticker");

  device.addEventListener("http", http);
}
void loop() {
  device.update();
}

void http(Args args) {

  Serial.println("http");
  Serial.println(device.getArg(&args, 0).v);
  
}*/