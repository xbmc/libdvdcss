SUBDIRS = dvdcss

lib_LTLIBRARIES = libdvdcss.la

libdvdcss_la_SOURCES = \
	libdvdcss.c libdvdcss.h \
	device.c device.h \
	css.c css.h csstables.h \
	ioctl.c ioctl.h \
	error.c \
	common.h \
	$(bsdi_sources)

EXTRA_libdvdcss_la_SOURCES = \
	$(bsdi_extras)

libdvdcss_la_LIBADD = $(bsdi_libadd)
libdvdcss_la_LDFLAGS = -version-info 2:2:0 $(beos_ldflags)

bsdi_extras = bsdi_ioctl.c bsdi_dvd.h

if SYS_BSDI
bsdi_sources = $(bsdi_extras)
bsdi_libadd = -ldvd
endif

if SYS_BEOS
beos_ldflags = -no-undefined
endif

