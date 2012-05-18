/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define as 1 when compiling in debug mode */
/* #undef DEBUG */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define this if you have the GDAL library */
/* #undef HAVE_LIBGDAL */

/* ImageMagick library used for reading/writing images. */
#define HAVE_LIBIMAGEMAGICK 1

/* Define this if you have the libjpeghdr libraries */
/* #undef HAVE_LIBJPEGHDR */

/* Define this if you have the libppm libraries */
/* #undef HAVE_LIBNETPPM */

/* OpenEXR libraries, optional, used in io routines. */
/* #undef HAVE_LIBOPENEXR */

/* OpenGL libraries are required. */
#define HAVE_LIBOPENGL 1

/* QT libraries, optional, used in pfsview. */
#define HAVE_LIBQT 1

/* Define this if you have the libtiff libraries */
#define HAVE_LIBTIFF 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Disable assertion checks */
#define NDEBUG 1

/* Name of package */
#define PACKAGE "pfstools"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "pfstools"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "pfstools 1.8.5"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "pfstools"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.8.5"

/* Directory where data files are located. */
#define PKG_DATADIR "/usr/local/share/pfstools"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.8.5"

/* Windows compilation. */
/* #undef WIN32 */

/* Output stream for debug messages. */
#ifdef DEBUG
#define DEBUG_STR cerr
#else
#define DEBUG_STR if(1); else cerr
#endif
/* Output stream for verbose messages */        
#define VERBOSE_STR if(verbose) std::cerr << PROG_NAME << ": "        


/* On some systems iostream, string etc are in 'std' namespace */
using namespace std; 
