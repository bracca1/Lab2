#include <Arduino.h>

/*---------------Module Function Prototypes-----------------*/
// Interrupt handlers
void goHigh(void);
void goLow(void);

/*---------------Module Variables---------------------------*/
// volatile States_t state;
IntervalTimer highTimer;
IntervalTimer lowTimer;

// Output signal at 490Hz (period in microseconds)
static uint16_t SIGNAL_F = 490;
static uint16_t SIGNAL_T = 1 / SIGNAL_F * 1000000;

// Default duty cycle to 50%
static uint16_t lastPot = 512;
volatile static uint16_t high_T = map(lastPot, 0, 1023, 0, SIGNAL_T);

/*---------------Teensy Main Functions----------------------*/
void setup() {
  // Connect to Serial
  Serial.begin(9600);
  while(!Serial);

  // Input on Pin 23 (A9)
  pinMode(PIN_A9, INPUT);

  // Output on Pin 22 (A8)
  pinMode(PIN_A8, OUTPUT);
  highTimer.begin(goHigh, SIGNAL_T);
}

void loop() {
  // Pause interrupts to read input
  noInterrupts();
  uint16_t newInput = analogRead(PIN_A9);
  if (newInput != lastPot) {
    high_T = map(newInput, 0, 1023, 0, SIGNAL_T);
    lastPot = newInput;
  }
  interrupts();
}

/*----------------Module Functions--------------------------*/
// When the high timer goes off, set output to high
void goHigh(void) {
  digitalWrite(PIN_A8, HIGH);
  lowTimer.begin(goLow, high_T);
}

void goLow(void) {
  digitalWrite(PIN_A8, LOW);
  lowTimer.end();
}