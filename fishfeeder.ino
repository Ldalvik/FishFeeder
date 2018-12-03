#include <Particle.h>

SYSTEM_MODE(AUTOMATIC);

#define motor1 D1
#define motor2 D0
#define encoder D2

#define FEED 0
#define WAIT 1
#define ERROR 2
#define CHECK 3

#define FORWARD 0
#define REVERSE 1
#define OFF 2

int state = WAIT;

volatile unsigned int currentRotation = 0;

int feedHour;
int feedMinute;
int rotations;
int currentHour;
int currentMinute;
int currentSecond;

Timer t(60000, status);

void setup() {
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(encoder, INPUT_PULLUP);
  attachInterrupt(encoder, pulse, RISING);
  //t.start();
  Time.zone(-5);
  feedHour = 9;
  feedMinute = 10;
  rotations = 500;
  print("Start", "Flashed @ " + String(Time.hour()) + ":" + String(Time.minute()));
}

void loop() {
  currentHour = Time.hour();
  currentMinute = Time.minute();
  currentSecond = Time.second();
  Particle.function("fish_feeder", feeder);

  switch (state) {
    case FEED:
      if (currentRotation > 1000) {
        state = ERROR;
      }
      if (currentRotation < rotations) {
        motor(FORWARD);
      }
      if (currentRotation >= rotations) {
        state = CHECK;
      }
      break;
    case WAIT:
        currentRotation = 0;
        motor(OFF);
      if ((currentHour == feedHour) && (currentMinute == feedMinute)) {
        print("log", "Starting feed.");
        state = FEED;
      }
      break;
    case CHECK:
        if(currentMinute != feedMinute){
            state = WAIT;
      }
      break;
    case ERROR:
      print("log", "Rotation exceeded 1000, stopping feed.");
      state = WAIT;
      break;
  }
}

int feeder(String command) {
  if (strstr(command, "daily=")) {
    int equal = command.indexOf("=") + 1;
    String vars = command.remove(0, equal);
    rotations = getValue(vars, ',', 0).toInt();
    feedHour = getValue(vars, ',', 1).toInt();
    feedMinute = getValue(vars, ',', 2).toInt();
    nextFeed();
    return 1;
  }
}

void pulse() {
  currentRotation++;
}

void motor(int mode) {
    if(mode == FORWARD){
        digitalWrite(motor1, LOW);
        digitalWrite(motor2, HIGH);
    }
    if(mode == REVERSE){
        digitalWrite(motor1, HIGH);
        digitalWrite(motor2, LOW);
    }
    if(mode == OFF){
        digitalWrite(motor1, LOW);
        digitalWrite(motor2, LOW);
    }
}

void print(String title, String description) {
  Particle.publish(title, description);
}

void status() {
  print("{currentRotations}", String(currentRotation));
  print("{hour}", String(feedHour));
  print("{minute}", String(feedMinute));
  print("{rotations}", String(rotations));
}

void nextFeed() {
  int hours = feedHour - currentHour;
  int minutes = feedMinute - currentMinute;
  print("log", "Feeding in " + String(hours) + " hours and " + String(minutes) + " minutes for " + String(rotations) + " rotations.");
}

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
