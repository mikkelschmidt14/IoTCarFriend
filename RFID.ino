//RFID based on serial.

#include <SoftwareSerial.h>

SoftwareSerial SoftSerial(10,11); //rx,tx

char buffer[63];
int count = 0;
char tag[10];

void setup() {
  SoftSerial.begin(9600);
  Serial.begin(9600);
  

}

void loop() {
  while(SoftSerial.available() && count <= 63){
    buffer[count] = SoftSerial.read();
    count++;
  }
  
  Serial.write(buffer,count);
  count = 0;

  //Debugging code to write to serial RFID transmitter.
  if(Serial.available()){
    SoftSerial.write(Serial.read());
  }
}


