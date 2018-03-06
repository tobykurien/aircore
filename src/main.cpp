#include <Arduino.h>

// Code adapted from https://forum.arduino.cc/index.php?topic=22480.0

//the mathematical constant pi, because we are using radians instead of degrees.
// 360 degrees = 2*pi, 180 = pi, 90 = pi/2, etc.
const float pi = 3.14159;

const float stepSize = 0.1; //This is the step size for incrementing the angle (in radians)

//this is the maximum resolution of the PWM output.
const int maxAnalogRes = 255;

/* Define the pins the meter is connected to. We need 1 PWM to vary the voltage in
* each coil, and two more pins to reverse the polarity in that coil. So a total
* of 6 pins (3 per coil).
*/
const int sinPin = 9;     // ENA
const int cosPin = 10;    // ENB
const int sinDirPin1 = 2; // IN1
const int sinDirPin2 = 4; // IN2
const int cosDirPin1 = 7; // IN3
const int cosDirPin2 = 8; // IN4

// SIN+ -> OUT1
// SIN- -> OUT2
// COS+ -> OUT3
// COS- -> OUT4

const int tachoPin = 3;
int millisPer1000rpm = 60;
unsigned long tachoTime = 0;
unsigned long timeElapsed = 0;

// milliseconds between each angular step.
int delayTime = 40;

// setMeterPosition() - put it at the angle in radians
void setMeterPosition(float pos)
{

    // Calculate the voltage on the PWM pins based on the angle we want
    float sinCoilValue = maxAnalogRes * sin(pos);
    float cosCoilValue = maxAnalogRes * cos(pos);

    // change the polarity of the coil depending on the sign of the voltage level
    if (sinCoilValue <= 0)
    {
        digitalWrite(sinDirPin1, LOW);  //Change to LOW if using relays
        digitalWrite(sinDirPin2, HIGH); //comment this line out if using relays
    }
    else
    {
        digitalWrite(sinDirPin1, HIGH); //Change to HIGH if using relays
        digitalWrite(sinDirPin2, LOW);  //comment this line out if using relays
    }
    if (cosCoilValue <= 0)
    {
        digitalWrite(cosDirPin1, LOW);  //Change to LOW if using relays
        digitalWrite(cosDirPin2, HIGH); //comment this line out if using relays
    }
    else
    {
        digitalWrite(cosDirPin1, HIGH); //Change to HIGH if using relays
        digitalWrite(cosDirPin2, LOW);  //comment this line out if using relays
    }

    // take the absolute value because analogWrite doesn't take negatives
    sinCoilValue = abs(sinCoilValue);
    cosCoilValue = abs(cosCoilValue);

    analogWrite(sinPin, sinCoilValue);
    analogWrite(cosPin, cosCoilValue);
}

void tachoInterrupt() {
    if (tachoTime > 0) {
        timeElapsed = millis() - tachoTime;
        tachoTime = millis();
        if (timeElapsed > 200) timeElapsed = 200;
        if (timeElapsed < 0) timeElapsed = 0;
    } else {
        tachoTime = millis();
    }
}

void setup()
{
    Serial.begin(9600);

    //Set the pins to OUTPUT
    pinMode(sinPin, OUTPUT);
    pinMode(cosPin, OUTPUT);
    pinMode(sinDirPin1, OUTPUT);
    pinMode(sinDirPin2, OUTPUT);
    pinMode(cosDirPin1, OUTPUT);
    pinMode(cosDirPin2, OUTPUT);

    // Set the initial direction as "forward". Of course it could be "reverse" depending on which lead you connected :-).
    digitalWrite(sinDirPin1, HIGH);
    digitalWrite(sinDirPin2, LOW);
    digitalWrite(cosDirPin1, HIGH);
    digitalWrite(cosDirPin2, LOW);

    setMeterPosition(0);
    attachInterrupt(digitalPinToInterrupt(tachoPin), tachoInterrupt, RISING);
}

void loop() // run over and over again
{
    if (timeElapsed > 0) {
        float pos = map(timeElapsed, 200, 8.57, 0, pi/5*7);
        setMeterPosition(pos);
        Serial.print(timeElapsed, DEC);
        Serial.print(" ");
        Serial.println(pos, DEC);
    } else {
        //setMeterPosition(0);
    }

    delay(40);

    // //Rotate 0 through 360 degrees (OK, really 2*pi radians, but who cares?)
    // for (float i = 0; i < (2 * pi); i = i + stepSize)
    // {
    //     setMeterPosition(i);
    //     delay(delayTime); // waits for delayTime milliseconds
    // }

    // //Rotate 360 through 0 degrees
    // for (float i = (2 * pi); i > 0; i = i - stepSize)
    // {
    //     setMeterPosition(i);
    //     delay(delayTime); // waits for delayTime milliseconds
    // }
}
