#include <Button.h>

#define BUTTON_SETUP 0
#define BUTTON_PLAY 1
#define STATUS_LED 2

#define ON_OFF 3
#define PLAY_PAUSE 4

#define TIME_DISPLAY 200
#define MAX_HOURS 9
#define DEFAULT_HOURS 1
#define SECONDS_PER_HOUR 10

#define ON_OFF_TIME 1000
#define PAUSE_BETWEEN_ACTIONS 4000
#define PLAY_PAUSE_TIME 1000

#define MAIN_LOOP_SECONDS 2000

int requested_hours;

void blink(int time, int dest) {
  for (int i = 0; i < time; i++) {
    digitalWrite(dest, HIGH);
    delay(TIME_DISPLAY);
    digitalWrite(dest, LOW);
    delay(TIME_DISPLAY);
  }
}

void startWashingMachine() {
  digitalWrite(ON_OFF, HIGH);
  delay(ON_OFF_TIME);
  digitalWrite(ON_OFF, LOW);
  delay(PAUSE_BETWEEN_ACTIONS);
  digitalWrite(PLAY_PAUSE, HIGH);
  delay(PLAY_PAUSE_TIME);
  digitalWrite(PLAY_PAUSE, LOW);
}

void endLoop() {
  while(true) {
    delay(10000);
  }
}

void setup() {
  Button setup_button = Button(BUTTON_SETUP, BUTTON_PULLDOWN);
  Button play_button = Button(BUTTON_PLAY, BUTTON_PULLDOWN);
  
  pinMode(STATUS_LED, OUTPUT);
  pinMode(ON_OFF, OUTPUT);
  pinMode(PLAY_PAUSE, OUTPUT);
  
  Serial.begin(9600);
  requested_hours = DEFAULT_HOURS;
  
  while (!play_button.isPressed()) {
    if (setup_button.isPressed()) {
      requested_hours++;
      if (requested_hours > MAX_HOURS) {
        requested_hours = 1;
      }
      blink(requested_hours, STATUS_LED);
    }
  }
}

void loop() {
  int remaining_seconds = 0;
  int remaining_hours = 0;
  int endTime = requested_hours * SECONDS_PER_HOUR;
  
  while (true) {
    remaining_seconds = endTime - (millis() / 1000);
    remaining_hours = remaining_seconds / SECONDS_PER_HOUR;
    
    Serial.print(endTime);
    Serial.print(" - ");
    Serial.println(remaining_seconds);

    if (remaining_seconds <= 0) {
      startWashingMachine();
      endLoop();
    } else {
      blink(remaining_hours + 1, STATUS_LED);
    }
    delay(MAIN_LOOP_SECONDS);
  }
}
