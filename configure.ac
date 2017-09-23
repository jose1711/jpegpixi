dnl Process this file with autoconf to produce configure.
dnl This file is part of jpegpixi, a program to interpolate pixels in
dnl JFIF image files.
dnl Copyright (C) 2002, 2003, 2004, 2005 Martin Dickopp
dnl
dnl This file is free software; the copyright holder gives unlimited
dnl permission to copy and/or distribute it, with or without
dnl modifications, as long as this notice is preserved.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

AC_PREREQ(2.59)
AC_INIT([jpegpixi], [1.1.1], [martin@zero-based.org])
AC_CONFIG_SRCDIR([jpegpixi.c])
AC_COPYRIGHT([Copyright (C) 2002, 2003, 2004, 2005 Martin Dickopp])
AM_INIT_AUTOMAKE([gnu no-define std-options 1.9])
AC_CONFIG_HEADER([config.h])

MD_PATH_PROG([opag])

AC_PROG_CC
if test "x$ac_cv_prog_cc_stdc" = xno; then
  AC_MSG_ERROR([an ANSI C compiler is required to build this package])
fi
AC_C_CONST

if test $ac_cv_c_const = no; then
  MD_NO_CONST=1
else
  MD_NO_CONST=0
fi
AC_SUBST([MD_NO_CONST])

AC_CHECK_HEADERS([errno.h float.h malloc.h math.h], [], [], [AC_INCLUDES_DEFAULT])
AC_CHECK_DECLS([strerror, strchr], [], [], [AC_INCLUDES_DEFAULT])
AC_CHECK_TYPES([size_t, ssize_t, ptrdiff_t])

MATHLIB= save_LIBS="$LIBS"
AC_SEARCH_LIBS([fabs], [m], [MATHLIB="-lm"])
LIBS="$save_LIBS"
AC_SUBST([MATHLIB])

AC_SEARCH_LIBS([remove], [posix])
AC_SEARCH_LIBS([strerror], [cposix])

AC_CHECK_FUNCS([fgetc_unlocked])

AC_REPLACE_FUNCS([strerror])
if test "x$ac_cv_func_strerror" != xyes; then
  MD_CHECK_TYPE_SYS_ERRLIST
fi

MD_PATH(jpeglib, jpeglib.h, jpeg, jpeg_start_decompress)
if test $md_path_have_jpeglib = no; then
  AC_MSG_ERROR([jpeglib is required to build this package])
fi

MD_NLS

CPPFLAGS="$JPEGLIB_CPPFLAGS $CPPFLAGS"
LIBS="$JPEGLIB_LIBS $LIBS"
AC_CONFIG_FILES(Makefile man/Makefile man/de/Makefile man/fi/Makefile man/fr/Makefile po/Makefile jpegwrapper.h
                man/jpegpixi.1 man/jpeghotp.1 man/de/jpegpixi.1 man/de/jpeghotp.1 man/fi/jpegpixi.1 man/fi/jpeghotp.1
                man/fr/jpegpixi.1 man/fr/jpeghotp.1)
AC_OUTPUT
