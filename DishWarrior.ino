//#include <U8glib.h>


// Dish Warrior
//
// lordzurp - 01-2019

// Appel des bibliothèques
  #include <Wire.h>
  #include <Bounce2.h>
  #include <U8glib.h>
  #include <DishWarrior.h>
  
  #define ADRESSE_I2C_RTC 0x68
  U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST); // Fast I2C / TWI 

// Entrées
  entree pressostat   (2, "pressostat",     INPUT);
  entree therm_50     (3, "thermostat 50",  INPUT_PULLUP);
  entree therm_65     (4, "thermostat 65",  INPUT_PULLUP);
  entree start_bouton (5, "start engine",   INPUT_PULLUP);
  entree clavier_L    (A1, "bouton gauche", INPUT_PULLUP);
  entree clavier_C    (A2, "bouton central",INPUT_PULLUP);
  entree clavier_R    (A3, "bouton droit",  INPUT_PULLUP);
// sorties
  sortie EV_eau     (8,  "EV eau");
  sortie M_cuve     (9,  "Moteur cuve");            // moteur remplissage cuve
  sortie M_cycle    (10, "Moteur cycle");           // moteur cycle
  sortie Chauffe    (11, "Resistance de chauffe");  // résistance chauffe
  sortie M_vidange  (12, "Moteur vidange");         // moteur vidange
  sortie blink_LED  (13, "heartbeat_LED");          // LED intégrée
// Etapes
  etape vidange             (0,10,0,    "vidange",      "de l'eau");
  etape remplissage         (10,10,0,   "remplissage",  "de la cuve");
  etape cycle_lavage        (10,0,60,   "lavage",       "en cours");
  etape cycle_rincage_court (120,0,2,   "rincage",      "court");
  etape cycle_rincage_long  (1200,0,10, "rincage",      "long");
  etape regen               (10,0,1,    "regeneration", "en cours");

// Variables
  long previous_draw_time = 0;
  bool start = 0;
  
  int current_etape = 0;
  String etape_desc_1 = " Let's go";
  String etape_desc_2 = " washing !";

  bool command_start = 0;

  int page = 1; // la page à afficher, on swipe toutes les 5 secondes

// time variables
  byte seconde, minute, heure;
  long current_time = 0;
  long previous_heartbeat_time = 0;
  long previous_pageswipe_time = 0;

  long push_time = 0;
  long push_start_time = 0;

// calcul du temps de cycle du lavage
  long debut_cycle = 0;
  int duree_cycle = 120;
  int restant_cycle = 0;

  // icone lave vaisselle - 40x42px
  const uint8_t dish_bitmap[] U8G_PROGMEM = {
    0x3f,0xff,0xff,0xff,0xfc,0x7f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xf0,0x00,0x00,0x00,0x0f,0xf0,0xc7,0x18,0x00,0x0f,0xf1,0xef,0xbc,0x00,0x0f,0xf1,0xef,0xbc,
    0x00,0x0f,0xf0,0xc7,0x18,0x00,0x0f,0xf0,0x00,0x00,0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xfd,0xfd,0xff,0xff,0xff,0xfc,0xfc,0xff,0xff,0xff,0xfc,0x7c,0x7f,0xff,0xff,0xfc,0x7c,0x3f,
    0xff,0xff,0xfc,0x7c,0x3f,0xff,0xff,0xfc,0x7c,0x3f,0xff,0xff,0xfc,0x7c,0x3f,0xf0,0x00,0x3c,0x7c,
    0x3f,0xf0,0x00,0x3c,0x7c,0x3f,0xf0,0x00,0x3c,0x7c,0x3f,0xf0,0x00,0x3c,0x7c,0x3f,0xf8,0x00,0x7c,
    0x7c,0x3f,0xf8,0x00,0x7c,0x7c,0x3f,0xfc,0x00,0xfc,0x7c,0x3f,0xfe,0x01,0xfc,0x7c,0x3f,0xff,0xcf,
    0xfc,0x7c,0x3f,0xff,0xcf,0xfc,0x7c,0x3f,0xff,0xcf,0xfc,0x7c,0x3f,0xff,0xcf,0xfc,0x7c,0x7f,0xff,
    0x87,0xfc,0xfc,0xff,0xff,0x03,0xfd,0xfd,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0x7f,0xff,0xff,0xff,0xfe,0x3f,0xff,0xff,0xff,0xfc,0x1e,0x00,0x00,0x00,
    0x78
  };

  // Rick, 64x44px
  const uint8_t rick[] U8G_PROGMEM = {
    0x0,0x0,0x3,0xc0,0x0,0x8,0x0,0x0,0x0,0x0,0x3,0xe0,0x0,0x38,0x0,0x0,0x0,0x0,0x3,0xf0,0x0,0x78,0x0,0x0,0x0,0x0,0x3,0xf8,0x0,0xf8,0x0,0x0,0x0,0x0,0x3,0xfc,0x3,0xf8,0x0,0x0,0x0,0x0,0x7,0xfe,0x7,0xf8,0x0,0x0,0x0,0x0,0x7,0xff,0xf,0xf8,0x0,0x0,0x1,0xe0,0x7,0xff,0xff,0xf8,0x3,0xc0,
    0x1,0xfe,0x7,0xff,0xff,0xf8,0x7,0xc0,0x1,0xff,0xff,0xff,0xff,0xfc,0x1f,0xc0,0x0,0xff,0xff,0xff,0xff,0xfc,0xff,0x80,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0x80,0x0,0x7f,0xff,0xff,0xff,0xff,0xff,0x80,0x0,0x3f,0xff,0x80,0x1,0xff,0xff,0x0,0x0,0x1f,0xff,0x0,0x0,0xff,0xff,0x0,0x0,0x1f,0xfe,0x0,0x0,0x7f,0xfe,0x0,
    0x0,0x7,0xfc,0x0,0x0,0x3f,0xfc,0x0,0x0,0x7,0xf8,0x0,0x0,0x1f,0xf8,0x0,0x7,0xff,0xf7,0xf8,0x1f,0xef,0xff,0xe0,0x7,0xff,0xf4,0x1f,0xf0,0x2f,0xff,0xc0,0x3,0xff,0xf7,0xc0,0x3,0xef,0xff,0xc0,0x1,0xff,0xf0,0x3f,0xfc,0xf,0xff,0x80,0x0,0x7f,0xf3,0xfc,0x3f,0xcf,0xfe,0x0,0x0,0x3f,0xf6,0x6,0x60,0x6f,0xf8,0x0,
    0x0,0xf,0xf7,0xfe,0x7f,0xef,0xe0,0x0,0x0,0x1f,0xf4,0x62,0x46,0x2f,0xf0,0x0,0x0,0x3f,0xf4,0x2,0x40,0x2f,0xf8,0x0,0x0,0x3f,0xf4,0x2,0x40,0x2f,0xfe,0x0,0x0,0x7f,0x93,0xfc,0x3f,0xc9,0xfe,0x0,0x0,0xf,0x14,0x0,0x0,0x28,0xff,0x0,0x0,0x7,0x93,0xf2,0x47,0xc9,0xf0,0x0,0x0,0xf,0xd0,0x2,0x40,0xb,0xf0,0x0,
    0x0,0x1f,0xf0,0x2,0x40,0xf,0xfc,0x0,0x0,0x3f,0xf0,0x2,0x40,0xf,0xfc,0x0,0x0,0x7f,0xf1,0x1,0x80,0x8f,0xfc,0x0,0x0,0x3,0xf2,0x0,0x0,0x4f,0xc0,0x0,0x0,0x3,0xfa,0xff,0xff,0x5f,0xc0,0x0,0x0,0x3,0xea,0x2,0x8,0x57,0xc0,0x0,0x0,0x3,0x89,0x1,0x98,0x91,0xc0,0x0,0x0,0x0,0xc,0x0,0xd0,0x30,0x0,0x0,
    0x0,0x0,0x6,0x0,0x50,0xe0,0x0,0x0,0x0,0x0,0x3,0x80,0x63,0x80,0x0,0x0,0x0,0x0,0x0,0xe0,0xe,0x0,0x0,0x0,0x0,0x0,0x0,0x3f,0xf8,0x0,0x0,0x0
  };

  // Morty, 64x44px
  const uint8_t morty[] U8G_PROGMEM = {
    0x00, 0x00, 0x03, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xfc, 0x00, 0x00, 0x00, 
    0x00, 0x01, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 
    0x00, 0x0f, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 
    0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 
    0x00, 0x7f, 0xff, 0xfe, 0x00, 0x3f, 0x80, 0x00, 0x00, 0xff, 0xff, 0xfe, 0x00, 0x3f, 0x80, 0x00, 
    0x00, 0xff, 0xff, 0xf0, 0x00, 0x03, 0x00, 0x00, 0x03, 0xff, 0xff, 0x00, 0x00, 0x31, 0x80, 0x00, 
    0x03, 0xff, 0xfe, 0x00, 0x60, 0x1e, 0x40, 0x00, 0x07, 0xff, 0xfc, 0x00, 0xc0, 0x01, 0xfe, 0x00, 
    0x07, 0xff, 0xf8, 0x01, 0x80, 0x03, 0x01, 0x00, 0x0f, 0xff, 0xe0, 0x1f, 0x00, 0x06, 0x00, 0x80, 
    0x1f, 0xff, 0xc0, 0x07, 0xf8, 0x0c, 0x00, 0x80, 0x1f, 0xff, 0x80, 0x1c, 0x0c, 0x08, 0x06, 0xc0, 
    0x1f, 0xfe, 0x00, 0x30, 0x06, 0x08, 0x06, 0x40, 0x3f, 0xfe, 0x00, 0x60, 0x03, 0x08, 0x00, 0x40, 
    0x3f, 0xfc, 0x00, 0x40, 0x01, 0x08, 0x00, 0x40, 0x3f, 0xfc, 0x00, 0x40, 0x01, 0x08, 0x00, 0x40, 
    0x3f, 0xfc, 0x00, 0x40, 0x01, 0x04, 0x00, 0x80, 0x3f, 0xfc, 0x00, 0x40, 0x01, 0x06, 0x00, 0x80, 
    0x3f, 0xfc, 0x00, 0x46, 0x01, 0x01, 0x83, 0x80, 0x3f, 0xfc, 0x00, 0x46, 0x01, 0x00, 0x7c, 0x80, 
    0x3f, 0xfc, 0x00, 0x20, 0x02, 0x00, 0x00, 0x80, 0x1f, 0xfc, 0x00, 0x10, 0x04, 0x01, 0x80, 0x80, 
    0x1f, 0xfc, 0x00, 0x08, 0x08, 0x00, 0x40, 0x80, 0x1f, 0xfc, 0x00, 0x07, 0xf0, 0x0f, 0x80, 0x80, 
    0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 
    0x07, 0xfd, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0xff, 0x00, 0x00, 0x00, 0x06, 0x00, 0x80, 
    0x00, 0xfe, 0x00, 0x00, 0x00, 0x01, 0x81, 0x80, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x07, 0x83, 0x00, 
    0x00, 0x3e, 0x00, 0x00, 0x00, 0x36, 0x02, 0x00, 0x00, 0x0f, 0x82, 0x00, 0x00, 0x34, 0x06, 0x00, 
    0x00, 0x01, 0x8c, 0x00, 0x00, 0x1c, 0x08, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x30, 0x00, 
    0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x40, 0x00, 
    0x00, 0x00, 0x0e, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x07, 0x00, 0x00
  };


void setup() {
  Wire.begin();
  //Serial.begin(9600);

  // init carte relais
  reinit();
}

void loop() {
  // on init le timestamp initial de la boucle
  current_time = millis();

  start_bouton.update();
  pressostat.update();
  therm_65.update();
  therm_50.update();

  if  (start_bouton.state && !start) {
    start = 1;
    push_start_time = current_time;
  }

  if (!start_bouton.state && start) {
    push_time = current_time - push_start_time;
    if (push_time > 50) {
      start = 0;
      if ( (push_time > 100 && push_time < 1000) && command_start) {
        reinit();
        etape_desc_1 = "cycle";
        etape_desc_2 = "annule !";
      }
      if ((push_time > 1000 && push_time < 5000) && !command_start) {
        command_start = 1;
        current_etape = 0;
      }
    }
  }

  if (command_start) {
    switch (current_etape) {
      case 0:
        reinit_cycle();
        debut_cycle = millis();
        current_etape++;
        page = 1;
        previous_pageswipe_time = previous_pageswipe_time + 6000;
        break;
      case 1: // on commence par vidanger le LV
        vidange.run();
        if (vidange.running) {
          M_vidange.start();
          if ( pressostat.state )
            vidange.done = 1;
        }
        else {
          M_vidange.stop();
        }
        break;
      case 2: // on remplit le LV
        remplissage.run();
        if (remplissage.running) {
          if (pressostat.state) {
            EV_eau.start();
          } 
          else {
            remplissage.done = 1;
          }
        }
        else {
          EV_eau.stop();
        }
        break;
      case 3: // on lance le cycle de lavage
        cycle_lavage.run();
        if (cycle_lavage.running) {
          M_cycle.start();
          if (!therm_65.state)
            Chauffe.start();
          else
            Chauffe.stop();
        }
        else {
          Chauffe.stop();
          M_cycle.stop();
        }
        break;
      case 4: // on vidange l'eau sale
        vidange.run();
        if (vidange.running) {
          M_vidange.start();
          if (pressostat.state)
            vidange.done = 1;
        }
        else {
          M_vidange.stop();
        }
        break;
      case 5: // on remplit le LV
        remplissage.run();
        if (remplissage.running) {
          if (pressostat.state) {
            EV_eau.start();
          } 
          else {
            remplissage.done = 1;
          }
        }
        else {
          EV_eau.stop();
        }
        break;
      case 6: // on rince 1 fois
        cycle_rincage_court.run();
        if (cycle_rincage_court.running) {
          M_cycle.start();
          if (!therm_50.state)
            Chauffe.start();
          else
            Chauffe.stop();
        }
        else {
          Chauffe.stop();
          M_cycle.stop();
        }
        break;
      case 7: // on vidange encore
        vidange.run();
        if (vidange.running) {
          M_vidange.start();
          if (pressostat.state)
            vidange.done = 1;
        }
        else {
          M_vidange.stop();
        }
        break;
      case 8: // on remplit le LV
        remplissage.run();
        if (remplissage.running) {
          if (pressostat.state) {
            EV_eau.start();
          } 
          else {
            remplissage.done = 1;
          }
        }
        else {
          EV_eau.stop();
        }
        break;
      case 9: // on rince 2 fois
        cycle_rincage_long.run();
        if (cycle_rincage_long.running) {
          M_cycle.start();
          if (!therm_50.state)
            Chauffe.start();
          else
            Chauffe.stop();
        }
        else {
          Chauffe.stop();
          M_cycle.stop();
        }
        break;
      case 10: // on vidange encore
        vidange.run();
        if (vidange.running) {
          M_vidange.start();
          if (pressostat.state)
            vidange.done = 1;
        }
        else {
          M_vidange.stop();
        }
        break;
      case 11: // on remplit le LV
        remplissage.run();
        if (remplissage.running) {
          if (pressostat.state) {
            EV_eau.start();
          } 
          else {
            remplissage.done = 1;
          }
        }
        else {
          EV_eau.stop();
        }
        break;
      case 12: // on rince 3 fois
        cycle_rincage_long.run();
        if (cycle_rincage_long.running) {
          M_cycle.start();
          if (!therm_50.state)
            Chauffe.start();
          else
            Chauffe.stop();
        }
        else {
          Chauffe.stop();
          M_cycle.stop();
        }
        break;
      case 13: // on vidange encore
        vidange.run();
        if (vidange.running) {
          M_vidange.start();
          if (pressostat.state)
            vidange.done = 1;
        }
        else {
          M_vidange.stop();
        }
        break;
      case 14: // on régenere la résine
        regen.run();
        if (regen.running) {
          M_cuve.start();
          M_vidange.start();
        }
        else {
          M_cuve.stop();
          M_vidange.stop();
        }
        break;
      case 15: // sechage
        etape_desc_1 = "Sechage";
        etape_desc_2 = "en cours";
        if (restant_cycle <= 0)
          current_etape++;
        break;
      default: // si on sort des clous, on reset tout
        reinit();
        etape_desc_1 = "Dish clean";
        etape_desc_2 = "I win !";
        page = 1;
        previous_pageswipe_time = previous_pageswipe_time + 6000;
        break;
    }
    restant_cycle = duree_cycle - ((current_time - debut_cycle) / 1000 / 60);
  }

  if (current_time - previous_draw_time >= 200) {
    lire_date();
    u8g.firstPage();
    do {
      draw();
    } while( u8g.nextPage() );
    previous_draw_time = millis();
  }

  if (current_time - previous_heartbeat_time >= 500) {
    previous_heartbeat_time = millis();
    blink_LED.switche();
  }

  if (current_time - previous_pageswipe_time >= 5000) {
    previous_pageswipe_time = millis();
    page++;
    if (!command_start) {
      if (page >= 3) page = 1;
    }
    else {
      if (page >= 4) page = 1;      
    }
  }

}


