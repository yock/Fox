#include "DTMF.h"

#define PTT_PIN 7 // Push-to-talk ping
#define TX_PIN 5 // Audio transmit pin
#define CW_TX_FREQ 800 // Morse code tone frequency in Hz
#define DTMF_RX_PIN A0 // Receive pin for DTMF tones
#define DIT_LEN 90 // Length of a dit in milliseconds
#define TX_REST 5000 // Time between transmissions in milliseconds
#define DEBUG_LED 13 // LED to blink morse code patterns

String fromMorse(String input);

// 1 = dit , 3 = dah, 0 = space
// Letters/#                 A     B      C     D    E    F      G     H      I     J     K      L    M     N    O     P      Q      R     S    T    U      V     W      X     Y      Z       0        1      3       3       4       5       6       7       8       9
String morseTable[] = {"0", "13", "3111", "3131", "311", "1", "1131", "331", "1111", "11", "1333", "313", "1311", "33", "31", "333", "1331", "3313", "131", "111", "3", "113", "1113", "133", "3113", "3133", "3311", "33333", "13333", "11333", "11133", "11113", "11111", "31111", "33111", "33311", "33331"};

char callsign[] = "KE8ATC";
String message = "      D E     K E 8 A T C";
bool transmitEnabled = false;
bool transmitting = false;
String code = fromMorse(message);
int codeIndex = 0;
unsigned long nextToneAfter;
DTMF dtmf = DTMF(128.0, 8926.0);
float magnitudes[8];

void setup() {
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, LOW);
  pinMode(PTT_PIN, OUTPUT);
  digitalWrite(PTT_PIN, LOW);
  digitalWrite(DEBUG_LED, LOW);
  Serial.begin(9600);
}

void loop() {
  control();
  processCode();
}

void control() {
  if (transmitting) return;

  char button = detectDTMF(magnitudes);

  if (button == '1') {
    pttOff();
    transmitEnabled = false;
  }
  if (button == '2') {
    transmitEnabled = true;
  }
}

char detectDTMF(float magnitudes[]) {
  dtmf.sample(DTMF_RX_PIN);
  dtmf.detect(magnitudes, 506);
  return dtmf.button(magnitudes, 1800.);
}

void processCode() {
  if (!transmitEnabled) return;

  if (millis() >= nextToneAfter) {
    if (!transmitting) pttOn();
    if (code[codeIndex] == '0') {
      nextToneAfter = millis() + DIT_LEN;
    } else if (code[codeIndex] == '1') {
      nextToneAfter = millis() + (DIT_LEN * 2);
      digitalWrite(DEBUG_LED, HIGH);
      tone(TX_PIN, CW_TX_FREQ, DIT_LEN);
    } else if (code[codeIndex] == '3') {
      nextToneAfter = millis() + (DIT_LEN * 4);
      digitalWrite(DEBUG_LED, HIGH);
      tone(TX_PIN, CW_TX_FREQ, DIT_LEN * 3);
    }
    codeIndex++;
    if (codeIndex >= code.length()) {
      pttOff();
      nextToneAfter = millis() + TX_REST;
      codeIndex = 0;
    }
  }
}

String fromMorse(String input) {
  input.toUpperCase();
  String output = "";
  for (int i = 0; i < input.length() ; i++) {
    if (input[i] >= 65 && input[i] <= 90)
      output = output + morseTable[input[i] - 64] + '0';
    else if (input[i] >= 48 && input[i] <= 57)
      output = output + morseTable[input[i] - 21] + '0';
    else if (input[i] == 32)
      output = output + morseTable[0];
  }
  return output;
}

void pttOn() {
  if (digitalRead(PTT_PIN) == HIGH) return;

  transmitting = true;
  digitalWrite(PTT_PIN, HIGH);
  digitalWrite(DEBUG_LED, HIGH);
}

void pttOff() {
  if (digitalRead(PTT_PIN) == LOW) return;

  transmitting = false;
  digitalWrite(PTT_PIN, LOW);
  digitalWrite(DEBUG_LED, LOW);
} 

