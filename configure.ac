AC_INIT([giles], [0.1], [bricka@ccs.neu.edu], [giles], [http://github.com/bricka/giles])
AM_INIT_AUTOMAKE
AM_PROG_CC_C_O
AC_CONFIG_FILES([Makefile src/Makefile])
AC_SEARCH_LIBS([cdio_open], [cdio])
AC_SEARCH_LIBS([cddb_disc_new], [cddb])
AC_CHECK_PROG([cdparanoia_exists], [cdparanoia], [yes], [no])
AC_CHECK_PROG([lame_exists], [lame], [yes], [no])
AM_PATH_GTK_3_0
AC_OUTPUT
