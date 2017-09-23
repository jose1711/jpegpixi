/* This file is part of jpegpixi, a program to interpolate pixels in
   JFIF image files.
   Copyright (C) 2002, 2003, 2004 Martin Dickopp

   Jpegpixi is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2 of the License,
   or (at your option) any later version.

   Jpegpixi is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with jpegpixi; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301,
   USA.  */

#ifndef UTIL_H
#define UTIL_H 1


#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#if STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# if HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#if HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#else
# if HAVE_STRINGS_H
#  include <strings.h>
# endif
#endif
#if HAVE_INTTYPES_H
# include <inttypes.h>
#else
# if HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif


#if !HAVE_DECL_STRCHR
extern char *strchr (), *strrchr ();
#endif

#if HAVE_STRERROR
# if !HAVE_DECL_STRERROR
extern char *strerror ();
# endif
#else
extern char *strerror (int n);
#endif


#if !HAVE_SIZE_T
typedef unsigned int size_t;
#endif

#if !HAVE_SSIZE_T
typedef int ssize_t;
#endif

#if !HAVE_PTRDIFF_T
typedef int ptrdiff_t;
#endif


/* Native Language Support.  */
#undef _
#if HAVE_NLS
# include <libintl.h>
# define _(str) gettext (str)
#else
# define _(str) (str)
#endif


/* GCC attributes.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5)
# define gcc_attr_noreturn __attribute__ ((noreturn))
# define gcc_attr_const    __attribute__ ((const))
#else
# define gcc_attr_noreturn /* empty */
# define gcc_attr_const    /* empty */
#endif
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
# define gcc_attr_pure     __attribute__ ((pure))
# define gcc_attr_malloc   __attribute__ ((malloc))
#else
# define gcc_attr_pure     /* empty */
# define gcc_attr_malloc   /* empty */
#endif
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)
# define gcc_attr_nonnull(x) __attribute__ ((nonnull x))
#else
# define gcc_attr_nonnull(x) /* empty */
#endif


/* Number of elements in an array.  */
#define numof(a) (sizeof (a) / sizeof *(a))

/* Minimum and maximum.  */
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

/* Clamp a value between a minimum and a maximum.  */
#define CLAMP(v,lo,hi) ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v))

/* Do two lines overlap?  */
#define OVERLAP1D(x1,w1,x2,w2) ((x1) + (w1) > (x2) && (x1) < (x2) + (w2))

/* Do two rectangles overlap?  */
#define OVERLAP2D(x1,y1,w1,h1,x2,y2,w2,h2) (OVERLAP1D ((x1), (w1), (x2), (w2)) && OVERLAP1D ((y1), (h1), (y2), (h2)))


/* Name under which the program has been invoked.  */
extern const char *invocation_name;


/* malloc and realloc wrappers.  Terminate the program if memory allocation fails.  */
extern void *xmalloc (size_t size) gcc_attr_malloc;
extern void *xrealloc (void *ptr, size_t size);

/* Report a failure to allocate memory and terminate the program.  */
extern void mem_alloc_failed (void) gcc_attr_noreturn;


#endif
