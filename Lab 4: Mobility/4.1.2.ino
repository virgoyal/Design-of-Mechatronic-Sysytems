
const int potPin = 4;     // GPIO pin connected to the potentiometer
const int ledPin = 5;      // GPIO pin connected to the LED
const int ledChannel = 1;  // LEDC channel for the LED
const int freq = 30;       // Frequency in Hz


void setup() {
  pinMode(potPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);  // Set the baud rate to 115200
  // LEDC configuration
  ledcSetup(ledChannel, freq, 14);
  ledcAttachPin(ledPin, ledChannel);
}
//(30*2^14 = 0.0.49152 MHz or 491.52 kHz ) within range >39kHz <40MHz
void loop() {
  // Read the potentiometer value
  int potValue = analogRead(potPin);
  //Serial.println(potValue);

  // Map the potentiometer value to the LEDC duty cycle range (0-255)
  int dutyCycle = map(potValue, 0, 4095, 0, 16384);
  //Serial.println(dutyCycle);

  // Set the LEDC duty cycle
  ledcWrite(ledChannel, dutyCycle);
  delay(100);
}
