## Process this file with automake to produce Makefile.in.
## This file is part of jpegpixi, a program to interpolate pixels in
## JFIF image files.
## Copyright (C) 2002, 2003, 2004 Martin Dickopp
##
## This file is free software; the copyright holder gives unlimited
## permission to copy and/or distribute it, with or without
## modifications, as long as this notice is preserved.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY, to the extent permitted by law; without
## even the implied warranty of MERCHANTABILITY or FITNESS FOR A
## PARTICULAR PURPOSE.

SUBDIRS = . man @SUBDIR_PO@
DIST_SUBDIRS = . man po

bin_PROGRAMS = jpegpixi jpeghotp

AM_CPPFLAGS = -DLOCALEDIR="\"$(datadir)/locale\""

jpegpixi_SOURCES = consolidate.c dct.c file.c interpolate.c jfif.c jpegpixi.c jpegpixi.h optpixi.c optpixi.h \
                   parsenum.c rbtree.c rbtree.h util.c util.h weights.c
nodist_jpegpixi_SOURCES = jpegwrapper.h
jpegpixi_LDADD = @LIBOBJS@ @MATHLIB@

jpeghotp_SOURCES = consolidate.c jpeghotp.c jpegpixi.h opthotp.c opthotp.h parsenum.c rbtree.c rbtree.h util.c util.h
nodist_jpeghotp_SOURCES = jpegwrapper.h
jpeghotp_LDADD = @LIBOBJS@

BUILT_SOURCES = jpegwrapper.h optpixi.c optpixi.h opthotp.c opthotp.h po/POTSOURCES
EXTRA_DIST = jpegpixi.opag jpeghotp.opag README.jpeglib po/POTSOURCES
DISTCLEANFILES = jpegwrapper.h po/POTSOURCES

optpixi.h: jpegpixi.opag
optpixi.c: optpixi.h jpegpixi.opag
	@if test "@OPAG@" = missing; then \
	  echo "You have modified \`jpegpixi.opag', but you do not seem to have the \`opag'"; \
	  echo "program installed on your system.  Check the \`README' file; it tells you"; \
	  echo "where to get this program."; \
	  exit 1; \
	fi
	@OPAG@ --gcc --nls-error-text=_ --nls-help-text=_ $(srcdir)/jpegpixi.opag optpixi.c optpixi.h

opthotp.h: jpeghotp.opag
opthotp.c: opthotp.h jpeghotp.opag
	@if test "@OPAG@" = missing; then \
	  echo "You have modified \`jpeghotp.opag', but you do not seem to have the \`opag'"; \
	  echo "program installed on your system.  Check the \`README' file; it tells you"; \
	  echo "where to get this program."; \
	  exit 1; \
	fi
	@OPAG@ --gcc --nls-error-text=_ --nls-help-text=_ $(srcdir)/jpeghotp.opag opthotp.c opthotp.h

po/POTSOURCES: $(jpegpixi_SOURCES) $(jpeghotp_SOURCES) jpegpixi.opag jpeghotp.opag
	@{ for p in $(jpegpixi_SOURCES) $(jpeghotp_SOURCES); do \
	     echo "$$p"; \
	   done; } > po/POTSOURCES
