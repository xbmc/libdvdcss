
lib_LTLIBRARIES = libdvdcss.la
libdvdcss_la_SOURCES = libdvdcss.c css.c ioctl.c @EXTRA_SRC@
libdvdcss_la_LIBADD = @EXTRA_LIB@
libdvdcss_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -release $(LT_RELEASE)

pkgincludedir = $(includedir)/videolan
pkginclude_HEADERS = dvdcss.h

