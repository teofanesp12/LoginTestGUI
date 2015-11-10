AC_INIT([login], [0.02], [teofanesp12@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
PKG_CHECK_MODULES([GTK3], [gtk+-3.0 >= 3.0])
PKG_CHECK_MODULES([libpq], libpq, has_postgres=true, has_postgres=false)
AC_CONFIG_HEADERS([config.h])

CFLAGS="$CFLAGS $GTK3_CFLAGS"
LIBS="$LIBS $GTK3_LIBS"

CFLAGS="$CFLAGS -D WITH_LIBPQ $libpq_CFLAGS"
LIBS="$LIBS $libpq_LIBS"

AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT
