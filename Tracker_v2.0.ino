#define DIR_PIN 2
#define STEP_PIN 3
#define TRACKING_PIN 4
#define TRACKING_LED_PIN 5
#define OPEN_TRACKER_LED_PIN 6
#define OPEN_TRACKER_PIN 7
#define ENABLE_DRIVER_PIN 8
#define CLOSE_TRACKER_PIN 9
#define PARK_PIN 10
#define MOVE_N_DEGREES_PIN 11

const unsigned long TRACKING_SPEED = 37030;
const unsigned long FAST_SPEED = 200;

volatile bool trackingEnabled = false;
volatile bool isOpening = false;
volatile bool isClosing = false;
volatile bool isMovingToDegrees = false;
unsigned long stepDelay = TRACKING_SPEED;
unsigned long nextStepTime = 0;
unsigned long moveStartTime = 0; 

void setup() {
    pinMode(DIR_PIN, OUTPUT);
    digitalWrite(DIR_PIN, LOW);

    pinMode(STEP_PIN, OUTPUT);
    pinMode(TRACKING_PIN, INPUT);
    pinMode(TRACKING_LED_PIN, OUTPUT);
    pinMode(OPEN_TRACKER_PIN, INPUT);
    pinMode(OPEN_TRACKER_LED_PIN, OUTPUT);
    pinMode(CLOSE_TRACKER_PIN, INPUT);
    pinMode(MOVE_N_DEGREES_PIN, INPUT);
    pinMode(PARK_PIN, INPUT);
    pinMode(ENABLE_DRIVER_PIN, OUTPUT);

    digitalWrite(ENABLE_DRIVER_PIN, HIGH);
}

void loop() {
    handleButtons();
    checkMoveTimeout(); 
    generateStepPulse();
}

void handleButtons() {
    if (digitalRead(TRACKING_PIN) == HIGH) {
        toggleTracking(true);
    } else {
        toggleTracking(false);
    }

    if (digitalRead(OPEN_TRACKER_PIN) == HIGH) {
        openTracker(); 
    } else if (isOpening) {
        stopOperation();
    }

    if (digitalRead(CLOSE_TRACKER_PIN) == HIGH && !isClosing) {
        closeTracker();
    }

    if (digitalRead(MOVE_N_DEGREES_PIN) == HIGH && !isMovingToDegrees) {
        moveToDegrees();
    }

    if (digitalRead(PARK_PIN) == HIGH && isClosing) {
        stopOperation();
        if (trackingEnabled) toggleTracking(true);
    }
}

void toggleTracking(bool enable) {
    if (isOpening || isClosing || isMovingToDegrees) return;
    
    trackingEnabled = enable;
    digitalWrite(TRACKING_LED_PIN, enable ? HIGH : LOW);
    stepDelay = enable ? TRACKING_SPEED : 0;
    digitalWrite(ENABLE_DRIVER_PIN, enable ? LOW : HIGH);
}

void openTracker() {
    trackingEnabled = false;
    digitalWrite(TRACKING_LED_PIN, LOW);
    isOpening = true;
    stepDelay = FAST_SPEED;
    digitalWrite(ENABLE_DRIVER_PIN, LOW);
    digitalWrite(DIR_PIN, LOW);
    digitalWrite(OPEN_TRACKER_LED_PIN, HIGH);
}

void closeTracker() {
    trackingEnabled = false;
    digitalWrite(TRACKING_LED_PIN, LOW);
    isClosing = true;
    stepDelay = FAST_SPEED;
    digitalWrite(ENABLE_DRIVER_PIN, LOW);
    digitalWrite(DIR_PIN, HIGH);
}


void moveToDegrees() {
    trackingEnabled = false;
    digitalWrite(TRACKING_LED_PIN, LOW);
    isMovingToDegrees = true;
    digitalWrite(DIR_PIN, LOW); 
    digitalWrite(ENABLE_DRIVER_PIN, LOW);
    stepDelay = FAST_SPEED;
    moveStartTime = millis();
}

void stopOperation() {
    isOpening = false;
    isClosing = false;
    digitalWrite(ENABLE_DRIVER_PIN, HIGH);
    trackingEnabled = false;
    stepDelay = 0;
    digitalWrite(OPEN_TRACKER_LED_PIN, LOW);
    digitalWrite(DIR_PIN, LOW);
}

void generateStepPulse() {
    if (stepDelay == 0) return;

    unsigned long currentTime = micros();
    if (currentTime > nextStepTime) {
        digitalWrite(STEP_PIN, HIGH);
        nextStepTime = currentTime + stepDelay;
    }

    if (currentTime > nextStepTime - (stepDelay / 2)) {
        digitalWrite(STEP_PIN, LOW);
    }
}


void checkMoveTimeout() {
    if (isMovingToDegrees && (millis() - moveStartTime >= 16000)) {
        isMovingToDegrees = false;
        stepDelay = 0;
        digitalWrite(ENABLE_DRIVER_PIN, HIGH);
        
        if (trackingEnabled) {
            toggleTracking(true);
        }
    }
}