/*
   ______               _                  _///_ _           _                   _
  /   _  \             (_)                |  ___| |         | |                 (_)
  |  [_|  |__  ___  ___ _  ___  _ __      | |__ | | ___  ___| |_ _ __ ___  _ __  _  ___  _   _  ___
  |   ___/ _ \| __|| __| |/ _ \| '_ \_____|  __|| |/ _ \/  _|  _| '__/   \| '_ \| |/   \| | | |/ _ \
  |  |  | ( ) |__ ||__ | | ( ) | | | |____| |__ | |  __/| (_| |_| | | (_) | | | | | (_) | |_| |  __/
  \__|   \__,_|___||___|_|\___/|_| [_|    \____/|_|\___|\____\__\_|  \___/|_| |_|_|\__  |\__,_|\___|
                                                                                      | |
                                                                                      \_|
  Fichier: LectureCompleteMemoireEepromATmega328P.ino
  Description: Affiche l'intégralité de la mémoire EEPROM d'un microcontrôleur ATmega328P sur le moniteur série
  Auteur: Jérôme TOMSKI (https://passionelectronique.fr/)

  Créé le 28.05.2021
*/
#include <EEPROM.h>

void afficheNombreDecimalAvecEspaces(int valeur, byte nbreDeChiffres) {
  switch(nbreDeChiffres) {
    case 2:
      if(valeur<10) Serial.print(" ");
      break;
    case 3:
      if(valeur<10) Serial.print(" ");
      if(valeur<100) Serial.print(" ");
      break;
    case 4:
      if(valeur<10) Serial.print(" ");
      if(valeur<100) Serial.print(" ");
      if(valeur<1000) Serial.print(" ");
      break;
    default:
      break;
  }
  Serial.print(valeur, DEC);
}

void setup() {
  Serial.begin(9600);

  Serial.println("=======================================================================");
  Serial.println("    TABLEAU affichant les valeurs contenues dans la mémoire EEPROM     ");
  Serial.println("     d'un microcontrôleur ATmega328 (Arduino Uno, Nano, Pro mini)      ");
  Serial.println("=======================================================================");
  Serial.println("");
  Serial.println("Nota : toutes les valeurs sont affichées en décimal (0..255)");
  Serial.println("");

  byte lignes, colonnes;

  // Affichage de l'entête (numéros de colonnes, de 0 à 15)
  Serial.print("    \t");
  for(colonnes = 0; colonnes < 16 ; colonnes++) {
    afficheNombreDecimalAvecEspaces(colonnes, 3);
    Serial.print(" ");
  }
    Serial.println("");
  
  // Affichage du tableau en lui-même (avec adresse de la 1ère case mémoire, à chaque retour à la ligne)
  for(lignes = 0; lignes < 64 ; lignes++) {
    afficheNombreDecimalAvecEspaces(lignes*16, 4);
    Serial.print("\t");
    for(colonnes = 0; colonnes < 16 ; colonnes++) {
      afficheNombreDecimalAvecEspaces(EEPROM.read(colonnes+lignes*16), 3);
      Serial.print(" ");
    }
    Serial.println("");
  }

  // Affichage en dessous du tableau
  Serial.println("");
  Serial.print("Total = "); Serial.print(lignes*16); Serial.println(" cases mémoire lues");
}

void loop() {
}