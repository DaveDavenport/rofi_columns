AC_INIT([rofi_columns], [0.0.1], [https://github.com/DaveDavenport/rofi_columns/])

AC_CONFIG_SRCDIR([source/rofi_columns.c])
AC_CONFIG_HEADER([config.h])

dnl ---------------------------------------------------------------------
dnl Setup automake to be silent and in foreign mode.
dnl We want xz distribution
dnl ---------------------------------------------------------------------
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects dist-xz])
AM_SILENT_RULES([yes])

dnl ---------------------------------------------------------------------
dnl  Check for compiler
dnl ---------------------------------------------------------------------
AC_PROG_CC([clang gcc cc])

dnl ---------------------------------------------------------------------
dnl  C99 standard
dnl ---------------------------------------------------------------------
AC_PROG_CC_C99

dnl ---------------------------------------------------------------------
dnl C to Object rules.
dnl ---------------------------------------------------------------------
AM_PROG_CC_C_O

dnl ---------------------------------------------------------------------
dnl System extensions
dnl ---------------------------------------------------------------------
AC_USE_SYSTEM_EXTENSIONS

dnl ---------------------------------------------------------------------
dnl Base CFLAGS
dnl ---------------------------------------------------------------------
AM_CFLAGS="-Wall -Wextra -Wparentheses -Winline -pedantic  -Wunreachable-code"

dnl ---------------------------------------------------------------------
dnl Check for C functions.
dnl ---------------------------------------------------------------------
AC_CHECK_FUNC([getline],,  AC_MSG_ERROR("Could not find getline in c library"))
AC_CHECK_FUNC([getdelim],, AC_MSG_ERROR("Could not find getdelim in c library"))

dnl ---------------------------------------------------------------------
dnl Check dependencies
dnl ---------------------------------------------------------------------
PKG_PROG_PKG_CONFIG


dnl ---------------------------------------------------------------------
dnl PKG_CONFIG based dependencies  
dnl ---------------------------------------------------------------------
PKG_CHECK_MODULES([glib],     [glib-2.0 >= 2.40])


dnl ---------------------------------------------------------------------
dnl Add extra compiler flags
dnl ---------------------------------------------------------------------
AC_SUBST([AM_CFLAGS])

AC_CONFIG_FILES([Makefile ])
AC_OUTPUT

