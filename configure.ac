AC_INIT(src/libdvdcss.c)

AC_CANONICAL_SYSTEM
AC_CONFIG_AUX_DIR(.)

AM_INIT_AUTOMAKE(libdvdcss, 1.2.4)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_STDC_HEADERS

AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL 

AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

AC_CHECK_HEADERS(unistd.h sys/param.h)

dnl
dnl  Check the operating system
dnl
case x"${target_os}" in
  xdarwin*)
    CFLAGS="${CFLAGS} -no-cpp-precomp"
    ;;
  x*cygwin*)
    AC_DEFINE(SYS_CYGWIN, 1, Have a Cygwin system.)
    AC_DEFINE(WIN32, 1, Using Win32.)
    ;;
  xbeos*)
    AC_DEFINE(SYS_BEOS, 1, Have a BeOS system.)
    ;;
  x*msvc*)
    SYS_MSVC=1
    ;;
  x*)
    ;;
esac

dnl
dnl  libdvdcss: check for DVD ioctls
dnl

dnl  default is no
CAN_BUILD_LIBDVDCSS=0

dnl  for windoze
AC_CHECK_HEADERS(windows.h,[
  AC_CHECK_HEADERS(direct.h,,,[
  #include <windows.h>
  ])
  AC_CHECK_HEADERS(winioctl.h,[
    CAN_BUILD_LIBDVDCSS=1
  ],,[
  #include <windows.h>
  ])
])

dnl  for Un*x and BeOS
AC_CHECK_HEADERS(sys/ioctl.h,[
  CAN_BUILD_LIBDVDCSS=1
  AC_CHECK_HEADERS(sys/cdio.h sys/dvdio.h linux/cdrom.h dvd.h)
  BSD_DVD_STRUCT=0
  LINUX_DVD_STRUCT=0
  OPENBSD_DVD_STRUCT=0
  dnl
  dnl Old FreeBSD: sys/cdio.h
  dnl
  AC_EGREP_HEADER(dvd_struct,sys/cdio.h,[
    AC_DEFINE(DVD_STRUCT_IN_SYS_CDIO_H, 1,
              Define if <sys/cdio.h> defines dvd_struct.)
    AC_EGREP_HEADER(struct dvd_struct,sys/cdio.h,[
                    BSD_DVD_STRUCT=1],[LINUX_DVD_STRUCT=1; OPENBSD_DVD_STRUCT=1])
  ])
  dnl
  dnl Newer FreeBSD: sys/dvdio.h
  dnl
  AC_EGREP_HEADER(dvd_struct,sys/dvdio.h,[
    AC_DEFINE(DVD_STRUCT_IN_SYS_DVDIO_H, 1,
              Define if <sys/dvdio.h> defines dvd_struct.)
    AC_EGREP_HEADER(struct dvd_struct,sys/dvdio.h,[
                    BSD_DVD_STRUCT=1],[LINUX_DVD_STRUCT=1])
  ])
  dnl
  dnl Linux: linux/cdrom.h
  dnl
  AC_EGREP_HEADER(dvd_struct,linux/cdrom.h,[
    AC_DEFINE(DVD_STRUCT_IN_LINUX_CDROM_H, 1,
              Define if <linux/cdrom.h> defines DVD_STRUCT.)
    LINUX_DVD_STRUCT=1
  ])
  dnl
  dnl BSDI: dvd.h
  dnl
  AC_EGREP_HEADER(dvd_struct,dvd.h,[
    AC_DEFINE(DVD_STRUCT_IN_DVD_H, 1,
              Define if <dvd.h> defines DVD_STRUCT.)
    LINUX_DVD_STRUCT=1
  ],[
    dnl
    dnl BSDI: /sys/dev/scsi/scsi_ioctl.h, using our own libdvd
    dnl
    AC_CHECK_HEADERS(/sys/dev/scsi/scsi_ioctl.h,[
      AC_DEFINE(DVD_STRUCT_IN_BSDI_DVDIOCTL_DVD_H, 1,
                Define if <extras/BSDI_dvdioctl/dvd.h> defines DVD_STRUCT.)
      SYS_BSDI=1
      LINUX_DVD_STRUCT=1
    ])
  ])
  dnl
  dnl Solaris: sys/scsi/scsi_types.h, sys/scsi/impl/uscsi.h
  dnl
  AC_CHECK_HEADER(sys/scsi/scsi_types.h,[
    AC_CHECK_HEADER(sys/scsi/impl/uscsi.h,[
      AC_DEFINE(SOLARIS_USCSI, 1, Have userspace SCSI headers.)
    ])
  ])
  dnl
  dnl HP-UX: sys/scsi.h
  dnl
  AC_CHECK_HEADER(sys/scsi.h,[
    AC_EGREP_HEADER(sctl_io,sys/scsi.h,[
      AC_DEFINE(HPUX_SCTL_IO, 1, Define if <sys/scsi.h> defines sctl_io.)
    ])
  ])
  dnl
  dnl Darwin 
  dnl
  AC_CHECK_HEADER(IOKit/storage/IODVDMediaBSDClient.h,[
    AC_DEFINE(DARWIN_DVD_IOCTL, 1, Have IOKit DVD IOCTL headers)
  ])
  dnl
  dnl Final tests to check what was detected
  dnl
  if test x$LINUX_DVD_STRUCT = x1; then
    AC_DEFINE(HAVE_LINUX_DVD_STRUCT, 1, 
              Define if Linux-like dvd_struct is defined.)
    if test x$OPENBSD_DVD_STRUCT = x1; then
      AC_DEFINE(HAVE_OPENBSD_DVD_STRUCT, 1,
                Define if OpenBSD-like dvd_struct is defined.)
    fi
  else
    if test x$BSD_DVD_STRUCT = x1; then
      AC_DEFINE(HAVE_BSD_DVD_STRUCT, 1,
                Define if FreeBSD-like dvd_struct is defined.)
    fi
  fi
])

CFLAGS="-Wall ${CFLAGS}"
CFLAGS="-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDVDCSS_DIST ${CFLAGS}"

AM_CONDITIONAL(SYS_BSDI, test "x$SYS_BSDI" = "x1")
AM_CONDITIONAL(SYS_MSVC, test "x$SYS_MSVC" = "x1")

AC_OUTPUT([Makefile src/Makefile src/dvdcss/Makefile test/Makefile debian/Makefile doc/Makefile])

