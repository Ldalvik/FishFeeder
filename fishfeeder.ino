#include <Particle.h>

//Pinouts
#define motor1 D0
#define motor2 D1
#define encoder D2

//Switch/case states
#define FEED 0
#define WAIT 1
#define ERROR 2
#define CHECK 3

//Motor modes
#define FORWARD 0
#define REVERSE 1
#define OFF 2

int state = WAIT; //Default state

volatile unsigned int currentRotation = 0; //Current rotations counted by encoder

int feedHour = 10; //Hour to autofeed
int feedMinute = 30; //Minute to autofeed
int maxRotations = 1000; //Max allowed rotations
int rotations = 100; //Amount of rotations when feeding
int currentHour; //Current hour
int currentMinute; //Current minute
int currentSecond; //Current second
bool manualFeed = false; //Check if fed manually

void setup() {
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(encoder, INPUT_PULLUP);
  attachInterrupt(encoder, pulse, RISING); //Count rotations of encoder
  Time.zone(-5);
  print("Start", "Flashed @ " + String(Time.hour()) + ":" + String(Time.minute()));
}

void loop() {
  currentHour = Time.hour(); //Set current hour
  currentMinute = Time.minute(); //Set current minute
  currentSecond = Time.second(); //Set current second;
  Particle.function("fish_feeder", feeder);

  switch (state) {
    case FEED: //Is feeding fish
      if (currentRotation > maxRotations) { //Checks if rotation ever exceeds max rotations, state switches to ERROR if true
        state = ERROR;
      }
      if (currentRotation < rotations) { //If rotations counted by the encoder are lower than the set rotation, the motor will spin
        motor(FORWARD);
      }
      if (currentRotation >= rotations) { //If rotations are equal or more than the set rotation, motor off and state switches to CHECK
        motor(OFF);
        state = CHECK;
      }
      break;
    case WAIT: //Waiting to feed fish
      currentRotation = 0;
      motor(OFF);
      if ((currentHour == feedHour) && (currentMinute == feedMinute)) { //Check if current time is the set hour and minute
        print("log", String(rotations));
        state = FEED;
      }
      break;
    case CHECK: //Wait for time to change to keep from feeding the whole minute
      if (currentMinute > feedMinute || manualFeed) {
        state = WAIT;
        manualFeed = false;
      }
      break;
    case ERROR: //Rotations exceeded max allowed rotations
      print("log", "Rotation exceeded " + String(maxRotations) + ", stopping feed.");
      state = WAIT;
      break;
  }
}

int feeder(String command) {
  if (strstr(command, "daily=")) { //If command contains "daily="
    int equal = command.indexOf("=") + 1; //Receives "daily={rotations},{feedHour},{feedMinute}" (E.g. "daily=500,12,30")
    String vars = command.remove(0, equal); //Remove parameter
    rotations = getValue(vars, ',', 0).toInt(); //Gets rotations
    feedHour = getValue(vars, ',', 1).toInt(); //Gets hour
    feedMinute = getValue(vars, ',', 2).toInt(); //Gets minute
    print("log", "Feeding at " + String(feedHour) + ":" + String(feedMinute) + " for " + String(rotations) + " rotations.");
    return 3;
  }
  
  if (strstr(command, "feed=")) { //If command contains "daily="
    int equal = command.indexOf("=") + 1; //Receives "daily={rotations},{feedHour},{feedMinute}" (E.g. "daily=500,12,30")
    String vars = command.remove(0, equal); //Remove parameter
    rotations = getValue(vars, ',', 0).toInt(); //Gets rotations
    manualFeed = true;
    state = FEED;
    print("log", "Feeding for " + String(rotations) + " rotations.");
    return 2;
  }
  
  if (strstr(command, "status")) { //If command contains "status"
    status();
    return 1;
  }
}

//Interrupt function for encoder
void pulse() {
  currentRotation++; //Counts up while encoder is running
}

//Control motor
void motor(int mode) {
  if (mode == FORWARD) { //Spin motor forward
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, HIGH);
  }
  if (mode == REVERSE) { //Spin motor backward
    digitalWrite(motor1, HIGH);
    digitalWrite(motor2, LOW);
  }
  if (mode == OFF) { //Turn motor off
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
  }
}

//Print to console
void print(String title, String description) {
  Particle.publish(title, description, PRIVATE);
}

//Print values of daily feed variables
void status() {
  print("Daily feed values",
        "{\"currentRotation\":" + String(currentRotation) +
        ",\"feedHour\":" + String(feedHour) +
        ",\"feedMinute\":" + String(feedMinute) + "}");
}

//Split string by character
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
