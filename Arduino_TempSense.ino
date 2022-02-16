#include <pitches.h>

int melody[] = {
  NOTE_G3, NOTE_B3, NOTE_A3, NOTE_D3 //array of notes I researched that are needed for the largely known hunger games whistle
};

// note durations: 1 = one whole note
int lengthofnote[] = {
  1, 1, 1, 1
};


float RawReading; // raw reading obtained from sensor
float MeasuredTempCelcius;//raw reading -> voltage value -> temperature value
float MeasuredTempFaren; //converted celcius value

int LedPin = 10;
int LedState;
unsigned long lastupdate = 0; // last time LED was updated
long interval = 1000;  // blinking interval 


float MotorSpeed = 0; // Setting initial operation of fan to be off
int MotorPin = 6; // Pin that our DC motor is wired to

float MinimumTemp = 65; //Setting intial conditions for fan operation (temperature to initiate fan)
float MaximumTemp = 80; // Setting Temperature for fan to operate at 100% capacity

int SensorPin = A0; // Analog pin that the temperature sensor is wired to




void setup() {
Serial.begin(9600); //baud rate for serial data transmission
pinMode(SensorPin, INPUT); // Data acquired from temperature sensor is considered to be an input
pinMode(MotorPin, OUTPUT); // Data acquired from temperature sensor determins motor speed so it is an output
pinMode(LedPin, OUTPUT); // Using LED to notify user of critical temperature - i.e. get an ice pack
}


void loop() {
  
RawReading = analogRead(SensorPin); // reading the value from the temperature sensor
MeasuredTempCelcius = (RawReading*5.0)/1024.0; //convert the analog reading into voltage
MeasuredTempCelcius = (MeasuredTempCelcius - 0.6)*100; //subtract offset voltage - data sheet said 0.5 so I needed to play around with this a bit to get the correct value  
//Because manufacturing specifications call for 10mv per degrees celcius (or .01v/C), we divide or voltge measurement by this (ie multiply by 100) to get a value in Celcius
MeasuredTempFaren = (MeasuredTempCelcius*9/5) +32; //you know.. because this is the U.S.

if(MeasuredTempFaren < MinimumTemp) { // If the measured temperature is lower than our minimum value 
  //MotorSpeed = 0; // fan is not spinning
  digitalWrite(MotorPin, LOW); //turn Fan off
  digitalWrite(LedPin, LOW); //turn LED off
}


if((MeasuredTempFaren >= MinimumTemp) && (MeasuredTempFaren <= MaximumTemp)) { // If the measured temperature value is in our desired range
  unsigned long currenttime = millis(); //measuring time that has passed in milliseconds
  MotorSpeed = map(MeasuredTempFaren, MinimumTemp, MaximumTemp, 5, 250); // using the map function to change the physical motor speed
  analogWrite(MotorPin, MotorSpeed); // Writing the motor pin to the value dictated above 
    if (currenttime - lastupdate >= interval) { //if the difference between the current time and our last update is greater than our desired blinking interval, blink the LED
    lastupdate = currenttime; // saving the time at which we blinked the LED

      // depending on current LED state, switch the value 
      if (digitalRead(LedPin) == LOW) {
      digitalWrite(LedPin, HIGH);
      } 
      else {
      digitalWrite(LedPin, LOW);
      }
    }
}


if(MeasuredTempFaren  > MaximumTemp) { // if measured temperature is higher than maximum temperature
  MotorSpeed = 0; // fan is not spinning
  digitalWrite(MotorPin, MotorSpeed);
  digitalWrite(LedPin, HIGH);


   //hunger games whistle code using pitches.h library (library is just an array of notes)
    for (int thisNote = 0; thisNote < 5; thisNote++) {

    // note duration = 1 second / note type.
    
    int lengthofnote = 1000 / lengthofnote[thisNote];
    tone(8, melody[thisNote], lengthofnote); //writing the melody and note duration to our speaker pin

    // to distinguish between notes, we set the delay equal to 1.2*however long the duration of the note is
    int pause = lengthofnote * 1.20;
    delay(pause); //pause to distinguish notes
    noTone(8); // stop the tone from playing:
    }   
} 
  
String input1_output1 = String(MeasuredTempFaren) + "," + String(MotorSpeed);
Serial.println(input1_output1);
delay(2000);

}
