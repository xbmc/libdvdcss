ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = libdvdcss.spec doc/header.html doc/footer.html doc/latex/ INSTALL

dist_doc_DATA = AUTHORS COPYING NEWS README ChangeLog

DISTCLEANFILES = ChangeLog

ChangeLog: stamp-changelog

stamp-changelog:
	-git log > ChangeLog-tmp
	test -s ChangeLog-tmp && mv ChangeLog-tmp ChangeLog
	-rm -f ChangeLog-tmp
	test -e ChangeLog || touch ChangeLog

lib_LTLIBRARIES = libdvdcss.la
EXTRA_PROGRAMS = csstest dvd_region

libdvdcss_la_SOURCES = \
	src/libdvdcss.c \
	src/libdvdcss.h \
	src/device.c \
	src/device.h \
	src/css.c \
	src/css.h \
	src/csstables.h \
	src/ioctl.c \
	src/ioctl.h \
	src/error.c \
	src/common.h

libdvdcss_la_LDFLAGS = -version-info 3:0:1 $(DVDCSS_LDFLAGS)
libdvdcss_la_LIBADD =

if !SYS_MSVC
libdvdcss_la_LDFLAGS += = -no-undefined
endif

csstest_SOURCES = test/csstest.c
csstest_LDADD = libdvdcss.la
csstest_CFLAGS = -I $(top_srcdir)/src

dvd_region_SOURCES = test/dvd_region.c src/ioctl.c
dvd_region_LDADD = libdvdcss.la
dvd_region_CFLAGS = -I $(top_srcdir)/src

pkgincludedir = $(includedir)/dvdcss
pkginclude_HEADERS = src/dvdcss/dvdcss.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = src/libdvdcss.pc

apidoc: stamp-doxygen stamp-latex

stamp-doxygen: doc/doxygen.cfg
	cd doc && $(DOXYGEN) doxygen.cfg
	touch $@

stamp-latex: stamp-doxygen
if LATEX
	$(MAKE) -C doc/latex ps
endif
	touch $@

clean-local:
	-rm -Rf stamp-latex stamp-doxygen doc/html doc/latex
