AC_INIT([pvg], [1.0], [savoirfaire@hhra.me])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_PROG_RANLIB
AM_PROG_AR

AM_EXTRA_RECURSIVE_TARGETS([lib src])
AC_CONFIG_FILES([Makefile lib/Makefile lib/graphing/Makefile src/Makefile])

AC_CHECK_HEADERS(pthread.h,, [AC_MSG_ERROR([pthread.h required])])
AC_CHECK_HEADERS(ncurses.h,, [AC_MSG_ERROR([ncurses.h required])])

AC_CHECK_LIB(ncursesw, wprintw, [LIBS="$LIBS -lncursesw"])
AC_CHECK_LIB(pthread, pthread_create, [LIBS="$LIBS -lpthread"])

AC_OUTPUT
