#include <WiFi.h>
#include <WiFiUdp.h>
const char* ssid = "TP-Link_E0C8";
const char* password = "52665134";

WiFiUDP UDPTestServer;
const IPAddress myIP(192,168,1,124);//MINE
IPAddress TargetIP(192, 168, 1, 214); //DAUDI

const int potPin = 4;     // GPIO pin connected to the potentiometer
const int ledPin =  1;  // GPIO pin connected to the LED
const int ledcChannel = 1;  // LEDC channel for the LED
const int freq = 30;       // Frequency in Hz
const int ledcResolution = 11; // 11-bit resolution

void setup() {
Serial.begin(115200);
analogReadResolution(12); // 12-bit ADC resolution ****

ledcSetup(ledcChannel, freq, ledcResolution);   // Configure LEDC
ledcAttachPin(ledPin, ledcChannel);

  // UDP RECEIVER ->>>
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.config(myIP, IPAddress(192, 168, 1, 1),IPAddress(255, 255, 255, 0));
WiFi.begin(ssid, password);
UDPTestServer.begin(3111); 

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("WiFi connected");

}  
const int UDP_PACKET_SIZE = 100; 
 byte packetBuffer[UDP_PACKET_SIZE]; // can be up to 65535
// <<<- UDP RECEIVER

// UDP SENDER ->>>
void fncUdpSend(int i) // send 2 byte int i
{
byte udpBuffer[2];
udpBuffer[0] = i & 0xff; // send 1st (LSB) byte of i
udpBuffer[1] = i>>8; // send 2nd (MSB) byte of i
UDPTestServer.beginPacket(TargetIP, 3111);
UDPTestServer.write(udpBuffer, 2); // send 2 bytes in udpBuffer
UDPTestServer.endPacket();
}// <<<-UDP SENDER

// UDP RECEIVER ->>>
void handleUDPServer() { // receive and print int i
int i, cb = UDPTestServer.parsePacket();
if (cb) {
UDPTestServer.read(packetBuffer, cb);
i = (packetBuffer[0] + (packetBuffer[1]<<8)); // puts 2 bytes into int
Serial.println(i); // prints the number (note no need to convert to asii)

ledcWrite(ledcChannel, i);
}
}// <<<- UDP RECEIVER

void loop() {
 // LED ->>>
  int potValue = analogRead(potPin); // Read potentiometer
  int dutyCycle = map(potValue, 0, 4095, 0, 2048); // Map potValue to LEDC duty cycle range
  // <<<-LED
  handleUDPServer();
  delay(10);    
  
  fncUdpSend(dutyCycle); // // Sending dutyCycle over UDP

  delay(10); 
}