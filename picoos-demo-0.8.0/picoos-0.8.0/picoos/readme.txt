
pico]OS readme.txt                                                 2004-02-24



                             INTRODUCTION


pico]OS is a  highly configurable  and very fast  real time operating system.
It  targets a wide  range of  architectures, from the  small 8 bit processors
with very low memory till huge architectures like 32 bit processors with lots
of memory.
Please see  the documentation  shipped  with this  package (html  help in the
directory doc/) for further information.





                    HOW TO BUILD THE PICO]OS LIBRARY


To build  the pico]OS  realtime operating  system,  you need to  execute  the
makefile in the root directory. The makefiles were tested with GNU Make 3.80.

  The makefile takes two parameters:
    PORT  = name of the port to translate (= the subdirectory name)
    BUILD = version to build, possible values are DEBUG and RELEASE

  Example:
    make PORT=x86dos BUILD=DEBUG

    Builds the x86 DOS port and includes debug informations.
    The generated library can be found in the directory lib/x86dos/deb/

  Make targets:
    The makefile knows the targets 'all', 'clean' and 'docu', and
    'all' is the default.  'all' compiles the operating system.
    'clean' removes all generated binaries. 'docu' generates the
    html help with use of the doxygen tool.

  Hint:
    When the makefile is started, it searches for the configuration file
    'config.mak' in the pico]OS root directory.  You can put in there your
    command line. Example:

     #--- start of file ---
     PORT  = x86dos
     BUILD = DEBUG
     #---- end of file ----


Please report bugs to dennis_k@freenet.de
