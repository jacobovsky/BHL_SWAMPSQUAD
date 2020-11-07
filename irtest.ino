#include <IRremote.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include "remote_buttons.h"

// start button
#define START_DIODE 4 // TODO do weryfikacji
#define REMOTE 2
#define DIODE_NUMBER 3

// valves 1 - alcohol, 2 - mixer
#define VALVE_1 5
#define VALVE_2 6

#define TIME 1
#define ALCOHOL 0

#define WHOLE_DRINK_TIME 1.67
#define WHOLE_DRINK_ALCOHOL 1

#define DRINK_TIME_INCREMENT 1.67
#define DRINK_ALCOHOL_INCREMENT 0.33

// diodes
#define ALCOHOL_DIODE_1 7
#define ALCOHOL_DIODE_2 8 
#define ALCOHOL_DIODE_3 9

#define TIME_DIODE_1 10
#define TIME_DIODE_2 11
#define TIME_DIODE_3 12

// variables used to control the flow of fluid
int firstValveAlcohol, firstValveTime = 0;
int timeDiodes[3] = {TIME_DIODE_1, TIME_DIODE_2, TIME_DIODE_3};
int alcoholDiodes[3] = {ALCOHOL_DIODE_1, ALCOHOL_DIODE_2, ALCOHOL_DIODE_3};

// wpisujemy nazwę pinu, po którym nastepuje komunikacja
IRrecv irrecv(REMOTE);
decode_results signals;

volatile bool pour = false;

void setup()
{
    noInterrupts();
    Serial.begin(9600);
    pinMode(VALVE_1, OUTPUT);
    pinMode(VALVE_2, OUTPUT);
    digitalWrite(VALVE_1, HIGH);
    digitalWrite(VALVE_2, HIGH);
    pinMode(START_DIODE, OUTPUT);
    for (int i = 0; i < 3; i++) {
      pinMode(timeDiodes[i], OUTPUT);
      pinMode(alcoholDiodes[i], OUTPUT);
    }
    irrecv.enableIRIn(); // włączenie odbierania danych

    for (int i = 0; i < 2; i++){ 
      digitalWrite(START_DIODE, HIGH);
      delay(300);
      digitalWrite(START_DIODE, LOW);
    }
    interrupts();
}


void loop() {
if (irrecv.decode(&signals)) {

        switch(signals.value) {
          case  key_play:
                Serial.print("firstValveAlcohol = ");
                Serial.println(firstValveAlcohol);
                Serial.print("firstValveTime = ");
                Serial.println(firstValveTime);

                pourDrink();
              break;
          case key_vol_MINUS:
              firstValveAlcohol = max(firstValveAlcohol - 1, 0);
              zeroDiodes(alcoholDiodes);
              lightDiodes(alcoholDiodes, firstValveAlcohol);
              break;
          case key_vol_PLUS:
              firstValveAlcohol = min(firstValveAlcohol + 1, DIODE_NUMBER);
              zeroDiodes(alcoholDiodes);
              lightDiodes(alcoholDiodes, firstValveAlcohol);
              break;
          case key_prev:
              firstValveTime = max(firstValveTime - 1, 0);
              zeroDiodes(timeDiodes);
              lightDiodes(timeDiodes, firstValveTime);
              break;
          case key_next:
              firstValveTime = min(firstValveTime + 1, DIODE_NUMBER);
              zeroDiodes(timeDiodes);
              lightDiodes(timeDiodes, firstValveTime);
              break;
        }
          irrecv.resume();  
    }
}

void pourDrink() {
  digitalWrite(START_DIODE, HIGH);
  float t = WHOLE_DRINK_TIME * (firstValveTime);
  float t1 = t * ((firstValveAlcohol) * 1.0 / DIODE_NUMBER);
  float t2 = t - t1;

  digitalWrite(VALVE_1, LOW);
  delay(int(t1 * 1000));
  digitalWrite(VALVE_1, HIGH);
  delay(200);
  digitalWrite(VALVE_2, LOW);
  delay(int(t2 * 1000));
  digitalWrite(VALVE_2, HIGH);
  delay(200);

  digitalWrite(START_DIODE, LOW);
  return;
}

void zeroDiodes(int* arr) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(arr[i], LOW);
  }
}

void lightDiodes(int* arr, float value) {
  for (int i = value; i > 0; i--) {
    digitalWrite(arr[i - 1], HIGH);
  }
}
