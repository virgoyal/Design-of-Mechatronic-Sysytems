#include "Adafruit_VL53L0X.h"
#include <Wire.h>
#include "vive510.h"
#include <WiFi.h>
#include <WiFiUdp.h>

#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31

// set the pins to shutdown
#define SHT_LOX1 27
#define SHT_LOX2 19

// intialize motor pins
const int MOTOR_RIGHT_PWM = 18;  // Change this to the right motor PWM pin
const int MOTOR_RIGHT_DIR1 = 17; // Change this to the right motor direction pin 1
const int MOTOR_RIGHT_DIR2 = 16; // Change this to the right motor direction pin 2

const int MOTOR_LEFT_PWM = 33;   // Change this to the left motor PWM pin
const int MOTOR_LEFT_DIR1 = 26;  // Change this to the left motor direction pin 1
const int MOTOR_LEFT_DIR2 = 25;  // Change this to the left motor direction pin 2

#define carSpeed 150

#define BEACON_PIN 36 // Pin to which the signal is connected
#define taskwall 13
#define taskbeacon 9
#define taskvive 10

#define SIGNALPIN1 13 // pin receiving signal from Vive circuit
#define UDPPORT 2510 // For GTA 2022C game
#define teamNumber 34
#define FREQ 1 // in Hz
const char* ssid = "TP-Link_E0C8";
const char* password = "52665134";

Vive510 vive1(SIGNALPIN1);
WiFiUDP UDPServer;
IPAddress ipTarget(192, 168, 1, 255); // 255 => broadcast
IPAddress myIP(192, 168, 1, 214); // change to your IP

int map_x_l = 2000;
int map_x_u = 6600;
int map_y_l = 3000;
int map_y_u = 5000;

int police_x = 0;
int police_y = 0;

int vive_x = 0;
int vive_y = 0;
long vive_ms = millis();

bool wall;
bool beacon;
bool vive;

// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;

uint16_t distfront = 0;
uint16_t distleft = 0;
uint16_t BeaconFreq = 0;

void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // activating LOX1 and resetting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);

  // initing LOX1
  if(!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while(1);
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initing LOX2
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while(1);
  }
}

void handleUDPServer() {
  const int UDP_PACKET_SIZE = 14; // can be up to 65535  
  uint8_t packetBuffer[UDP_PACKET_SIZE];

  int cb = UDPServer.parsePacket(); // if there is no message cb=0
  if (cb) {
  packetBuffer[13]=0; // null terminate string

  UDPServer.read(packetBuffer, UDP_PACKET_SIZE);
  if (atoi((char *)packetBuffer) == 0) { // If team = 0
  police_x = atoi((char *)packetBuffer+3); // ##,####,#### 2nd indexed char
  police_y = atoi((char *)packetBuffer+8); // ##,####,#### 7th indexed char
  Serial.print("From Team ");
  Serial.println((char *)packetBuffer);
  Serial.println("Police Car data: ");
  Serial.println(police_x);
  Serial.println(police_y);
  }
  }
}

void UdpSender(int x, int y)
{
  char udpBuffer[20];
  sprintf(udpBuffer, "%02d:%4d,%4d",teamNumber,x,y);
  UDPServer.beginPacket(ipTarget, UDPPORT);
  UDPServer.println(udpBuffer);
  UDPServer.endPacket();
  Serial.println(udpBuffer);
}

void ViveSender() {
  if (millis()-vive_ms>1000/FREQ) {
  vive_ms = millis();
  if (WiFi.status()==WL_CONNECTED)
  UdpSender(vive_x, vive_y);
  }
  if (vive1.status() == VIVE_RECEIVING) {
  vive_x = vive1.xCoord();
  vive_y = vive1.yCoord();
  }
  else {
  vive_x=0;
  vive_y=0;
  switch (vive1.sync(5)) {
  break;
  case VIVE_SYNC_ONLY: // missing sweep pulses (signal weak)
  break;
  default:
  case VIVE_NO_SIGNAL: // nothing detected
  break;
  }
  }
  Serial.println("vive_x");
  Serial.println(vive_x);
  Serial.println(vive_y);
  Serial.println("vive_y");
  delay(20);
}

int cal_normDis() {
  handleUDPServer(); // Getting Police Car vive x y
  ViveSender(); // Getting our robots vive x y
  // Update police xy
  int distance = pow((pow((police_x - vive_x), 2) + pow((police_y - vive_y), 2)), 0.5); // Norm distance
  int dist_x = pow(pow((police_x - vive_x), 2), 0.5); // X distance
  int dist_y = pow(pow((police_y - vive_y), 2), 0.5); // Y distance
  UdpSender(dist_x, dist_y);
  Serial.print("Car Vive Position: ");
  Serial.println(vive_x);
  Serial.println(vive_y);
  Serial.print("Cal Norm Dis: ");
  Serial.println(distance);
  return distance;
  }

void rotateRight() {
  // Rotate the car to the right
  Serial.println("ENTERED ROTATE RIGHT FUNCTION");
  analogWrite(MOTOR_LEFT_PWM, 250);
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);

  analogWrite(MOTOR_RIGHT_PWM, 230);
  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
}

void rotateLeft() {
  // Rotate the car to the right
  Serial.println("ENTERED ROTATE LEFT FUNCTION");
  analogWrite(MOTOR_LEFT_PWM, 250);
  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, HIGH);

  analogWrite(MOTOR_RIGHT_PWM, 230);
  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
}


void moveForward() {
  // Move the car forward
  Serial.println("ENTERED MOVE FORWARD FUNCTION");
  analogWrite(MOTOR_LEFT_PWM, 240);
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);

  analogWrite(MOTOR_RIGHT_PWM, 240);
  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);

}

void veerRight(){
  Serial.println("ENTERED VEER RIGHT FUNCTION");
  analogWrite(MOTOR_LEFT_PWM, 240);
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);

  analogWrite(MOTOR_RIGHT_PWM, 190);
  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);

}

void veerLeft(){
  Serial.println("ENTERED VEER LEFT FUNCTION");
  analogWrite(MOTOR_LEFT_PWM, 190);
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);

  analogWrite(MOTOR_RIGHT_PWM, 240);
  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
}

void stopCar() {
  // Stop the car
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
}

uint16_t detectFrequency() {
  unsigned long OnTime = pulseIn(BEACON_PIN, HIGH); // Measure on the period of the incoming signal
  if (OnTime > 400 && OnTime < 3000) {
  Serial.print("550Hz detected\n");
  return 550;
  } else if (OnTime > 8000) {
  Serial.print("23Hz detected\n");
  return 23;
  } else {
  Serial.print("Nothing detected\n");
  return 0;
  }
}

void headToBeacon() {
  if (BeaconFreq == 550 || BeaconFreq == 23) {
  moveForward();
  Serial.print("Driving forward\n");
  delay(1000);
  // return; // Exit the function after waiting for 6 seconds
  } else {
  rotateLeft();
  delay(100);

  stopCar();
  Serial.print("Driving left\n");
  delay(1000);
  }
  BeaconFreq = detectFrequency();
}

void AligningRobot() {
  int init_dist = cal_normDis();
  int cur_dist;
  int find_dir = 1; // If 1 => Go straight and check if distance decrease, 0 => turn

  while (find_dir == 1) {
  moveForward();
  delay(100);
  stopCar();
  cur_dist = cal_normDis();
  Serial.print("Align Robot: ");
  Serial.println(cur_dist);
  if (cur_dist < init_dist - 150) {
  Serial.println("Getting Closer");
  find_dir = 0;
  delay(1000);
  }
  else {
  Serial.println("Still Finding...");
  rotateRight();
  delay(5);
  stopCar();
  // mStop();
  delay(500);
  }
  }
}

void Go_to_police() {
  while (cal_normDis() > 200) {
  Serial.println("Car is Aligning...");
  AligningRobot();
  Serial.println("Moving towards the Police Car");
  moveForward();
  delay(100);
  stopCar();
  delay(500);

  }
  Serial.println("Pushing Police Car");
  moveForward();
  delay(1000);
  stopCar();
  delay(500);
}

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_DIR1, OUTPUT);
  pinMode(MOTOR_LEFT_DIR2, OUTPUT);

  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR1, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR2, OUTPUT);
  Wire.begin(21, 22);

  pinMode(BEACON_PIN, INPUT);
  pinMode(taskwall, INPUT);
  pinMode(taskbeacon, INPUT);
  pinMode(taskvive, INPUT);

  // wait until serial port opens for native USB devices
  while (! Serial) { delay(1); }

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);

  Serial.println(F("Shutdown pins inited..."));

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);

  Serial.println(F("Both in reset mode...(pins are low)"));
  

  Serial.println(F("Starting..."));
  setID();

  WiFi.mode(WIFI_AP_STA);
  WiFi.config(myIP, IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  WiFi.begin(ssid, password);
  UDPServer.begin(UDPPORT);
  Serial.printf("Team #%d ", teamNumber);
  Serial.print("Connecting to "); Serial.println(ssid);
  while(WiFi.status()!=WL_CONNECTED){
  delay(200); Serial.print(".");
  }
  Serial.println("WiFi connected to "); Serial.print(WiFi.localIP());
  vive1.begin();
  Serial.println("Vive trackers started");
 
}

void loop() {
  wall = digitalRead(taskwall);
  beacon = digitalRead(taskbeacon);
  vive = digitalRead(taskvive);

  if (taskwall == HIGH){
  //Serial.print("WALL FOLLOWING STARTS");
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!

  // print sensor one reading
  Serial.print(F("1: "));
  if(measure1.RangeStatus != 4) {     // if not out of range
  distfront = measure1.RangeMilliMeter;
    Serial.print(distfront);
  } else {
    distfront = 0;
  }
  
  Serial.print(F(" "));

  // print sensor two reading
  Serial.print(F("2: "));
  if(measure2.RangeStatus != 4) {
    distleft = measure2.RangeMilliMeter;
    Serial.print(distleft);
  } else {
    distleft = 0;
  }
  
  Serial.println();

  if (distfront < 350){
    stopCar();
    delay(300);

    rotateRight();
    delay(300);
  }

  else if (distleft > 250){
    veerLeft();
    delay(75); 
  }
   
  else if (distleft < 150){
    veerRight();
    delay(75);
  } 

  else {
    moveForward();
    delay(50);
  }
  }

  else if (taskbeacon == HIGH){
    BeaconFreq = detectFrequency();
    headToBeacon();
  }

  else if (taskvive == HIGH){
    handleUDPServer();
    ViveSender();
    Go_to_police();
    delay(2000);
  }

  else {
    stopCar();
    delay(1000);
  }
  
}
