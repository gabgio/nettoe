# Makefile.am file for nettoe
# Maintained by Mats Erik Andersson <meand@users.sourceforge.net>

# This file is processed by automake.

EXTRA_DIST = ocross.xpm ocross_skew.xpm \
	     ocross_left.xpm ocross_right.xpm \
	     ocross_bottom.xpm ocross_top.xpm \
	     ocross16.png ocross32.png \
	     ocross_skew16.png ocross_skew32.png \
	     nettoe.desktop.in nettoe.menu.in

DESKTOP = nettoe.desktop
ICON = ocross_skew
MENU = nettoe.menu

BUILT_SOURCES =

if ENABLE_DESKTOP
BUILT_SOURCES += $(DESKTOP)
if ENABLE_MENU
BUILT_SOURCES += $(MENU)
endif
endif

applicationsdir = @applicationsdir@
hicolordir = @iconsdir@/hicolor
menudir = @menudir@
pixmapsdir = @pixmapsdir@

rewrite = 's|[@]bindir[@]|$(bindir)|; s|[@]pixmapsdir[@]|$(pixmapsdir)|'

$(DESKTOP): Makefile $(DESKTOP).in
	$(SED) -e $(rewrite) $(srcdir)/$@.in > $@

$(MENU): Makefile $(MENU).in
	$(SED) -e $(rewrite) $(srcdir)/$@.in > $@

if ENABLE_DESKTOP
applications_DATA = $(DESKTOP)

if ENABLE_MENU
menu_DATA = $(MENU)
endif

install-data-hook:
	test -d "$(DESTDIR)$(pixmapsdir)" || \
	  $(mkdir_p) "$(DESTDIR)$(pixmapsdir)"
	$(INSTALL_DATA) $(srcdir)/$(ICON).xpm "$(DESTDIR)$(pixmapsdir)"/nettoe.xpm
	test -d "$(DESTDIR)$(hicolordir)"/16x16/apps || \
	  $(mkdir_p) "$(DESTDIR)$(hicolordir)"/16x16/apps
	test -d "$(DESTDIR)$(hicolordir)"/32x32/apps || \
	  $(mkdir_p) "$(DESTDIR)$(hicolordir)"/32x32/apps
	test -s $(srcdir)/"$(ICON)16.png" && \
	  $(INSTALL_DATA) $(srcdir)/"$(ICON)16.png" "$(DESTDIR)$(hicolordir)"/16x16/apps/nettoe.png
	test -s $(srcdir)/"$(ICON)32.png" && \
	  $(INSTALL_DATA) $(srcdir)/"$(ICON)32.png" "$(DESTDIR)$(hicolordir)"/32x32/apps/nettoe.png

uninstall-hook:
	rm "$(DESTDIR)$(pixmapsdir)"/nettoe.xpm \
	   "$(DESTDIR)$(hicolordir)"/16x16/apps/nettoe.png \
	   "$(DESTDIR)$(hicolordir)"/32x32/apps/nettoe.png
endif

CLEANFILES = $(DESKTOP) $(MENU)
