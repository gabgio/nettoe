dnl configure.in file for nettoe
dnl Initiated by Gabriele Giorgetti <g.gabriele@europe.com>
dnl Continued by Mats Erik Andersson <meand@users.sourceforge.net>

dnl To be processed by autoconf.

AC_INIT([nettoe], [1.5.1], [meand@users.sourceforge.net])
AC_PREREQ(2.59)
AC_REVISION([in CVS, $Revision$])

AC_CONFIG_SRCDIR([src/nettoe.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS(config.h)
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.9 -Wall -Werror no-define])

AC_DEFINE([PACKAGE_RELEASE_DATE],
	  ["March 21th, 2014"], [Date of source release.])

dnl Decide on using terminfo
AC_ARG_WITH([terminfo],
  [AS_HELP_STRING([--without-terminfo],
    [do not use terminfo/termcap])])

dnl Choice of server's TCP port.
AC_ARG_WITH([port],
  [AS_HELP_STRING([--with-port=NUM],
    [numeric TCP port of server (defaults to 7501)])],
    [], [with_port=7501])

dnl Supporting files for desktop.
AC_ARG_ENABLE([desktop],
  [AS_HELP_STRING([--enable-desktop],
    [install icons and desktop description])])
AC_ARG_ENABLE([menu],
  [AS_HELP_STRING([--enable-menu],
    [install menu file on desktop systems])])

AM_CONDITIONAL([ENABLE_DESKTOP], [test "$enable_desktop" = yes])
AM_CONDITIONAL([ENABLE_MENU], [test "$enable_menu" = yes])

AC_PROG_CC
AC_PROG_SED

dnl Check for standard C headers
AC_HEADER_STDC

dnl Check for extra required headers
AC_CHECK_HEADERS(       \
	string.h	\
	strings.h	\
        errno.h         \
	signal.h	\
        netdb.h         \
        netinet/in.h    \
	arpa/inet.h	\
        sys/socket.h    \
	sys/types.h	\
	sys/wait.h	\
	unistd.h        \
	curses.h        \
	ncurses.h	\
	ncurses/ncurses.h	\
	ncurses/term.h	\
	termcap.h	\
)

dnl OpenSolaris complains if <term.h> is not
dnl included together with <curses.h>. This
dnl works around that issue.
if test "$ac_cv_header_curses_h" = "yes"; then
  AC_MSG_CHECKING(for term.h with curses.h)
  save_LIBS="$LIBS"
  LIBS="$save_LIBS -lcurses"
  AC_TRY_LINK([#include <curses.h>
#include <term.h>], ,
    [eval "ac_nettoe_curses_term=yes"],
    [eval "ac_nettoe_curses_term=no"])
  LIBS="$save_LIBS"
  if test "$ac_nettoe_curses_term" = "yes"; then
    AC_DEFINE(HAVE_TERM_H, 1,
	    [Define to 1 if you have <term.h>])
  fi
  AC_MSG_RESULT($ac_nettoe_curses_term)
fi

dnl Check for library functions
AC_CHECK_FUNCS(strcat strcmp strcpy strncasecmp)

dnl Network library for Solaris
AC_SEARCH_LIBS([socket],[socket])

dnl Supported terminal capability libraries.
dnl They are checked in this order later on.
AC_CHECK_LIB([tinfo], [setupterm], [LIBTINFO=-ltinfo])
AC_CHECK_LIB([ncurses], [setupterm], [LIBNCURSES=-lncurses],
  [## Dig deeper for ncurses with OpenSolaris.
   AC_MSG_CHECKING([for setupterm and ncurses in /usr/gnu/lib])
   saved_LDFLAGS=$LDFLAGS
   saved_LIBS=$LIBS
   LDFLAGS="$LDFLAGS -L/usr/gnu/lib"
   LIBS="$LIBS -lncurses"
   dnl AC_CHECK_LIB([ncurses], [setupterm], [LIBNCURSES=-lncurses])
   AC_LINK_IFELSE([AC_LANG_CALL([], [setupterm])],
		  [LIBNCURSES=-lncurses
		   NETTOE_LDADD="$NETTOE_LDADD -L/usr/gnu/lib"
		   ac_nettoe_usr_gnu_lib_ncurses=yes
		   AC_MSG_RESULT([yes])],
		  [
		   ac_nettoe_usr_gnu_lib_ncurses=no
		   AC_MSG_RESULT([no])])
   LDFLAGS=$saved_LDFLAGS
   LIBS=$saved_LIBS]dnl
)
AC_CHECK_LIB([curses], [setupterm], [LIBCURSES=-lcurses])
AC_CHECK_LIB([termcap], [tgetent], [LIBTERMCAP=-ltermcap])
AC_SUBST(LIBTINFO)
AC_SUBST(LIBNCURSES)
AC_SUBST(LIBCURSES)
AC_SUBST(LIBTERMCAP)
AC_SUBST(NETTOE_LDADD)

dnl Use setupterm() and tgetent() as markers
dnl for choosing between libtinfo, libncurses,
dnl libcurses, and libtermcap.
if test "$with_terminfo" = "no" ; then
  NETTOE_LDADD=
else
  AC_MSG_CHECKING([for best terminal capability library])
  if test "$ac_cv_lib_tinfo_setupterm" = "yes" \
      && test "$ac_cv_header_ncurses_h" = "yes" ; then
	AC_DEFINE(HAVE_SETUPTERM, 1,
		  [Use setupterm() for terminal setup])
	AC_DEFINE(USE_NCURSES, 1,
		  [Use ncurses for terminal capabilities])
	NETTOE_LDADD="${NETTOE_LDADD} ${LIBTINFO}"
	with_terminfo=yes
	AC_MSG_RESULT(${LIBTINFO})
  elif ( test "$ac_cv_lib_ncurses_setupterm" = "yes" \
  	 || test "x$ac_nettoe_usr_gnu_lib_ncurses" = x"yes" ) \
	&& ( test "$ac_cv_header_ncurses_h" = "yes" \
	     || test "$ac_cv_header_ncurses_ncurses_h" = "yes" ); then
	  AC_DEFINE(HAVE_SETUPTERM, 1,
		    [Use setupterm() for terminal setup])
	  AC_DEFINE(USE_NCURSES, 1,
		    [Use ncurses for terminal capabilities])
	  NETTOE_LDADD="${NETTOE_LDADD} ${LIBNCURSES}"
	  with_terminfo=yes
	  AC_MSG_RESULT($LIBNCURSES)
  elif test "$ac_cv_lib_curses_setupterm" = "yes" \
	&& test "$ac_cv_header_curses_h" = "yes" ; then
	  AC_DEFINE(HAVE_SETUPTERM, 1,
		    [Use setupterm() for terminal setup])
	  AC_DEFINE(USE_CURSES, 1,
		    [Use curses for terminal capabilities])
	  NETTOE_LDADD="${NETTOE_LDADD} ${LIBCURSES}"
	  with_terminfo=yes
	  AC_MSG_RESULT($LIBCURSES)
  elif test "$ac_cv_lib_termcap_tgetent" = "yes" \
	&& test "$ac_cv_header_termcap_h" = "yes" ; then
	  AC_DEFINE(HAVE_TGETENT, 1,
		    [Use tgetent() for terminal setup])
	  AC_DEFINE(USE_TERMCAP, 1,
		    [Use termcap for terminal capabilities])
	  NETTOE_LDADD="${NETTOE_LDADD} ${LIBTERMCAP}"
	  with_terminfo=yes
	  AC_MSG_RESULT($LIBTERMCAP)
  else
	  with_terminfo=no
	  NETTOE_LDADD=
	  AC_MSG_RESULT(none)
  fi

  if test "$ac_cv_lib_ncurses_setupterm" = "yes" \
      || test "$ac_cv_lib_curses_setupterm" = "yes" \
      || test "$ac_cv_lib_tinfo_setupterm" = "yes" ; then
    AC_DEFINE(USE_TERMINFO, 1, [Use terminfo for terminal capabilities])
  fi
fi

# Parse selected listening port.
if test "$with_port" != "yes" && test "$with_port" != "no"; then
  test -n "$NETTOE_PORT" || NETTOE_PORT=$with_port
fi

test -n "$NETTOE_PORT" || NETTOE_PORT=7501

AC_ARG_VAR([NETTOE_PORT], [numeric TCP port of server])

test -n "$applicationsdir" || applicationsdir='${datadir}/applications'
test -n "$iconsdir" || iconsdir='${datadir}/icons'
test -n "$menudir" || menudir='${datadir}/menu'
test -n "$pixmapsdir" || pixmapsdir='${datadir}/pixmaps'

AC_ARG_VAR([applicationsdir], [program descriptions [$(datadir)/applications]])
AC_ARG_VAR([iconsdir], [base directory for icons [$(datadir)/icons]])
AC_ARG_VAR([menudir], [location of menu descriptions [$(datadir)/menu]])
AC_ARG_VAR([pixmapsdir], [location of pixmaps [$(datadir)/pixmaps]])

AC_CONFIG_FILES([Makefile docs/Makefile src/Makefile test/Makefile
		 desktop/Makefile])

AC_OUTPUT

dnl ${prefix} is embedded two levels into the string values.
echo ${bindir} | $GREP '\$' >/dev/null 2>&1 && bindir=`eval echo ${bindir}`
echo ${bindir} | $GREP '\$' >/dev/null 2>&1 && bindir=`eval echo ${bindir}`

echo ${mandir} | $GREP '\$' >/dev/null 2>&1 && mandir=`eval echo ${mandir}`
echo ${mandir} | $GREP '\$' >/dev/null 2>&1 && mandir=`eval echo ${mandir}`

echo ${datadir} | $GREP '\$' >/dev/null 2>&1 && datadir=`eval echo ${datadir}`
echo ${datadir} | $GREP '\$' >/dev/null 2>&1 && datadir=`eval echo ${datadir}`

echo ${applicationsdir} | $GREP '\$' >/dev/null 2>&1 && applicationsdir=`eval echo ${applicationsdir}`
echo ${iconsdir} | $GREP '\$' >/dev/null 2>&1 && iconsdir=`eval echo ${iconsdir}`
echo ${pixmapsdir} | $GREP '\$' >/dev/null 2>&1 && pixmapsdir=`eval echo ${pixmapsdir}`
echo ${menudir} | $GREP '\$' >/dev/null 2>&1 && menudir=`eval echo ${menudir}`

## Summary of configuration results.
##
cat <<EOT

  ${PACKAGE_NAME}, version ${PACKAGE_VERSION}:

    Prefix:       ${prefix}
    Bindir:       ${bindir}
    Mandir:       ${mandir}
EOT

if test x"$enable_desktop" = xyes; then
  cat <<EOT

    Desktop file: ${applicationsdir}/nettoe.desktop
    Icons:        ${iconsdir}/hicolor/...
    Pixmap:       ${pixmapsdir}/nettoe.xpm
EOT
  if test x"$enable_menu" = xyes; then
    cat <<EOT
    Menu file:    ${menudir}/nettoe.menu
EOT
  fi
fi

cat <<EOT

    TCP port:     ${NETTOE_PORT}
    Curses:       ${with_terminfo}

    Compiling:    ${CC} ${CPPFLAGS} ${CFLAGS}
    Linking:      ${NETTOE_LDADD}

    Build:        ${build}
    Host:         ${host}

EOT
