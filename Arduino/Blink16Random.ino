#define DEBUG 0

#define PIN_DATA 11
#define PIN_CLOCK 13
#define PIN_LOAD_ENABLE 10

#define NUM_EYES 16
#define PERCENT_BLINK 10
#define MIN_BLINK_TIME 1100
#define MAX_BLINK_TIME 2500
#define PERCENT_SLEEP 2
#define MIN_SLEEP_TIME 10000
#define MAX_SLEEP_TIME 60000

unsigned long lastRun;

long eyes[NUM_EYES];

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  randomSeed(analogRead(0));

  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LOAD_ENABLE, OUTPUT);

  lastRun = millis();
  initEyes();
}

// the loop function runs over and over again forever
void loop() {
  delay(100);
  updateEyes();
  
  unsigned int lights = getLightsData();
  digitalWrite (PIN_LOAD_ENABLE, LOW);
  shiftOut (PIN_DATA, PIN_CLOCK, MSBFIRST, highByte (lights)); // send out upper 8 bits
  shiftOut (PIN_DATA, PIN_CLOCK, MSBFIRST, lowByte (lights)); // send out lower 8 bits
  digitalWrite (PIN_LOAD_ENABLE, HIGH);
}

void initEyes() {
  for (int i = 0; i < NUM_EYES; i++) {
    eyes[i] = random(5000);
  }  
}

void updateEyes() {
  unsigned long deltaTime = millis() - lastRun;
  #ifdef DEBUG
  Serial.print("Delta Time "); Serial.println(deltaTime);
  #endif
  lastRun = millis();

  for (int i = 0; i < NUM_EYES; i++) {
    if (eyes[i] > 0) {
      #ifdef DEBUG
      Serial.print("Decrement Eye from "); Serial.print(eyes[i]);
      #endif
      eyes[i] = max(0, eyes[i] - deltaTime);
      #ifdef DEBUG
      Serial.print(" to "); Serial.println(eyes[i]);
      #endif
    } else {
      unsigned int prcnt = random(100);
      #ifdef DEBUG
      Serial.print("random "); Serial.println(prcnt);
      #endif
      if (prcnt < PERCENT_SLEEP) {
        eyes[i] = max(MIN_SLEEP_TIME, random(MAX_SLEEP_TIME));
        #ifdef DEBUG
        Serial.print("Sleeping for "); Serial.println(eyes[i]);
        #endif
      } else if (prcnt < PERCENT_BLINK) {
        eyes[i] = max(MIN_BLINK_TIME, random(MAX_BLINK_TIME));
        #ifdef DEBUG
        Serial.print("Blinking for "); Serial.println(eyes[i]);
        #endif
      }
    }
  }  
}

unsigned int getLightsData() {
  unsigned int lights = 1;

  for (int i = 0; i < NUM_EYES; i++) {
    lights = lights << 1;
    if (eyes[i] <= 1000) {
      lights = lights | 1;
    }
  }

  return lights;
}
