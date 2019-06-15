# Home Automation with Nodemcu ESP8266
Home automation with circuit diagram and code

ESP8266 with some electronics gives you control over your lights, fans or any electrical appliances leaving you resting on your sofa :).

## Circuit diagram:

![Circuit](images/circuit.png)

## Instructions

What you need:

* 1 x Nodemcu ESP8266
* 1 x ACS712 5A
* 1 x Full Bridge Rectifier / 4 x IN4007 diode
* 1 x PC817 optocoupler
* 2 x MOC3020 optocoupler / 2 x MOC3021 optocoupler
* 2 x BTA16 triac
* 2 x 33k ohm resistor
* 2 x 100 ohm resistor
* 4 x 1k ohm resistor
* 1 x switch
* 1 x Bulb Holder
* 1 x Plug
* 1 x Soldering Iron
* 2 m Soldering wire
* 1 x Perf Board (small)
* Jumper Wires

### Zero Cross Detection
AC Current is a sine wave ranging from x to -x, when the current is zero it is called *ZERO CROSSING*. If we can control a switch very fast(~ 10ms) to cut the sine wave, we can control the power. The Triac will be a suitable component to do so. If you are like me you will get 50Hz, so a sine wave will last 20ms and every zero crossing will occur every 10ms.  After we detect the zero crossing, we give a delay of 2000 microsenconds to 9000 microseconds (results may vary) then fire the triac, we can cut the sinwave accurately and control power given to the load.

<img src="images/sine_wave.png" width="500" hieght="300"/> 
 - Credit goes to ElectroNoobs Youtube

 To Detect Zero Crossing we will use a Full bridge Rectifier, PC817 optocoupler, 2 33k ohm ressistors and the D2 pin of the NodeMCU.
 
 Here is the scematics:
 
 
 ![Circuit](images/circuit_zcd.png)
 
 
 And Code:
 
 ```C
 void setup() {
  Serial.begin(115200);
  pinMode(D2, INPUT);
  attachInterrupt(digitalPinToInterrupt(D2), zcd, FALLING);
}

void loop() {
}

void zcd() {
  Serial.println("Now");
}
 ```
 
The input at D2 will be a trapezoidal wave (DC) with peak value of the voltage given to PIN4 at PC817, in this case it is 3.3v . We detect the LOW value using NodeMCU then Serial.println("NOW"). It should happen 100 times a second. 🎉 Zero Cross Detection is over.
 
 
 
### Firing The Triac


#### First Test


Ok now lets see if our triacs and MOC3020 optocoupler works.
We will just modify our circuit a bit and change a bit of our code.

Schematics:

![Circuit](images/circuit_zcd_triac.png)


Code:

```C
int delayto = 2000;
void setup() {
  Serial.begin(115200);
  pinMode(D2, INPUT);
  pinMode(D7, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(D2), zcd, FALLING);
}

void loop() {
}

void zcd() {
  delayMicroseconds(delayto);
  digitalWrite(D7, HIGH);
  delayMicroseconds(50);
  digitalWrite(D7, LOW);
}
```
__A Triac stays conductive till the polarity on the current changes___. So in this example we fire the triac after the zero cross with a delay of 2000 microseconds which, according to my test gives full power to my load. Changing the *delayto* from 2000 - 9000 can control power. Test this in both of the triacs and MOC3020. What is left is to add cloud functionality so that we can control the power from any where in the world (where there is Internet connection) and adding a two way switch.

#### Two Way Switch

### Cloud functionality

