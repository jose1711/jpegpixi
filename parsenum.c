/* This file is part of jpegpixi, a program to interpolate pixels in
   JFIF image files.
   Copyright (C) 2003, 2004 Martin Dickopp

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

#if STDC_HEADERS
# include <ctype.h>
#else
# define isdigit(c) ((c) >= '0' && (c) <= '9')
#endif

#include "jpegpixi.h"



/* Parse a number.  Return 1 if it is absolute, 0 if it is relative, -1 in case of an error.  */
int
parse_number (const char **const strptr, unsigned int *const numptr)
{
    if (!isdigit ((unsigned char)**strptr) && **strptr != '.')
        return -1;

    *numptr = 0;

    while (isdigit ((unsigned char)**strptr))
    {
        *numptr = *numptr * 10 + (*(*strptr)++ - '0');
        if (*numptr >= DENOM)
            return -1;
    }

    if (**strptr == '%' || **strptr == '.')
    {
        ++*strptr;
        if (*numptr > 100)
            return -1;

        *numptr *= (DENOM / 100);

        if (*(*strptr - 1) == '.')
        {
            unsigned int value = DENOM / 1000;

            while (isdigit ((unsigned char)**strptr))
            {
                *numptr += value * (*(*strptr)++ - '0');
                value /= 10;
            }

            if (**strptr == '%')
                ++*strptr;
            else
                return -1;

            if (*numptr > DENOM)
                return -1;
        }

        return 0;
    }

    return 1;
}
