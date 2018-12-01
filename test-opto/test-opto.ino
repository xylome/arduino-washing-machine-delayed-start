#define STATUS_LED LED_BUILTIN
#define ON_OFF 2
#define PLAY_PAUSE 3
#define BUTTON_1 4
#define DELAY 75

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ON_OFF, OUTPUT);
  pinMode(PLAY_PAUSE, OUTPUT);
  pinMode(BUTTON_1, INPUT);

  while(digitalRead(BUTTON_1) == LOW) {
    
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(DELAY);
}
