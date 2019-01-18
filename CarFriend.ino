/*
   Measure distance and sends it to ThingSpeak via LoRaWAN.
   Connect Ultra Sonic Sensor (HC-SR04) as follows:
        Vcc to 5v
        Trig to 1
        Echo to 0
        Gnd to gnd
    Connect RN2483 LoRa Module as follows:
        Vin to 3.3v
        Tx to 12
        Rx to 13
        gnd to gnd
    Connect Grove - 125KHz RFID Reader as follows:
        Vcc to 5v
        Tx to 10
        Rx to 11
        gnd to gnd
*/

#include <rn2xx3.h>
#include <SoftwareSerial.h>
#include <SpritzCipher.h>

//A unique ID is needed in order for the device to be identified and located.
int uniqueID = 0;

//Ultrasonic sensor definitions. Pins:
const int trigPin = 1;
const int echoPin = 0;
const int range = 15;
long duration;
int distance;
boolean isParked = false;

//LoRa Module definitions:
SoftwareSerial myLoraSerial(12,13);
rn2xx3 myLora(myLoraSerial);
//Keys needed to register with gateway.
const char *devAddr = "A97C20E3";
const char *nwkSKey = "D9C37E0CE353095E59CA0A708779BD7C";
const char *appSKey = "339598331941505DE99A8FB6E71385C5";

//RFID Definitions
SoftwareSerial SoftSerial(10, 11); //rx,tx
byte buffer[63];
int count = 0;
char tag[10];

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(57600); // Starts the serial communication
  SoftSerial.begin(57600);

  delay(100);
  
  //Lora setup.
  myLoraSerial.begin(57600);
  myLora.autobaud();
  Serial.println("HWEUI:");
  Serial.println(myLora.hweui());

  myLora.initABP(devAddr, appSKey, nwkSKey);
  delay(1000);


}


void loop() {
  
  distance = measureDistance();
  Serial.print("Distance: "); Serial.println(distance);

  if (distance < range && !isParked) {
    isParked = true;

    //Reads input from SoftSerial(the RFID tag)
    count = 0;
    while (SoftSerial.available() && count <= 63) {
      buffer[count] = SoftSerial.read();
      count++;
    }
    buffer[count] = '\0';
    
    String hashed = hashRFID(buffer, count);
    Serial.print("HASH is:"); Serial.println(hashed);

    //Transmit hash to lora if valid RFID is read.

    if (hashed.length() > 2) {
      Serial.println("Transmitting hash on LoRa.");
      myLora.txUncnf(hashed);
    } else {
      Serial.println("Sensor blocked without RFID read.");
      myLora.txUncnf("Illegal parking");
    }

  
  } else if (distance > range + 10 && isParked){ //+10 added to avoid hysteresis.
    isParked == false;
  }

  delay(1000);

}

int measureDistance() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  return duration * 0.034 / 2;
}

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
