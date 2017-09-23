## This file is part of jpegpixi, a program to interpolate pixels in
## JFIF image files.
## Copyright (C) 2001, 2002, 2003, 2004, 2005 Martin Dickopp
##
## This file is free software; the copyright holder gives unlimited
## permission to copy and/or distribute it, with or without
## modifications, as long as this notice is preserved.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY, to the extent permitted by law; without
## even the implied warranty of MERCHANTABILITY or FITNESS FOR A
## PARTICULAR PURPOSE.


# MD_PATH_PROG(PROGRAM)
# ---------------------
# Set PROGRAM to the path to PROGRAM, or to `missing' if the program is not
# available.
AC_DEFUN([MD_PATH_PROG],
[AS_VAR_PUSHDEF([md_Program], [md_path_$1])dnl
AS_VAR_SET(md_Program, [])
AC_ARG_WITH([$1],
             AC_HELP_STRING([--with-$1=PROGRAM], [use PROGRAM as $1 executable]),
[case $withval in
yes)
  ;;
no)
  AS_VAR_SET(md_Program, missing)
  ;;
*)
  AS_VAR_SET(md_Program, ["$withval"])
  ;;
esac])
AS_IF([test "x[]AS_VAR_GET(md_Program)" != x],
      [AS_TR_CPP([$1])="AS_VAR_GET(md_Program)"],
      [AC_PATH_PROG(AS_TR_CPP([$1]), [$1], missing)])dnl
AS_VAR_POPDEF([md_Program])dnl
])# MD_PATH_PROG


# MD_PATH(PACKAGE, HEADER, LIBRARY, SYMBOL)
# -----------------------------------------
# Set PACKAGE_CPPFLAGS and PACKAGE_LIBS to the flags needed to compile/link with PACKAGE.  HEADER and LIBRARY
# should be a header file and a library, respectively, belonging to PACKAGE.  SYMBOL should be a SYMBOL in
# LIBRARY.  If the package is available, define HAVE_PACKAGE.
AC_DEFUN([MD_PATH],
[AS_VAR_PUSHDEF([md_Package], [md_path_have_$1])dnl
AS_VAR_PUSHDEF([md_Inc], [md_path_include_$1])dnl
AS_VAR_PUSHDEF([md_Lib], [md_path_lib_$1])dnl
AS_VAR_SET(md_Package, yes)
AS_VAR_SET(md_Inc, [])
AS_VAR_SET(md_Lib, [])
AC_ARG_WITH([$1],
             AC_HELP_STRING([--with-$1=DIR], [search $1 header files in DIR/include, library in DIR/lib]),
[case $withval in
yes)
  AS_VAR_SET(md_Package, yes)
  ;;
no)
  AS_VAR_SET(md_Package, no)
  ;;
*)
  AS_VAR_SET(md_Package, yes)
  AS_VAR_SET(md_Inc, ["$withval/include"])
  AS_VAR_SET(md_Lib, ["$withval/lib"])
  ;;
esac])
AC_ARG_WITH([$1-include],
             AC_HELP_STRING([--with-$1-include=DIR], [search $1 header files in DIR]),
[case $withval in
yes)
  AS_VAR_SET(md_Package, yes)
  ;;
no)
  AS_VAR_SET(md_Package, no)
  ;;
*)
  AS_VAR_SET(md_Package, yes)
  AS_VAR_SET(md_Inc, ["$withval"])
  ;;
esac])
AC_ARG_WITH([$1-lib],
             AC_HELP_STRING([--with-$1-lib=DIR], [search $1 library in DIR]),
[case $withval in
yes)
  AS_VAR_SET(md_Package, yes)
  ;;
no)
  AS_VAR_SET(md_Package, no)
  ;;
*)
  AS_VAR_SET(md_Package, yes)
  AS_VAR_SET(md_Lib, ["$withval"])
  ;;
esac])
AS_TR_CPP([$1]_CPPFLAGS)= AS_TR_CPP([$1]_LIBS)=
AS_IF([test AS_VAR_GET(md_Package) = yes],
[AS_IF([test "x[]AS_VAR_GET(md_Inc)" != x], [AS_TR_CPP([$1]_CPPFLAGS)="-I[]AS_VAR_GET(md_Inc)"])
AS_IF([test "x[]AS_VAR_GET(md_Lib)" != x], [AS_TR_CPP([$1]_LIBS)="-L[]AS_VAR_GET(md_Lib)"])
save_CPPFLAGS="$CPPFLAGS"; CPPFLAGS="$CPPFLAGS $[]AS_TR_CPP([$1]_CPPFLAGS)"
AC_CHECK_HEADER([$2],
[save_LDFLAGS="$LDFLAGS"; LDFLAGS="$LDFLAGS $[]AS_TR_CPP([$1]_LIBS)"
AC_CHECK_LIB([$3], [$4], [AS_TR_CPP([$1]_LIBS)="$[]AS_TR_CPP([$1]_LIBS) -l[$3]"], [AS_VAR_SET(md_Package, no)])
LDFLAGS="$save_LDFLAGS"], [AS_VAR_SET(md_Package, no)], [AC_INCLUDES_DEFAULT])
CPPFLAGS="$save_CPPFLAGS"])
AH_TEMPLATE(AS_TR_CPP(HAVE_[$1]), [Define if package `$1' is available.])dnl
AS_IF([test AS_VAR_GET(md_Package) = yes], [AC_DEFINE(AS_TR_CPP(HAVE_[$1]), [1])])dnl
AS_VAR_POPDEF([md_Lib])dnl
AS_VAR_POPDEF([md_Inc])dnl
AS_VAR_POPDEF([md_Package])dnl
])# MD_PATH


# MD_CHECK_TYPE_SYS_ERRLIST
# -------------------------
# Set SYS_ERRLIST_TYPE to the type of sys_errlist [0].
AC_DEFUN([MD_CHECK_TYPE_SYS_ERRLIST],
[AC_CACHE_CHECK([for type of sys_errlist elements], [md_cv_var_type_sys_errlist],
[md_cv_var_type_sys_errlist='char *'
for md_type_sys_errlist in 'const char *const' 'const char *' 'char *const'; do
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([AC_INCLUDES_DEFAULT],
                                     [extern $md_type_sys_errlist sys_errlist @<:@@:>@;])],
                    [md_cv_var_type_sys_errlist="$md_type_sys_errlist"; break])
done])
AC_DEFINE_UNQUOTED(SYS_ERRLIST_TYPE, [$md_cv_var_type_sys_errlist], [Define to the type of `sys_errlist' elements.])dnl
])# MD_CHECK_TYPE_SYS_ERRLIST


# MD_NLS
# ------
# Test for Native Language Support.  If supported, HAVE_NLS is defined and SUBDIR_PO is substituted with `po'.
# Otherwise, SUBDIR_PO is substituted with an empty string.  XGETTEXT, MSGMERGE, and MSGFMT are replaced by
# the paths of the respective programs.
#
AC_DEFUN([MD_NLS],
[AC_ARG_ENABLE([nls], AC_HELP_STRING([--disable-nls], [do not use Native Language Support]),
               [md_nls="$enableval"], [md_nls=yes])
AM_MISSING_PROG([XGETTEXT], [xgettext])
AM_MISSING_PROG([MSGMERGE], [msgmerge])
AM_MISSING_PROG([MSGFMT], [msgfmt])
if test "x$md_nls" != xyes && test "x$md_nls" != xno; then
  AC_MSG_WARN([unrecognized value for --enable-nls; disabling Native Language Support])
  md_nls=no
fi
SUBDIR_PO=
if test "x$md_nls" = xyes; then
  AC_CHECK_HEADERS([libintl.h],
      [AC_CHECK_DECLS([gettext, ngettext, bindtextdomain, textdomain], [], [md_nls=no], [AC_INCLUDES_DEFAULT
@%:@include <libintl.h>])], [md_nls=no], [AC_INCLUDES_DEFAULT])
  if test "x$md_nls" = xyes; then
      AC_CHECK_HEADERS([locale.h],
          [AC_CHECK_DECLS([setlocale, LC_MESSAGES], [], [md_nls=no], [AC_INCLUDES_DEFAULT
@%:@include <locale.h>])], [md_nls=no], [AC_INCLUDES_DEFAULT])
    if test "x$md_nls" = xyes; then
      AC_SEARCH_LIBS([ngettext], [intl], [], [md_nls=no])
    fi
  fi
  if test "x$md_nls" = xyes; then
    AC_DEFINE([HAVE_NLS], [1], [Define to 1 to enable Native Language Support.])
    SUBDIR_PO=po
  else
    AC_MSG_WARN([disabling Native Language Support])
  fi
fi[]dnl
AC_SUBST([SUBDIR_PO])dnl
])# MD_NLS
