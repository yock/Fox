#include "pitches.h"
#include "DTMF.h"

#define PTT_PIN 7 // Push-to-talk ping
#define TX_PIN 5 // Audio transmit pin
#define CW_TX_FREQ 700 // Morse code tone frequency
#define TX_DELAY 2000 // Delay before beginning audio transmission
#define DTMF_RX_PIN A0 // Receive pin for DTMF tones

char callsign[] = "KE8ATC";
DTMF dtmf = DTMF(128.0, 8926.0);
float magnitudes[8];
int pause = 400; // Unitless number that will be counted up to each loop before transmitting
char dtmfChar;

void setup() {
  pinMode(PTT_PIN, OUTPUT);
  digitalWrite(PTT_PIN, LOW);
  Serial.begin(9600);
}

void loop() {
  for (int x = 0; x < pause; x++) {
    dtmfChar = detectDTMFChar(magnitudes);
  }
}

char detectDTMFChar(float magnitudes[]) {
  dtmf.sample(DTMF_RX_PIN);
  dtmf.detect(magnitudes, 506);
  return dtmf.button(magnitudes, 1800.);
}
