#include <Arduino.h>
#include <DishWarrior.h>

entree::entree(int pin, String desc, uint8_t mode_input)
  {
    input_pin = pin;
    pinMode(input_pin, mode_input);
    description = desc;
  }
  void entree::update() {
    state = !digitalRead(input_pin);
  }

sortie::sortie(int pin, String desc)
  {
    output_pin = pin;
    pinMode(output_pin, OUTPUT);
    digitalWrite(output_pin, HIGH);
    description = desc;
  }
  void sortie::start() {
    digitalWrite(output_pin, LOW);
    state = 1;
  }
  void sortie::stop() {
    digitalWrite(output_pin, HIGH);
    state = 0;
  }
  void sortie::switche() {
    if (state) {
      digitalWrite(output_pin, HIGH);
      state = 0;
    }
    else {
      digitalWrite(output_pin, LOW);
      state = 1;
    }
  }

etape::etape(long atempo_on = 0, long atempo_off = 0, int aduree = 0, String adesc1 = "", String adesc2 = "")
  {
    tempo_on = atempo_on;
    tempo_off = atempo_off;
    duree = aduree;
    desc1 = adesc1;
    desc2 = adesc2;
  }
  void etape::run() {
    if (!running) {
      if (!start_tempo_on) {
        start_time = current_time;
        start_tempo_on = 1;
      }
      if ((current_time - start_time) >= (tempo_on *1000) ) {
        start_time = current_time;
        running = 1;
        start_tempo_on = 0;
      }
    }
    else {
      if ( ( (duree >= 1) && ( (current_time - start_time) >= (duree *60000) ) ) || done) {
        if (!start_tempo_off) {
          stop_time = current_time;
          start_tempo_off = 1;
        }
        if ( ( (current_time - stop_time) >= (tempo_off *1000) ) && start_tempo_off ) {
          running = 0;
          done = 0;
          start_tempo_off = 0;
          current_etape++;
        }
      }
    }
    etape_desc_1 = desc1;
    etape_desc_2 = desc2;
  }
  void etape::reset() {
    running = 0;
    done = 0;
    start_tempo_on = 0;
    start_tempo_off = 0;
  }
