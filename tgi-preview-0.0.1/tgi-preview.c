#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <tgi.h>
#include <tgi/tgi-kernel.h>
#include <cc65.h>



/*****************************************************************************/
/*     	      	    	  	     Data				     */
/*****************************************************************************/



/* Loaded fonts */
static const tgi_vectorfont* Goth = 0;
static const tgi_vectorfont* Litt = 0;
static const tgi_vectorfont* Scri = 0;

/* Resolution */
static unsigned XRes, YRes;
static unsigned XRes2, YRes2;



/*****************************************************************************/
/*     	      	    	    	     Code				     */
/*****************************************************************************/



static void Error (const char* Format, ...)
/* Print an error and exit */
{
    va_list ap;
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
        Error ("%s: %d", S, Error);
    }
}



static void CenterText (int Y, const char* Text)
{
    tgi_outtextxy ((XRes - tgi_gettextwidth (Text)) / 2, Y, Text);
}



static void Screen0 (void)
{
    unsigned Height;

    /* Clear the screen */
    tgi_clear ();

    /* Title */
    tgi_install_vectorfont (Scri);
    tgi_settextstyle (0x100, 0x100, TGI_TEXT_HORIZONTAL, TGI_FONT_VECTOR);
    Height = tgi_gettextheight ("1");
    CenterText (YRes2, "The following material");
    CenterText (YRes2 + Height, "is completely fictional");

    cgetc ();
}



static void Screen1 (void)
{
    unsigned Height;

    /* Clear the screen */
    tgi_clear ();

    /* Title */
    tgi_install_vectorfont (Litt);
    tgi_settextstyle (0x280, 0x280, TGI_TEXT_HORIZONTAL, TGI_FONT_VECTOR);
    Height = tgi_gettextheight ("1");
    CenterText (Height*1, "6502 C Compiler");
    CenterText (Height*2, "market share");

    /* Pie slices */
    tgi_pieslice (XRes2 - 5, 130, 80, 55, 0, 300);
    tgi_pieslice (XRes2 + 5, 135, 80, 55, 300, 360);

    /* Legend */
    tgi_settextscale (0x1C0, 0x1C0);
    tgi_line (30, 100, XRes2 - 40, 130);
    tgi_outtextxy (20, 90, "cc65");
    tgi_line (260, 100, XRes2 + 70, 145);
    tgi_outtextxy (245, 90, "Others");

    cgetc ();
}



static void Screen2 (void)
{
    typedef struct {
        unsigned        X;
        unsigned        Y;
        char            Year[5];
        char            Count[5];
    } Point;
    static const Point Numbers[] = {
        {   15,   5, "2006",  "0.5" },
        {   65,  12, "2007",  "1.2" },
        {  115,  26, "2008",  "2.6" },
        {  165,  60, "2009",  "6.0" },
        {  215,  80, "2010",  "8.0" },
        {  265, 160, "2011", "16.0" },
    };

    register const Point* L;
    register const Point* P;
    unsigned char I;
    unsigned char W;
    unsigned X, Y;

    /* Clear the screen */
    tgi_clear ();

    /* Title */
    tgi_install_vectorfont (Litt);
    tgi_settextstyle (0x180, 0x180, TGI_TEXT_HORIZONTAL, TGI_FONT_VECTOR);
    Y = tgi_gettextheight ("1");
    CenterText (Y, "Projected cc65 sales figures");
    tgi_settextscale (0x100, 0x100);
    Y += tgi_gettextheight ("1");
    CenterText (Y, "(millions of units)");

    /* Coordinate system */
    tgi_line (15, YRes - 15, XRes - 5, YRes - 15);
    tgi_line (15, YRes - 15, 15, 5);
    for (I = 0, P = Numbers; I < sizeof (Numbers) / sizeof (Numbers[0]); ++I, ++P) {

        /* X ticks */
        tgi_line (P->X, YRes - 15, P->X, YRes - 12);

        /* X label */
        tgi_settextstyle (0x100, 0x100, TGI_TEXT_HORIZONTAL, TGI_FONT_VECTOR);
        X = P->X - tgi_gettextwidth (P->Year) / 2;
        Y = YRes - 2;
        tgi_outtextxy (X, Y, P->Year);

        /* Y ticks */
        Y = YRes - 15 - P->Y;
        tgi_line (12, Y, 15, Y);

        /* Y label */
        tgi_settextstyle (0x100, 0x100, TGI_TEXT_VERTICAL, TGI_FONT_VECTOR);
        X = 8;
        W = tgi_gettextwidth (P->Count) / 2;
        /* Check for overlap with previous label */
        if (I == 0 || Y < YRes - 15 - L->Y - W) {
            tgi_outtextxy (X, Y + W, P->Count);
        }

        /* Graph */
        if (I > 0) {
            tgi_line (L->X, YRes - L->Y - 15, P->X, YRes - P->Y - 15);
        }

        /* L is last point */
        L = P;
    }

    cgetc ();
}



static void Screen3 (void)
{
    typedef struct {
        unsigned        X;
        unsigned        Val;
        char            Label[5];
        char            Desc[4];
    } Bar;
    static const Bar Numbers[] = {
        {  30, 97*256/100, "2006", "97%" },
        { 110, 94*256/100, "2007", "94%" },
        { 190, 98*256/100, "2008", "98%" },
    };


    register const Bar* B;
    unsigned char I;
    unsigned char Height;
    unsigned Y1, Y2;


    /* Clear the screen */
    tgi_clear ();

    /* Title */
    tgi_install_vectorfont (Litt);
    tgi_settextstyle (0x1C0, 0x1C0, TGI_TEXT_HORIZONTAL, TGI_FONT_VECTOR);
    CenterText (tgi_gettextheight ("1"), "Customer satisfaction index");

    /* Coordinate system */
    Y1 = YRes - 15;
    tgi_settextstyle (0x100, 0x100, TGI_TEXT_VERTICAL, TGI_FONT_VECTOR);
    tgi_line (15, Y1, XRes - 5, Y1);
    tgi_line (15, Y1, 15, 5);
    tgi_line (15, 30, 12, 30);
    tgi_line (15, YRes2, 12, YRes2);
    tgi_outtextxy (8, 30 + tgi_gettextwidth ("100%") / 2, "100%");
    tgi_outtextxy (8, YRes2 + tgi_gettextwidth ("50%") / 2, "50%");
    tgi_settextstyle (0x100, 0x100, TGI_TEXT_HORIZONTAL, TGI_FONT_VECTOR);
    Height = tgi_gettextheight ("1");
    for (I = 0, B = Numbers; I < sizeof (Numbers) / sizeof (Numbers[0]); ++I, ++B) {
        Y2 = YRes - 15 - tgi_imulround (B->Val, YRes - 45);
        tgi_line (B->X, Y1, B->X, Y2);
        tgi_lineto (B->X + 70, Y2);
        tgi_lineto (B->X + 70, Y1);
        tgi_outtextxy (B->X + 35 - tgi_gettextwidth (B->Label) / 2, YRes2, B->Label);
        tgi_outtextxy (B->X + 35 - tgi_gettextwidth (B->Desc) / 2, YRes2 + Height, B->Desc);
    }
    cgetc ();
}



static void Screen9 (void)
{
    unsigned Y;

    /* Clear the screen */
    tgi_clear ();

    /* Title */
    tgi_install_vectorfont (Goth);
    tgi_settextstyle (0x280, 0x300, TGI_TEXT_HORIZONTAL, TGI_FONT_VECTOR);
    Y = (YRes + tgi_gettextheight ("1")) / 2;
    CenterText (Y, "The End");

    cgetc ();
}



static const tgi_vectorfont* LoadFont (const char* Name)
/* Load a font, do error checking */
{   
    const tgi_vectorfont* F;

    printf ("Loading %s...\n", Name);
    F = tgi_load_vectorfont (Name);
    CheckTGIError ("tgi_load_vectorfont");
    return F;
}



int main (void)
{
    unsigned char Border;

    /* Load and initialize the driver */
    printf ("Loading driver, please wait ...\n");
    tgi_load_driver (tgi_stddrv);
    CheckTGIError ("tgi_load_driver");

    /* Load the vector fonts */
    Goth = LoadFont ("goth.tch");
    Litt = LoadFont ("litt.tch");
    Scri = LoadFont ("scri.tch");

    /* Set the palette, set the border color */
    Border = bordercolor (COLOR_BLUE);

    /* Switch to graphics mode */
    tgi_init ();
    CheckTGIError ("tgi_init");

    /* Set the drawing color */
    tgi_setcolor (COLOR_WHITE);

    /* Remember the resolution */
    XRes = tgi_getxres ();
    YRes = tgi_getyres ();
    XRes2 = XRes / 2;
    YRes2 = YRes / 2;

    /* Graphics stuff */
    Screen0 ();
    Screen1 ();
    Screen2 ();
    Screen3 ();
    Screen9 ();

    /* Unload the driver */
    tgi_unload ();

    /* Reset the border */
    (void) bordercolor (Border);

    /* Done */
    printf ("Done\n");
    return EXIT_SUCCESS;
}
