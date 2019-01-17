/*
   Measure distance and sends it to ThingSpeak via ESP8266.
   Connect Ultra Sonic Sensor (HC-SR04) as follows:
        Vcc to 5v
        Trig to D1
        Echo to D0
        Gnd to gnd
*/


#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <SoftwareSerial.h>
#include <SpritzCipher.h>

// defines pins numbers
const int trigPin = D1;
const int echoPin = D0;

// defines variables
const char* ssid = "OnePlus 6T"; // Insert SSID for the accesspoint
const char* password = "12345679"; // Insert Wifi key/password
WiFiClient client;
unsigned long channelID = 673886;
const char * myWriteAPIKey = "QT7PQPVJDV6RQQKK";
const char * server = "api.thingspeak.com";
const int postingInterval = 20 * 1000;

long duration;
int distance;
Boolean isParked = false;

SoftwareSerial SoftSerial(D3, D4); //rx,tx
byte buffer[63];
int count = 0;
char tag[10];

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
  SoftSerial.begin(9600);

  delay(100);

  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.println(WiFi.localIP());

}


void loop() {

  distance = measureDistance();
  Serial.print("Distance: "); Serial.println(distance);

  if (distance < 15 && !isParked) {
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

    //Post distance to ThingSpeak
    ThingSpeak.begin(client);
    client.connect(server, 80);

    if (hashed.length() > 2) {
      ThingSpeak.setField(4, hashed);
    } else {
      ThingSpeak.setField(4, "Illigal parking");
    }

    ThingSpeak.writeFields(channelID, myWriteAPIKey);
    client.stop();

  } else if (distance > range && isParked){
    isParked == false;
  }

  delay(postingInterval);

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
