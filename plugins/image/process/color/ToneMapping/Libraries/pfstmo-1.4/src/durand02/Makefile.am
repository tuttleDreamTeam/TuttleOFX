###
bin_PROGRAMS = pfstmo_durand02

man_MANS = pfstmo_durand02.1

if SUP_FFTW3F
FAST_BILATERAL = fastbilateral.cpp fastbilateral.h
else
FAST_BILATERAL =
endif

pfstmo_durand02_SOURCES = pfstmo_durand02.cpp \
			  tmo_durand02.cpp tmo_durand02.h \
			  bilateral.cpp bilateral.h ${FAST_BILATERAL}

LIBS = $(PFS_LIBS) $(FFTW3F_LIBS)
INCLUDES = $(PFS_CFLAGS) $(FFTW3F_CFLAGS) -I$(top_srcdir)/src/pfstmo/
EXTRA_DIST = $(man_MANS)
