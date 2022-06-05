// Programm: Matrix Bildschirmschoner
// Autor: Markus Stehr aka. BastetFurry (c)2005
// Lizenz: GPL
//
// Sinn des Programms: Darstellung des aus Matrix bekannten Bildschirmcodes

// Die üblichen verdächtigen
#include <stdlib.h>
#include <conio.h>

// Variablendeklaration
unsigned char stelle;       // Beinhaltet die Spalte die zZ bearbeitet wird
unsigned char laenge[80];   // Array der Längen der Buchstabenschlangen
unsigned char ende[80];     // Array der letzten Zeile der Buchstabenschlange
unsigned char done;         // Sind wir fertig? Wird in diesem Programm nicht erreicht
unsigned char XSize, YSize; // Hier kommen die Bildschirmgrößen rein

// Diese Routine erstellt eine Buchstabenschlange neu
void zufallslinie()
{
  laenge[stelle]=(rand()%20)+1;
	ende[stelle]=-1;
}

// Diese Routine manipuliert den Farbspeicher
// Schick wenn man den gesamten Speicher wie ein Array behandeln kann *g*
#define farbesetzen(fs_x,fs_y,fs_farbe) COLOR_RAM[(fs_y-1)*XSize+fs_x] = fs_farbe

/*void farbesetzen(char fs_x,char fs_y, char fs_farbe)
{
  COLOR_RAM[(fs_y-1)*40+fs_x] = fs_farbe;
}
*/

int main(void)
{
  screensize (&XSize, &YSize);  // Bildschirmgröße ermitteln
	_randomize();  // Zufallszahlen setzen
  // Bildschirm vorbereiten
  textcolor (COLOR_WHITE);
  bordercolor (COLOR_BLACK);
  bgcolor (COLOR_BLACK);
  cursor (1);
	clrscr();      // Bildschirm löschen

  // Infotext
  cputs("MATRIX-Demo by BastetFurry\r\n");
  cputs("nachtkatzen productions (c)2005\r\n");
  cputs("----------------------------------------");
  cputs("Dieses Programm darf frei kopiert werden\r\n\r\n");
  cputs("Auf die Idee kam ich durch einen Contest");
  cputs("auf der 2004er Hobbytronic.\r\n");
  cputs("Ziel war es mit CBM-BASIC *grusel* einen");
  cputs("Matrix Bildschirmschoner zu schreiben.\r\n");
  cputs("Ich schafte es auch, aber das Proggy\r\n");
  cputs("war langsam, und selbst ein Basic-\r\n");
  cputs("compiler konnte da nicht viel retten.\r\n");
  cputs("Nun bin ich auf CC65 gestossen und mein\r\n");
  cputs("Erstlingswerk liegt in eurer Floppy.\r\n");
  cputs("Ich hoffe ich als verwoehnter PC-Coder\r\n");
  cputs("bringe euch hiermit etwas Spass\r\n");
  cputs("auf eurem Bildschirm ;)\r\n");
  cputs("Vielleicht kommt ja nochmal was\r\n");
  cputs("groesseres von mir!\r\n\r\n");
  cputs("Bitte Programm \"Entern\" !");
  cgetc(); // Warte auf Enter
#ifdef __C128__
  if(XSize==80) textcolor (COLOR_GREEN);
#endif
  clrscr();

  for(stelle=0;stelle<XSize;++stelle)
  {
    zufallslinie();
  }

  while(!done)
  {
    for(stelle=0;stelle<XSize;++stelle)
    {
      ++ende[stelle];
      if(ende[stelle]>YSize-1)ende[stelle]=YSize-1;
      if(ende[stelle]>0)
      {
      	farbesetzen(stelle,ende[stelle]-1,COLOR_GREEN);
      	cputcxy(stelle,ende[stelle],(rand()%128)+' ');
      }
    }

    for(stelle=0;stelle<XSize;++stelle)
    {
      if(ende[stelle]==YSize-1)laenge[stelle]--;
//      farbesetzen(stelle,ende[stelle]-laenge[stelle]-1,COLOR_LIGHTGREEN);
      cputcxy(stelle,ende[stelle]-laenge[stelle]-1,' ');
      cputcxy(stelle,ende[stelle]-laenge[stelle],' ');

      if(laenge[stelle]==1)
      {
      	cputcxy(stelle,ende[stelle],' ');
        zufallslinie();
      }
    }


  } // Ende while(!done)

} // Ende main()
