###
bin_PROGRAMS = pfstmo_fattal02

man_MANS = pfstmo_fattal02.1

pfstmo_fattal02_SOURCES = pfstmo_fattal02.cpp \
			  tmo_fattal02.cpp tmo_fattal02.h \
			  pde.h pde.cpp

LIBS = $(PFS_LIBS)
INCLUDES = $(PFS_CFLAGS) -I$(top_srcdir)/src/pfstmo/
EXTRA_DIST = $(man_MANS)

