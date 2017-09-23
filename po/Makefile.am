## Process this file with automake to produce Makefile.in.
## This file is part of jpegpixi, a program to interpolate pixels in
## JFIF image files.
## Copyright (C) 2004 Martin Dickopp
##
## This file is free software; the copyright holder gives unlimited
## permission to copy and/or distribute it, with or without
## modifications, as long as this notice is preserved.
##
## This file is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY, to the extent permitted by law; without
## even the implied warranty of MERCHANTABILITY or FITNESS FOR A
## PARTICULAR PURPOSE.

POFILES = de.po fi.po fr.po
MOFILES = de.gmo fi.gmo fr.gmo

dist_noinst_DATA = $(MOFILES)

BUILT_SOURCES = @PACKAGE_TARNAME@.pot
EXTRA_DIST = @PACKAGE_TARNAME@.pot $(POFILES)
MAINTAINERCLEANFILES = $(MOFILES)

@PACKAGE_TARNAME@.pot: POTSOURCES
	if test -f POTSOURCES; then d=.; else d="$(srcdir)"; fi; \
	@XGETTEXT@ --directory="$(top_builddir)" --directory="$(top_srcdir)" --output=@PACKAGE_TARNAME@.pot \
	           --msgid-bugs-address="Martin Dickopp <@PACKAGE_BUGREPORT@>" --add-comments=TRANSLATORS \
	           --width=120 --keyword=_ --files-from="$$d/POTSOURCES"

$(POFILES): @PACKAGE_TARNAME@.pot
	@MSGMERGE@ --directory=. --directory="$(srcdir)" --output-file=$@ --width=120 $@ @PACKAGE_TARNAME@.pot

.po.gmo:
	@MSGFMT@ --directory=. --directory="$(srcdir)" --output-file=$@ --check $<

install-data-local:
	@for p in $(MOFILES); do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  m=`echo $$p | sed -e 's/\.[^.]*$$//'`; \
	  echo " $(mkdir_p) '$(DESTDIR)$(datadir)/locale/$$m/LC_MESSAGES'"; \
	  $(mkdir_p) "$(DESTDIR)$(datadir)/locale/$$m/LC_MESSAGES"; \
	  echo " $(INSTALL_DATA) '$$d$$p' '$(DESTDIR)$(datadir)/locale/$$m/LC_MESSAGES/@PACKAGE_TARNAME@.mo'"; \
	  $(INSTALL_DATA) "$$d$$p" "$(DESTDIR)$(datadir)/locale/$$m/LC_MESSAGES/@PACKAGE_TARNAME@.mo"; \
	done

uninstall-local:
	@for p in $(MOFILES); do \
	  m=`echo $$p | sed -e 's/\.[^.]*$$//'`; \
	  echo " rm -f '$(DESTDIR)$(datadir)/locale/$$m/LC_MESSAGES/@PACKAGE_TARNAME@.mo'"; \
	  rm -f "$(DESTDIR)$(datadir)/locale/$$m/LC_MESSAGES/@PACKAGE_TARNAME@.mo"; \
	done
