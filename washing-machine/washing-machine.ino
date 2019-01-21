/*
Copyright (C) 2019  Xavier Héroult <xavier@placard.fr.eu.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <Button.h>

#if defined (__AVR_ATtiny85__)
#define BUTTON_SETUP 0
#define BUTTON_PLAY 1
#define STATUS_LED 2
#define ON_OFF 3
#define PLAY_PAUSE 4
#else
#define BUTTON_SETUP 4 
#define BUTTON_PLAY 5
#define STATUS_LED LED_BUILTIN
#define ON_OFF 2
#define PLAY_PAUSE 3
#define DEBUG ON
#endif

#define TIME_BEFORE_SETUP 200
#define TIME_DISPLAY 200
#define MAX_HOURS 9
#define DEFAULT_HOURS 4
#define SECONDS_PER_HOUR 3600

#define ON_OFF_TIME 5000
#define PAUSE_BETWEEN_ACTIONS 3000
#define PLAY_PAUSE_TIME 1000

#define MAIN_LOOP_SECONDS 2000
#define END_BLINK_TIME 15
#define END_LOOP_PAUSE 5000

int requested_hours;

void blink(int time, int dest) {
  for (int i = 0; i < time; i++) {
    digitalWrite(dest, HIGH);
    delay(TIME_DISPLAY);
    digitalWrite(dest, LOW);
    delay(TIME_DISPLAY);
  }
}

void start_washing_machine() {
  digitalWrite(ON_OFF, HIGH);
  delay(ON_OFF_TIME);
  digitalWrite(ON_OFF, LOW);
  delay(PAUSE_BETWEEN_ACTIONS);
  digitalWrite(PLAY_PAUSE, HIGH);
  delay(PLAY_PAUSE_TIME);
  digitalWrite(PLAY_PAUSE, LOW);
}

void end_display(int dest) {
  for (int i = 0; i < END_BLINK_TIME; i++) {
    digitalWrite(dest, HIGH);
    delay(100);
    digitalWrite(dest, LOW);
    delay(100);
  }
}

void end_loop() {
  while(true) {
    end_display(STATUS_LED);
    delay(END_LOOP_PAUSE);
  }
}

void setup() {
  Button setup_button = Button(BUTTON_SETUP, BUTTON_PULLDOWN);
  Button play_button = Button(BUTTON_PLAY, BUTTON_PULLDOWN);
  
  pinMode(STATUS_LED, OUTPUT);
  pinMode(ON_OFF, OUTPUT);
  pinMode(PLAY_PAUSE, OUTPUT);
  
#if defined(DEBUG)
  Serial.begin(9600);
#endif
  requested_hours = DEFAULT_HOURS;
  
  // show us the default setting
  delay(TIME_BEFORE_SETUP);
  blink(requested_hours, STATUS_LED);
  
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
  int end_time = (millis() / 1000) + requested_hours * SECONDS_PER_HOUR - 1;
#if defined(DEBUG)
  Serial.println("Main loop");
  Serial.print("requestes hours: ");
  Serial.print(requested_hours);
  Serial.print("h - endTime: ");
  Serial.print(end_time);
  Serial.println("s");
#endif
  
  while (true) {
    remaining_seconds = end_time - (millis() / 1000);
    remaining_hours = remaining_seconds / SECONDS_PER_HOUR;
#if defined(DEBUG)
    Serial.print("remaining secsonds: ");
    Serial.print(remaining_seconds);
    Serial.print("s - remaining hours: ");
    Serial.print(remaining_hours);
    Serial.print("h - end time: ");
    Serial.print(end_time);
    Serial.print("s - now: ");
    Serial.print(millis()  / 1000);
    Serial.println("s");
#endif
    if (remaining_seconds <= 0) {
      start_washing_machine();
      end_loop();
    } else {
      blink(remaining_hours + 1, STATUS_LED);
    }
    delay(MAIN_LOOP_SECONDS);
  }
}
