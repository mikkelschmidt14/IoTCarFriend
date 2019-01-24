IOT LoRa app designed for automated parking systems. Uses an arduino connected with a ultrasonic range sensor, a LoRa module, and an RFID reader. It transmits a hash of the read RFID tag if available, and sends it via the LoRa module, where it ends up on Thingspeak for further analysis.

An alternative method communicating directly with thingspeak over WiFi by using the built-in shield included with ESP8226 is also included.

Also included is an app communicating with the setup on thingsSpeak.
