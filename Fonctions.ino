// Convertir les nombres décimaux codés binaires en décimaux normaux
byte bcdToDec(byte val) {
  return( (val/16*10) + (val%16));
}

void lire_date() {
  // Réception de l'heure et de la date
  Wire.beginTransmission(ADRESSE_I2C_RTC);
  Wire.write(0); // Positionner le pointeur de registre sur 00h
  Wire.endTransmission();
  Wire.requestFrom(ADRESSE_I2C_RTC, 7);
  // Accède aux données de l'horloge (à partir du registre 00h)
  seconde = bcdToDec(Wire.read() & 0x7f);
  minute = bcdToDec(Wire.read());
  heure = bcdToDec(Wire.read() & 0x3f);
}

void u8g_prepare(void) {
  u8g.setFont(u8g_font_6x10);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

void draw(void) {
  u8g_prepare();

  // affichage de l'heure 
    u8g.setScale2x2(); 
    u8g.setPrintPos(15, 0);
    if (heure<10) {
      u8g.print("0");
      u8g.setPrintPos(21, 0);
    }
    u8g.print(heure);
    if ( seconde % 2)
      u8g.drawStr(27, -1, ":");
    u8g.setPrintPos(33, 0);
    if (minute<10) {
      u8g.print("0");
      u8g.setPrintPos(39, 0);
    }
    u8g.print(minute);
    u8g.undoScale();

  if (cycle_run) {
    switch (page) {
      case 1:  // affichage de l'étape courante
        u8g.setScale2x2(); 
        u8g.setPrintPos(0, 10);
        u8g.print(etape_desc_1);
        u8g.setPrintPos(0, 20);
        u8g.print(etape_desc_2);
        u8g.undoScale();
        break;
      case 2:  // affichage du temps restant
        u8g.setScale2x2(); 
        u8g.drawStr(9, 10, "fin dans");
        u8g.setPrintPos(15,20);
        u8g.print(restant_cycle);
        u8g.drawStr(33,20, "min");
        u8g.undoScale();
        break;
      case 3:  // Afichage des icones
        if (blink_LED.state)
          u8g.drawBitmapP( 5, 20, 8, 44, morty);
        else
          u8g.drawBitmapP( 60, 20, 8, 44, rick);
        break;
      default:
        page = 1;
    }
  }
  else if (menu_1) {
    switch (page_menu) {
      case 1:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "  départ  ");
        u8g.drawStr(0, 20, " immediat ");
        u8g.undoScale();
        break;
      case 2:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "  départ  ");
        u8g.drawStr(0, 20, "  à  15h  ");
        u8g.undoScale();
        break;
      case 3:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "  départ  ");
        u8g.drawStr(0, 20, "  à 20h30 ");
        u8g.undoScale();
        break;
      case 4:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "  départ  ");
        u8g.drawStr(0, 20, "  à 02h30 ");
        u8g.undoScale();
        break;
      case 5:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, " réglages ");
        u8g.drawStr(0, 20, " du cycle ");
        u8g.undoScale();
        break;
      default:
        page_menu = 1;
    }
  }
  else if (menu_2) {
    switch (page_menu) {
      case 1:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "  lavage  ");
        u8g.drawStr(0, 20, "  normal  ");
        u8g.undoScale();
        break;
      case 2:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "  lavage  ");
        u8g.drawStr(0, 20, " intensif ");
        u8g.undoScale();
        break;
      case 3:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "  lavage  ");
        u8g.drawStr(0, 20, "   éco    ");
        u8g.undoScale();
        break;
      case 4:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "  rincage ");
        u8g.drawStr(0, 20, " seulement");
        u8g.undoScale();
        break;
      case 5:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "  retour  ");
        u8g.drawStr(0, 20, "  au menu ");
        u8g.undoScale();
        break;
      default:
        page_menu = 1;
    }
  }
  else if (cycle_termine) {
    switch (page) {
      case 1:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "Dish clean");
        u8g.drawStr(0, 20, " I win !  ");
        u8g.undoScale();
        break;
      case 2:
        u8g.drawBitmapP( 10, 20, 12, 42, dish_bitmap);
        break;
      default:
        page = 1;
    }
  }
  else if (cycle_annule) {
    switch (page) {
      case 1:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, " Cycle    ");
        u8g.drawStr(0, 20, " annulé ! ");
        u8g.undoScale();
        break;
      case 2:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "  Game    ");
        u8g.drawStr(0, 20, "    Over  ");
        u8g.undoScale();
        break;
      default:
        page = 1;
    }
  }
  else if (cycle_pause) {
    switch (page) {
      case 1:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, " Cycle    ");
        u8g.drawStr(0, 20, " en pause ");
        u8g.undoScale();
        break;
      case 2:
        u8g.drawBitmapP( 10, 20, 12, 42, dish_bitmap);
        break;
      default:
        page = 1;
    }
  }
  else {
    switch (page) {
      case 1:
        u8g.setScale2x2(); 
        u8g.drawStr(0, 10, "  Insert  ");
        u8g.drawStr(0, 20, "  Coin !  ");
        u8g.undoScale();
        break;
      case 2:
        u8g.drawBitmapP( 10, 20, 12, 42, insert_coin);
        break;
      default:
        page = 1;
    }
  }
}

//        u8g.drawBitmapP( 44, 20, 5, 42, dish_bitmap);

void reinit() {
  M_vidange.stop();
  EV_eau.stop();
  M_cuve.stop();
  M_cycle.stop();
  Chauffe.stop();

  current_etape = 0;
  cycle_run = 0;
  cycle_pause = 0;

  reinit_cycle();
}

void reinit_cycle() {
  vidange.reset();
  remplissage.reset();
  cycle_lavage.reset();
  cycle_rincage_court.reset();
  cycle_rincage_long.reset();
}

// Routine d'interruption, heartbeat 1Hz
ISR(TIMER1_COMPA_vect) {
  blink_LED.switche();
}
