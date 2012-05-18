###
bin_PROGRAMS = pfstmo_mantiuk08

SUBDIRS = cqp

man_MANS = pfstmo_mantiuk08.1

pfstmo_mantiuk08_SOURCES = pfstmo_mantiuk08.cpp display_adaptive_tmo.cpp display_adaptive_tmo.h \
display_function.cpp display_function.h display_size.cpp display_size.h

LIBS = $(PFS_LIBS) cqp/libcqp.a $(GSL_LIBS)
INCLUDES = $(PFS_CFLAGS) $(GSL_CFLAGS) -I$(top_srcdir)/src/pfstmo/
EXTRA_DIST = $(man_MANS)

