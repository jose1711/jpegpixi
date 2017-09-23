/* This file is part of jpegpixi, a program to interpolate pixels in
   JFIF image files.
   Copyright (C) 2002 Martin Dickopp

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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "util.h"

#include <stdio.h>

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if !STDC_HEADERS
# if HAVE_MALLOC_H
#  include <malloc.h>
# else
extern void *malloc (), *realloc ();
# endif
#endif

#ifndef STDERR_FILENO
# define STDERR_FILENO 2
#endif



/* Name under which the program has been invoked.  */
const char *invocation_name;



/* malloc wrapper.  Terminates the program if memory allocation fails.  */
void *
xmalloc (const size_t size)
{
    void *const ptr = malloc (size);
    if (ptr == 0)
        mem_alloc_failed ();
    return ptr;
}



/* realloc wrapper.  Terminates the program if memory allocation fails.  */
void *
xrealloc (void *ptr, const size_t size)
{
    ptr = realloc (ptr, size);
    if (ptr == 0)
        mem_alloc_failed ();
    return ptr;
}



/* Report a failure to allocate memory and terminate the program.
   This function must not do anything which could cause further memory
   allocation attempts, including not using `fprintf'.  */
void
mem_alloc_failed (void)
{
    {
        const char *ptr = invocation_name;
        size_t len = strlen (ptr);

        while (len > 0)
        {
            const ssize_t chars_written = write (STDERR_FILENO, ptr, len);
            /* If writing to standard error fails, terminate right away.  */
            if (chars_written == -1)
                exit (1);
            len -= (size_t)chars_written;
            ptr += (ptrdiff_t)chars_written;
        }
    }

    {
        static const char message [] = ": out of memory\n";
        const char *ptr = message;
        size_t len = numof (message) - 1;

        while (len > 0)
        {
            const ssize_t chars_written = write (STDERR_FILENO, ptr, len);
            /* If writing to standard error fails, terminate right away.  */
            if (chars_written == -1)
                exit (1);
            len -= (size_t)chars_written;
            ptr += (ptrdiff_t)chars_written;
        }
    }

    exit (1);
}
