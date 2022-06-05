#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <conio.h>
#include <cc65.h>
#include <tgi.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>

#undef DYN_DRV

#ifndef DYN_DRV
#  define DYN_DRV 0
#endif


char** argv2;

/* Dummy for substitution of either first or second argument with succeding comma.
   This way the params can be used within an enum or within a string array. */
#define PARAMS P(x_start, X_START) P(y_start,Y_START) P(r_start,R_START) P(r_min,R_MIN) 

/* Param list to define enums with each second argument of the list, separated by comma */
enum paramlist
{
  #define P(x,y) y,
  PARAMS
  PARAMCOUNT
  #undef P
};

/* Param list with stringification to define string array
   with each first argument of the list, separated by comma */
static const unsigned char* paramnames[] = 
{
  #define P(x,y) #x,
  PARAMS 
  " " 
  #undef P
};

/* Array to hold parameter values. The size of the
   array grows or diminishes with the number of params. */
static int paramarray[PARAMCOUNT];

/* Although cc65 issues a warning, -32768=0x8000 fits in integer! */
static int paramlowbounds[PARAMCOUNT] = {-32768, -32768, 4, 2};

/* Predefined values for 320x200 resolution */
static int paramupbounds[PARAMCOUNT] = {32767, 32767, 256, 256};

static unsigned int scale;

static void Error(const char* Format, ...)
{
  va_list ap;

  /* Print an error and exit */
  va_start(ap, Format);
  vfprintf(stderr, Format, ap);
  va_end(ap);
  fputc('\n', stderr);
  exit(EXIT_FAILURE);
}

static void CheckTGIError(const char* S)
{
  unsigned char ErrCode = tgi_geterror();

  if (ErrCode != TGI_ERR_OK)
  {
    /* tgi_unload() is registered in atexit() function! */
    // tgi_unload();

    Error("%s: %d", S, Error);
  }
}

/* The recursive routine for the circle fractal */
void __fastcall__ circfrac(unsigned int x, unsigned int y, unsigned int size)
{
  unsigned int size2;

  if (kbhit())
  {
#ifdef CH_STOP
    if (CH_STOP == cgetc())
#else
    if (CH_ESC == cgetc())
#endif
    {
      Error("Drawing of figures interrupted by user.");
    }
  }

  /* If initial radius is 256, then we need unsigned long to hold nominator.
     We can only draw ellipses with radii smaller than 256! By subtracting one
     and taking account of the given ranges for the command line args,
     the radii are always smaller than 256. */
  tgi_ellipse(x - 1, y - 1, (size2 = size * 256ul / scale) - 1, size - 1);

  if ((size /= 2) >= paramarray[R_MIN])
  {
    size2 /= 2;
    circfrac(x - size2, y, size);
    circfrac(x, y - size, size);
    circfrac(x + size2, y, size);
    circfrac(x, y + size, size);
  }
}

/* Outer loop for all functions which have to
   be executed for each command line argument */
void forallparams(void (*func) (uint8_t j))
{
  uint8_t i;
  
  for (i = 0; i < PARAMCOUNT; i++)
  {
    func(i);
  }
}

/* Prints out the parameters which have to be specified by the command
   line and their corresponding ranges. To be used with forallparams(). */
void PrintParamNamesAndBounds(uint8_t j)
{
  printf("%s [%d..%d]", paramnames[j], paramlowbounds[j], paramupbounds[j]);
  if (j < PARAMCOUNT - 1)
  {
    printf(",\n");
  }
}

/* Checks, if given command line args are in the ranges
   given by paramlowbounds[] and paramupbounds[]. */
void CheckParamRange(uint8_t j)
{
  long int paramvalue;

  paramvalue = strtol(argv2[j + 1], NULL, 0);

  if (ERANGE == errno)
  {
    Error("Value far off range in %s = %ld", paramnames[j], paramvalue);
  }
  if (paramlowbounds[j] > paramvalue)
  {
    Error("Low bound exceeded in %s = %ld", paramnames[j], paramvalue);
  }

  if (paramupbounds[j] < paramvalue)
  {
    Error("Upper bound exceeded in %s = %ld", paramnames[j], paramvalue);
  }

  paramarray[j] = paramvalue;
}

/* Prints out the results of the ascii to integer convertion of
   the command line arguments. To be used with forallparams(). */
void ShowInterpretedParamValues(uint8_t i)
{
  printf("%s = %d\n", paramnames[i], paramarray[i]);
}

void PrintParamPlaceholder(uint8_t i)
{
  printf(i < PARAMCOUNT - 1 ? "#, " : "#\n");
}

void InitGraphics(void)
{
#if DYN_DRV
  /* Unload the driver */
  atexit(tgi_unload);

  /* Load and initialize the driver */
  printf("Loading driver, please wait ...\n");
  tgi_load_driver(tgi_stddrv);
  CheckTGIError("tgi_load_driver");
#else
  /* Uninstall the driver */
  atexit(tgi_uninstall);

  /* Install the driver */
  tgi_install(tgi_static_stddrv);
  CheckTGIError("tgi_install");
#endif

  /* Round aspect to avoid too many bad set pixels */
  scale = (tgi_getaspectratio() + 64) & 0xff80;
  printf("Pixel Aspect: %x\n", scale);
}

void CheckR_MIN(void)
{
  unsigned int r;

  r = paramarray[R_START];
  while ((r&1) == 0)
  {
    r /= 2;
  }
  if (paramarray[R_MIN] < r)
  {
    printf("Warning: Each smaller circle has half the radius of the"
           "next bigger circles. The radius of the smallest circles"
           "can't be reached without precision loss by successive division by 2\n");
  }
}

int main(int argc, char* argv[])
{
  argv2 = argv;

  printf("\n*** Fractal with circles displaced along x/y axes ***\n"
         "Compiled with cc65 Version %d.%d.%d.%d\n\n",
         __CC65__ >> 12, (__CC65__ >> 8) & 15,
         (__CC65__ >> 4) & 15, __CC65__ & 15);

  /* This section of code prints information about how to use the program */
  if (argc != PARAMCOUNT + 1)
  {
#ifdef __CBM__
    printf("Usage-> RUN:REM ");
#elif defined(__APPLE2__) || defined(__APPLE2ENH__)
    {
      extern void _LOADADDR__[];
      printf("Usage-> CALL%d:REM ", _LOADADDR__);
    }
#else
    printf("Usage-> %s ", argv[0]);
#endif
    forallparams(PrintParamPlaceholder);
    printf("where each # is to be replaced by:\n");
    forallparams(PrintParamNamesAndBounds);

#ifdef CH_STOP
    Error("\n\nUse STOP key to interrupt recursion and exit.");
#else
    Error("\n\nUse ESC key to interrupt recursion and exit.");
#endif
  }

  forallparams(CheckParamRange);

  /* Checks for validity of command line args.
     Because argv[] is needed, forallparams(index) cannot be used */

  /* Print out the command line args again,
     to show the interpreted values and let the user compare them to the input */
  printf( "Parameters read (inclusive filename):%d\n", argc);
  forallparams(ShowInterpretedParamValues);

  putchar('\n');

  CheckR_MIN();

  InitGraphics();

  printf("Press key\n");
  cgetc();
  tgi_init();
  tgi_clear();

  /* Start recursion with initial coordinates, 
     initial sidelength and initial height */
  circfrac(paramarray[X_START], paramarray[Y_START], paramarray[R_START]);

  cgetc();

  /* Done */
  printf ("Done\n");
  return EXIT_SUCCESS;
}
