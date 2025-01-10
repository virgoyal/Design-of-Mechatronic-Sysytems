const int buttonPin = 4; // GPIO pin connected to the switch
const int ledPin = 5;    // GPIO pin connected to the LED

void setup() {
  pinMode(buttonPin, INPUT); // making BUTTONPin an input
  pinMode(ledPin, OUTPUT);// making ledPin an OUTput
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    digitalWrite(ledPin, HIGH); // Turn on the LED
  } else {
    digitalWrite(ledPin, LOW); // Turn off the LED
  }
}
