SUBDIRS = src test autotools
DIST_SUBDIRS = $(SUBDIRS) debian doc

EXTRA_DIST = libdvdcss.spec bootstrap

AUTOMAKE_OPTIONS = foreign dist-bzip2 subdir-objects

doc-dummy:

doc: doc-dummy
	$(MAKE) -C doc doc

