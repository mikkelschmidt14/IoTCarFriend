//RFID based on serial.

#include <SoftwareSerial.h>
#include <SpritzCipher.h>

SoftwareSerial SoftSerial(10,11); //rx,tx

byte buffer[63];
int count = 0;
char tag[10];

String hashRFID(byte data[], byte dataLength){
  byte hashLength = 1;
  byte digest[dataLength]; //out buffer.
  byte digest_2[hashLength];

  spritz_ctx hash_ctx;

  unsigned int i;

  spritz_hash_setup(&hash_ctx);
  
  for(i = 0; i < dataLength; i++){
    spritz_hash_update(&hash_ctx, data + i, 1);
  }

  spritz_hash_final(&hash_ctx, digest_2, hashLength);

  spritz_hash(digest, hashLength, data, dataLength);

  spritz_hash(digest, hashLength, data, dataLength);

  String output;
  for(i = 0; i < sizeof(digest); i++){
    output += String(digest[i]);
  }
  return output;
}

void setup() {
  SoftSerial.begin(9600);
  Serial.begin(9600);
  

}

void loop() {
  while(SoftSerial.available() && count <= 63){
    buffer[count] = SoftSerial.read();
    count++;
  }
  String hashed = hashRFID(buffer, count);
  
  Serial.println(hashed);
  count = 0;
  delay(5000);
  //Debugging code to write to serial RFID transmitter.
  if(Serial.available()){
    SoftSerial.write(Serial.read());
  }
}




