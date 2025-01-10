#include "html510.h"
HTML510Server h(80);


const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html>
<body>
<h1>LED CONTROL</h1>
<label for="frequencyRange">Frequency-> 3.0 Hz</label>
<input type="range" id="frequencyRange" min="3" max="30" step="1" value="25">
<span>30.0 Hz</span>
<br>
<label for="dutyCycleRange">Duty Cycle-> 0%</label>
<input type="range" id="dutyCycleRange" min="0" max="100"  value="50">
<span>100%</span>
<br>
<script>      
frequencyRange.onchange = function() {
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
document.getElementById("frequencyValue").innerHTML = this.responseText;
}
};
var str = "sliderfval=";
var res = str.concat(this.value);
xhttp.open("GET", res, true);
xhttp.send();
}

dutyCycleRange.onchange = function() {
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
document.getElementById("dutyCycleValue").innerHTML = this.responseText;
}
};
var str = "sliderdval=";
var res = str.concat(this.value);
xhttp.open("GET", res, true);
xhttp.send();
}

</script>
</body>
</html>
)===";




const char* ssid = "yooo";



const IPAddress myIP(192,168,1,124);//MINE

const int ledPin = 5;      // GPIO pin connected to the LED
const int ledChannel = 1;  // LEDC channel for the LED

void handleRoot(){
h.sendhtml(body);
}
void handlefSlider(){ // slider for frequency
int freq=h.getVal();
ledcSetup(ledChannel, freq, 14);
ledcAttachPin(ledPin, ledChannel);
}

void handledSlider(){ // slider for dutycycle
int dc = h.getVal();
int dutyCycle = map(dc, 0, 4095, 0, 16384);
  ledcWrite(ledChannel, dutyCycle);
  delay(100);
//h.sendplain();
}

void setup() {

  //setting up access point
Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
ledcSetup(ledChannel, 25, 14);
ledcAttachPin(ledPin, ledChannel);

Serial.print("Access point "); Serial.print(ssid);
WiFi.softAP( ssid) ; 

WiFi.softAPConfig(myIP, IPAddress(192,168,1,1), IPAddress(255, 255, 255, 0));
Serial.print(" AP IP address"); Serial.println(myIP);

h.begin();
h.attachHandler("/ ",handleRoot);
h.attachHandler("/sliderfval=",handlefSlider);
h.attachHandler("/sliderdval=",handledSlider);

}



void loop() {
  h.serve();
  delay(10); // Adjust delay as needed for smooth control
}