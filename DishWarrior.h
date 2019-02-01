#include <Arduino.h>
#define DishWarrior.h

extern int current_etape;
extern long current_time;
extern String etape_desc_1;
extern String etape_desc_2;

class entree {
  public:
    entree(int pin, String desc, uint8_t mode_input);
    void update();
    bool state;
    String description;
  private:
  	int input_pin;
};

class sortie {
  public:
    sortie(int pin, String desc);
    void start();
    void stop();
    void switche();
    bool state;
    String description;
  private:
  	int output_pin;
};

class etape {
  public:
    etape(long atempo_on, long atempo_off,int aduree, String adesc1, String adesc2);
    void run();
    void reset();
    String desc1;
    String desc2;
    bool running;
    bool done;
    int duree;
  private:
    long stop_time = 0;
    long start_time = 0;
    long tempo_on;
    long tempo_off;
    bool start_tempo_on = 0;
    bool start_tempo_off = 0;
};
