/*

cc65 koala viewer

Just a small example of how to load graphics into memory.
It's hereby put in the public domain, do whatever you want
with this source.

MagerValp@cling.gu.se 2002-11-29

*/


#include <cbm.h>
#include <conio.h>


unsigned char loadtoram(unsigned char lfn, unsigned char *dest, unsigned int length) {
  int l;

  while (length) {
    l = cbm_read(lfn, dest, length); /* try to read length bytes */
    if (l == -1) {
      return(1); /* something went wrong */
    } else {
      length -= l; /* decrease length by actualy amount read */
      dest += l; /* increase destination pointer by same amount */
    }
  }
  return(0); /* ok */
}


#define poke(A,X) (*(unsigned char *)A) = (X)
#define peek(A) (*(unsigned char *)A)


unsigned char *filename = "cyberwing.koa";


int main(void) {
  unsigned char dev;
  unsigned char addr[2];

  dev = peek(0x00ba); /* get current device number */

  /* open the file */
  if (cbm_open(1, dev, 2, filename)) {
    cprintf("Couldn't open %s.\n", filename);
    return(1);
  }

  /* read file load address */
  if (cbm_read(1, &addr, 2) != 2) {
    cbm_close(1);
    cprintf("Couldn't read load address.\n");
    return(1);
  }

  /* make sure load address is $4400 or $6000 */
  if (addr[0] != 0 || (addr[1] != 0x44 && addr[1] != 0x60)) {
    cbm_close(1);
    cprintf("This doesn't look like a koala picture.\n");
    return(2);
  }

  /* load bitmap data */
  if (loadtoram(1, (unsigned char *)0x2000, 8000)) {
    cbm_close(1);
    cprintf("Error while reading bitmap.\n");
    return(1);
  }

  /* load screen data */
  if (loadtoram(1, (unsigned char *)0x0400, 1000)) {
    cbm_close(1);
    clrscr();
    cprintf("Error while reading screen ram.\n");
    return(1);
  }

  /* load colour ram */
  if (loadtoram(1, (unsigned char *)0xd800, 1000)) {
    cbm_close(1);
    clrscr();
    cprintf("Error while reading colour ram.\n");
    return(1);
  }

  /* load background colour into $d021 */
  if (loadtoram(1, (unsigned char *)0xd021, 1)) {
    cbm_close(1);
    clrscr();
    cprintf("Error while reading background colour.\n");
    return(1);
  }

  /* done */
  cbm_close(1);

  poke(0xd011, 0x3b); /* enable bitmap mode */
  poke(0xd016, 0x18); /* enable multicolour */
  poke(0xd018, 0x1f); /* screen at $0400 bitmap at $2000 */
  poke(0xd020, 0x00); /* black border */

  while (!kbhit()) { ; } /* wait for key */

  poke(0xd011, 0x1b);
  poke(0xd016, 0x08);
  poke(0xd018, 0x17);
  poke(0xd020, 0x0e);
  poke(0xd021, 0x06);

  clrscr();
  poke(198,0); /* clear keyboard queue */
}
