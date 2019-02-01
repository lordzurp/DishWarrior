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
  u8g.setFont(u8g_font_6x10r);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

void draw(void) {
  u8g_prepare();

  // affichage de l'heure 
    u8g.setScale2x2(); 
    u8g.setPrintPos( 9, 0);
    if (heure<10) {
      u8g.print("0");
      u8g.setPrintPos( 15, 0);
    }
    u8g.print(heure);
    u8g.drawStr( 21, -1, ":");
    u8g.setPrintPos( 27, 0);
    if (minute<10) {
      u8g.print("0");
      u8g.setPrintPos( 33, 0);
    }
    u8g.print(minute);
    u8g.drawStr( 39, -1, ":");
    u8g.setPrintPos( 45, 0);
    if (seconde<10) {
      u8g.print("0");
      u8g.setPrintPos( 51, 0);
    }
    u8g.print(seconde);
    u8g.undoScale();

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
      if (command_start) {
        u8g.setScale2x2(); 
        u8g.drawStr( 9, 10, "fin dans");
        u8g.setPrintPos(15,20);
        u8g.print(restant_cycle);
        u8g.drawStr(33,20, "min");
        u8g.undoScale();
      }
      else
        u8g.drawBitmapP( 44, 20, 5, 42, dish_bitmap);
      break;
    case 3:  // Afichage des icones
      if (command_start) {
        if (blink_LED.state)
          u8g.drawBitmapP( 5, 20, 8, 44, morty);
        else
          u8g.drawBitmapP( 60, 20, 8, 44, rick);
      }
      break;
    default:
      break;
  }
}

void reinit() {
  M_vidange.stop();
  EV_eau.stop();
  M_cuve.stop();
  M_cycle.stop();
  Chauffe.stop();

  current_etape = 0;
  command_start = 0;

  reinit_cycle();
}

void reinit_cycle() {
  vidange.reset();
  remplissage.reset();
  cycle_lavage.reset();
  cycle_rincage_court.reset();
  cycle_rincage_long.reset();
}
