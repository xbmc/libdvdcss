SUBDIRS = dvdcss

lib_LTLIBRARIES = libdvdcss.la

libdvdcss_la_SOURCES = libdvdcss.c device.c error.c css.c ioctl.c

EXTRA_libdvdcss_la_SOURCES = bsdi_ioctl.c
libdvdcss_la_DEPENDENCIES = @EXTRA_SOURCES@

libdvdcss_la_LIBADD = @EXTRA_SOURCES@ @EXTRA_LIB@
libdvdcss_la_LDFLAGS = -version-info 2:1:0

noinst_HEADERS = csstables.h ioctl.h device.h common.h css.h libdvdcss.h bsdi_dvd.h
