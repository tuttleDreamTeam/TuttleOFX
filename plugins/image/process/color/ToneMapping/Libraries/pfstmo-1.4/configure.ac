dnl
dnl Initialization part
dnl
AC_INIT([pfstmo], [1.4], [rafm@users.sourceforge.net])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)
AC_LANG(C++)			# perform tests in C++ language
AC_PROG_CXX
AC_PROG_CC        
AC_PROG_LIBTOOL


dnl
dnl Default setup
dnl
#CXXFLAGS="-O3 -funroll-loops -fstrength-reduce -fschedule-insns2 -felide-constructors -frerun-loop-opt -fexceptions -fno-strict-aliasing -fexpensive-optimizations -ffast-math -pipe"
enable_fftw3f="yes"


dnl
dnl Templates for autoheader defines
dnl
AH_TEMPLATE([DEBUG],
	    [Define as 1 when compiling in debug mode ])


dnl
dnl Commandline switches
dnl
AC_ARG_ENABLE(portable,
		AC_HELP_STRING([--disable-portable],
				 [Disable portable build, allow architecture-specific 
                                  optimizations (default: enable)]),
                [enable_portable=$enableval],enable_portable=yes)

AC_ARG_ENABLE(debug,
		AC_HELP_STRING([--enable-debug],
				 [Enable debug symbols and asserts]),
		 [if test "$enable_debug" = "yes" ; then
                     temp_CXXFLAGS=`echo $CXXFLAGS | sed "s/-O./ /; s/-g//"`
		     CXXFLAGS="-g $temp_CXXFLAGS"
		     AC_DEFINE([DEBUG])
		  fi],
                  [enable_debug="no"
                     temp_CXXFLAGS=`echo $CXXFLAGS | sed "s/-O./ /; s/-g//"`
   	             CXXFLAGS="-O3 $temp_CXXFLAGS"
                     AC_DEFINE(NDEBUG, 1, [Disable assertion checks])


                     AC_MSG_CHECKING([for $CC option to support OpenMP])
                     temp_CXXFLAGS="$CXXFLAGS"
                     CXXFLAGS="$CXXFLAGS -fopenmp"
                     AC_COMPILE_IFELSE([
#ifndef _OPENMP
 #error "Not supported"
#endif
                       ], [AC_MSG_RESULT([yes])], [CXXFLAGS="$temp_CXXFLAGS"; AC_MSG_RESULT([no])])

                     AC_MSG_CHECKING([for $CC optimization options])
                     temp_CXXFLAGS="$CXXFLAGS"
                     CXXFLAGS="$CXXFLAGS -ffast-math -ftree-vectorize -mfpmath=sse,387"
                     AC_COMPILE_IFELSE([
/* nothing */
                       ], [AC_MSG_RESULT([yes])], [CXXFLAGS="$temp_CXXFLAGS"; AC_MSG_RESULT([no])])


                     AC_MSG_CHECKING([for $CC branch prediction])
                     temp_CXXFLAGS="$CXXFLAGS"
                     CXXFLAGS="$CXXFLAGS -DBRANCH_PREDICTION"
                     AC_COMPILE_IFELSE([
main() { int x = 0; if( __builtin_expect((x),0) ) x = 1; }
                       ], [AC_MSG_RESULT([yes])], [CXXFLAGS="$temp_CXXFLAGS"; AC_MSG_RESULT([no])])
                     ])

dnl
dnl System detection and configuration
dnl

AX_GCC_ARCHFLAG($enable_portable, [CXXFLAGS="$CXXFLAGS $ax_cv_gcc_archflag"])

dnl PFS tools library - base for input sources
AC_MSG_CHECKING([for pfs library])
PKG_CHECK_MODULES(PFS, pfs >= 1.0)
AC_SUBST(PFS_CFLAGS)
AC_SUBST(PFS_LIBS)

dnl FFTW - library for computing the Discrete Fourier Transform (DFT)
AC_MSG_CHECKING([for fftw3f library])
PKG_CHECK_MODULES(FFTW3F, fftw3f >= 3.0, 
	[AC_DEFINE( HAVE_FFTW3F, 1, [FFTW3F library is required for fast bilateral filtering.])
	enable_fftw3f="yes"],[
	enable_fftw3f="no"])
AC_SUBST(FFTW3F_LIBS)
AC_SUBST(FFTW3F_CFLAGS)
#enable_fftw3f="no"

dnl GSL - math library, required for mantiuk08 operator
AC_MSG_CHECKING([for GSL library])
PKG_CHECK_MODULES(GSL, gsl, 
	[AC_DEFINE( HAVE_GSL, 1, [GSL library is required for mantiuk08 operator.])
	enable_gsl="yes"],[
	enable_gsl="no"])
AC_SUBST(GSL_LIBS)
AC_SUBST(GSL_CFLAGS)

dnl Debug version
if test "$enable_debug" = "yes" ; then
  AC_DEFINE([DEBUG], 1, [Compilation for debugging purposes.])
fi


dnl
dnl Autoheader defines
dnl
AH_BOTTOM([
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
])


dnl
dnl Decide, what is supported
dnl
AM_CONDITIONAL(SUP_FFTW3F, test "$enable_fftw3f" = "yes" )
AM_CONDITIONAL(SUP_GSL, test "$enable_gsl" = "yes" )


dnl
dnl Finish
dnl
AC_CONFIG_FILES([ Makefile ])
AC_CONFIG_FILES([ src/Makefile ])
AC_CONFIG_FILES([ src/durand02/Makefile ])
AC_CONFIG_FILES([ src/drago03/Makefile ])
AC_CONFIG_FILES([ src/fattal02/Makefile ])
AC_CONFIG_FILES([ src/mantiuk06/Makefile ])
AC_CONFIG_FILES([ src/reinhard02/Makefile ])
AC_CONFIG_FILES([ src/reinhard05/Makefile ])
AC_CONFIG_FILES([ src/pattanaik00/Makefile ])
AC_CONFIG_FILES([ src/mantiuk08/Makefile ])
AC_CONFIG_FILES([ src/mantiuk08/cqp/Makefile ])
AC_OUTPUT

AC_MSG_RESULT([
PFStmo is now configured

  Source directory:     $srcdir
  Installation prefix:  $prefix
  C++ compiler:         $CXX $XTRA_CXXFLAGS $WARN_CXXFLAGS $CXXFLAGS
  LIBS:                 $LIBS

  fftw3f:               $enable_fftw3f
  LIBS:                 $FFTW3F_LIBS

  GSL:                  $enable_gsl])
if test "x$enable_gsl" = "xyes" ; then
AC_MSG_RESULT([    LIBS:                 $GSL_LIBS
    CFLAGS:               $GSL_CFLAGS])
else
AC_MSG_RESULT([      The following command(s) WILL NOT be compiled: pfstmo_mantiuk08])
fi  
AC_MSG_RESULT([
  Debug mode:           $enable_debug
])
