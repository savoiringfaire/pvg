AC_INIT([pvg], [1.0], [savoirfaire@hhra.me])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
])

AC_CHECK_HEADERS(pthread.h,, [AC_MSG_ERROR([pthread.h required])])
AC_CHECK_HEADERS(ncurses.h,, [AC_MSG_ERROR([ncurses.h required])])

AC_CHECK_LIB(ncursesw, printw, [LIBS="$LIBS -lncursesw"])
AC_CHECK_LIB(pthread, pthread_create, [LIBS="$LIBS -lpthread"])

AC_OUTPUT
