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
#include <avr/sleep.h>

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
#define DEFAULT_HOURS 5
#define SECONDS_PER_HOUR 3600

#define ON_OFF_TIME 5000
#define PAUSE_BETWEEN_ACTIONS 3000
#define PLAY_PAUSE_TIME 1000

#define END_BLINK_TIME 8
#define END_BLINK_DELAY 80

// Routines to set and clear bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int requested_hours;
volatile int f_timer = 0;

void system_sleep(int mode) {
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
  set_sleep_mode(mode);
  sleep_enable();
  sleep_mode();                        // System actually sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out 
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}

// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {
  byte bb;
  int ww;
  
  if (ii > 9 ) { 
    ii=9;
  }
  
  bb = ii & 7;
  if (ii > 7) {
    bb |= (1<<5);
  }
  
  bb |= (1<<WDCE);
  
  ww = bb;
 
  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}
  
// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_timer = 1;  // set global flag
}

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
    delay(END_BLINK_DELAY);
    digitalWrite(dest, LOW);
    delay(END_BLINK_DELAY);
  }
}

void end_loop() {
  setup_watchdog(9);
  f_timer = 1;
  while(true) {
    if (f_timer == 1) {
      f_timer = 0;
      end_display(STATUS_LED);
      system_sleep(SLEEP_MODE_PWR_DOWN);
    }
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
  
  // Show us the default setting.
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

  setup_watchdog(8);
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
  f_timer = 1;
  while (true) {
    if (f_timer == 1) {
      f_timer = 0;
      
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
     
      blink(remaining_hours + 1, STATUS_LED);
      if (remaining_seconds <= 0) {
        start_washing_machine();
        end_loop();
      }
      // Cannot set above, else the internal counter is stopped.
      system_sleep(SLEEP_MODE_IDLE); 
    }
  }
}
