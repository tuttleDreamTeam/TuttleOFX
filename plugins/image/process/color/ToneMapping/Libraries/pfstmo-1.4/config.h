/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Compilation for debugging purposes. */
/* #undef DEBUG */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* FFTW3F library is required for fast bilateral filtering. */
/* #undef HAVE_FFTW3F */

/* GSL library is required for mantiuk08 operator. */
/* #undef HAVE_GSL */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

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

/* Disable assertion checks */
#define NDEBUG 1

/* Name of package */
#define PACKAGE "pfstmo"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "rafm@users.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "pfstmo"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "pfstmo 1.4"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "pfstmo"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.4"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.4"


/* Output stream for debug messages. */
#ifdef DEBUG
#define DEBUG_STR std::cerr
#else
#define DEBUG_STR if(1); else std::cerr
#endif 

/* Output stream for verbose mode messages. */
#define VERBOSE_STR if( verbose ) std::cerr << PROG_NAME << ": "
extern bool verbose; /* verbose should be declared for each standalone code */

#ifdef BRANCH_PREDICTION
#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)
#else
#define likely(x)	(x)
#define unlikely(x)	(x)
#endif

