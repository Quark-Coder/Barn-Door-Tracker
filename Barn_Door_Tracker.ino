#define DIR 2
#define STEP 3
#define SPEED_BTN 4
#define EN_LED 5
#define doubleSpeed_LED 6
#define CHANGE_SPEED 7
#define EN 8
#define REWIND 9
#define END_SWITCH 10

int isSpeedX2 = 0;
int isEnable = 0;
int isRewinding = 0;
int isRewindingEnded = 0;
int PulseStatus = 0;

bool rewinding = false;  
bool enable = false;
bool speed_x2 = false;

unsigned long Speed = 37500;  //  Ajust for proper speed in your setup
unsigned long NextTime = 0;     

void setup() {
  
  pinMode(DIR, OUTPUT);  
  digitalWrite(DIR, LOW); 
  
  pinMode(STEP, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(EN_LED, OUTPUT);
  pinMode(doubleSpeed_LED, OUTPUT);
  
  pinMode(CHANGE_SPEED, INPUT);
  pinMode(SPEED_BTN, INPUT);
  
  Serial.begin(115200);
}

void loop() {
  isSpeedX2 = digitalRead(CHANGE_SPEED);
  isEnable = digitalRead(SPEED_BTN);
  isRewinding = digitalRead(REWIND); 
  isRewindingEnded = digitalRead(END_SWITCH); 

  if(isSpeedX2 == HIGH) {
  speed_x2 = true;
  digitalWrite(DIR, HIGH); 
  } else {
    speed_x2 = false;
    digitalWrite(DIR, LOW); 
  }

  if(isEnable == HIGH ) {
    enable = true;
  } else {
    enable = false;
  }

  if(isRewinding == HIGH) {
    rewinding = true;
    speed_x2 = false;
    enable = false;
  }
  
  if(rewinding == true) {
    digitalWrite(DIR, HIGH);
    Speed = 200;
  } 
  
  if((isRewindingEnded == HIGH) && (rewinding == true)) {
    digitalWrite(DIR, LOW);
    Speed = 37500;
    rewinding = false;
  }

  if(enable == true) {
	digitalWrite(EN_LED, HIGH);
    digitalWrite(EN, LOW);
  } else if((enable == false) && (rewinding == false)) {
	digitalWrite(EN_LED, LOW);
	digitalWrite(EN, HIGH);
  }

  if(speed_x2 == true) {
    digitalWrite(doubleSpeed_LED, HIGH);
    Speed = 37500 / 2;
  } else if((speed_x2 == false) && (rewinding == false)) {
	digitalWrite(doubleSpeed_LED, LOW);
	Speed = 37500;
  }
  
  if(micros() > NextTime) {     
    digitalWrite(STEP, HIGH);  
    NextTime += Speed;         
    PulseStatus=1;                 
  }
  
  if( PulseStatus & micros() > NextTime - (Speed/2)) {   
    digitalWrite(STEP, LOW);
    PulseStatus=0;              
  }                                 
}
