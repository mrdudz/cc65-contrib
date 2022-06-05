#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <conio.h>
#include <cc65.h>
#include <tgi.h>
#include <string.h>
#include <unistd.h>

#undef DYN_DRV

#ifndef DYN_DRV
#define DYN_DRV 0
#endif

/* 65536*Sqrt(3) */
#define SQRT3 113512u

/* Resolution */
static unsigned int XRes, YRes;

/* Length of the triangle base */
static unsigned int SideLength;

static void Error (const char* Format, ...)
{
    va_list ap;

    /* Print an error and exit */
    va_start (ap, Format);
    vfprintf (stderr, Format, ap);
    va_end (ap);
    fputc ('\n', stderr);
    exit (EXIT_FAILURE);
}

static void CheckTGIError (const char* S)
{
    unsigned char ErrCode = tgi_geterror ();

    if (ErrCode != TGI_ERR_OK) {
        tgi_unload ();
        Error ("%s: %d", S, Error);
    }
}

/* The parameters of sier are 256 times the pixel coordinates! */
void sier(unsigned long x, unsigned int y, unsigned int h, unsigned int size)
{
    /* Halve the sidelength and height of the triangle to get a 
       smaller triangle */
    unsigned int size2 = size / 2;
    unsigned int h2 = h / 2;

    /* If the current sidelength of the triangle is larger than
       1/16 of the initial length, do next recursion */
    if (size >= SideLength * 16) {
        sier (x,             y,      h2, size2);  /* Lower left triangle  */
        sier (x + size2,     y,      h2, size2);  /* Lower right triangle */
        sier (x + size2 / 2, y - h2, h2, size2);  /* Top triangle         */
    } else {
        tgi_line (x >> 8,              y >> 8,
                  (x + size) >> 8,     y >> 8);        /* Base of triangle */
        tgi_line (x >> 8,              y >> 8,
                  (x + size / 2) >> 8, (y - h) >> 8);  /* Left side        */
        tgi_line ((x + size / 2) >> 8, (y - h) >> 8,
                  (x + size) >> 8,     y >> 8);        /* Right side       */
    }
}

int main (void)
{
    clrscr ();
    printf ("*** Sierpinski Triangle ***\n"
            "Compiled with cc65 Version %d.%d\n\r",
            (__CC65__ >> 8) & 0xff, __CC65__ & 0xff);

#if DYN_DRV
    /* Load and initialize the driver */
    printf ("Loading driver, please wait ...\n");
    tgi_load_driver (tgi_stddrv);
    CheckTGIError ("tgi_load_driver");
#else
    /* Install the driver */
    tgi_install (tgi_static_stddrv);
    CheckTGIError ("tgi_install");
#endif

    printf ("Press key\n");
    cgetc ();
    
    /* Switch to graphics mode */
    tgi_init ();
    tgi_clear ();
    
    CheckTGIError ("tgi_init");

    /* Remember the resolution */
    XRes = tgi_getxres ();
    YRes = tgi_getyres ();

    SideLength = (((unsigned long) YRes) << 8) * 0x200 / SQRT3;

    /* Start recursion with initial coordinates, 
       initial sidelength and initial height */	
    sier (((XRes - SideLength) / 2) << 8, (YRes - 1) << 8, 
          (YRes - 1) << 8, SideLength << 8);

    cgetc();

#if DYN_DRV
    /* Unload the driver */
    tgi_unload ();
#else
    /* Uninstall the driver */
    tgi_uninstall ();
#endif

    /* Done */
    printf ("Done\n");
    return EXIT_SUCCESS;
}
