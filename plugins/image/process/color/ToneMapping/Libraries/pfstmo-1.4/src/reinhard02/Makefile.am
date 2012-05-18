###
bin_PROGRAMS = pfstmo_reinhard02

man_MANS = pfstmo_reinhard02.1

pfstmo_reinhard02_SOURCES = pfstmo_reinhard02.cpp \
		 	    tmo_reinhard02.cpp tmo_reinhard02.h \
			    approx.cpp

LIBS = $(PFS_LIBS)
INCLUDES = $(PFS_CFLAGS) -I$(top_srcdir)/src/pfstmo/
EXTRA_DIST = $(man_MANS)

