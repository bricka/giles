AC_INIT([giles], [0.1], [bricka@ccs.neu.edu], [giles], [http://github.com/bricka/giles])
AM_INIT_AUTOMAKE
AM_PROG_CC_C_O
AC_CONFIG_FILES([Makefile src/Makefile])
AC_SEARCH_LIBS([cdio_open], [cdio])
AC_SEARCH_LIBS([cddb_disc_new], [cddb])
AC_CHECK_PROG([cdparanoia_exists], [cdparanoia], [yes])
AC_CHECK_PROG([lame_exists], [lame], [yes])

if test "$cdparanoia_exists" != "yes" ; then
    AC_MSG_ERROR([cdparanoia is not installed])
fi

if test "$lame_exists" != "yes" ; then
    AC_MSG_ERROR([lame is not installed])
fi

AM_PATH_GTK_3_0
AC_ARG_ENABLE([debug], [AS_HELP_STRING([--enable-debug], [enable debugging (default is disabled)])], [CFLAGS="$CFLAGS -DDEBUG"])
AC_OUTPUT
