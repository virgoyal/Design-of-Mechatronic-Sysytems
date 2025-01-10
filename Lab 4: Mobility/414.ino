#include "html510.h"
HTML510Server h(80);


const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html>
<body>
<h1>MOTOR CONTROL</h1>

<label for="frequencyRange">direction-> R</label>
<input type="range" id="frequencyRange" min="0" max="100" step="100" value="50">
<span>F</span>

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


const int motorpin = 4;
const int pin1 = 5;     
const int pin2 = 6;  
const int motorchannel = 1;

       
void handleRoot(){ // slider for direction
h.sendhtml(body);
}
void handledirSlider(){
int dir=h.getVal();
if (dir>49){
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
}
else{
  digitalWrite(pin2, HIGH);
  digitalWrite(pin1, LOW);
}

}

void handledSlider(){ // slider for dutycycle
char str [10];
char prnt [20];

int dc = h.getVal();
int dutyCycle = map(dc, 0, 100, 0, 16384);
  ledcWrite(motorchannel, dutyCycle);
  delay(100);
  
 

}

void setup() {
  //setting up access point
Serial.begin(115200);
  pinMode(motorpin, OUTPUT);
  pinMode(pin1, OUTPUT);pinMode(pin2, OUTPUT);
ledcSetup(motorchannel, 30, 14);
ledcAttachPin(motorpin, motorchannel);

Serial.print("Access point "); Serial.print(ssid);
WiFi.softAP( ssid) ; 

WiFi.softAPConfig(myIP, IPAddress(192,168,1,1), IPAddress(255, 255, 255, 0));
Serial.print(" AP IP address"); Serial.println(myIP);

h.begin();
h.attachHandler("/ ",handleRoot);
h.attachHandler("/sliderfval=",handledirSlider);
h.attachHandler("/sliderdval=",handledSlider);





}



void loop() {
  h.serve();
  delay(10); // Adjust delay as needed for smooth control
}