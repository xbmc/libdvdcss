SUBDIRS = dvdcss

lib_LTLIBRARIES = libdvdcss.la
libdvdcss_la_SOURCES = libdvdcss.c css.c ioctl.c bsdi_ioctl.c
libdvdcss_la_LIBADD = @EXTRA_LIB@
libdvdcss_la_LDFLAGS = -version-info 2:1:0

noinst_HEADERS = csstables.h ioctl.h common.h css.h libdvdcss.h bsdi_dvd.h
