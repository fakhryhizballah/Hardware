#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial linkflow(5, 6); // (Rx, Tx)

void setup()
{
  Serial, begin(9600);
  linkflow.begin(9600);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
