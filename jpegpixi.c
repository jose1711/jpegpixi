/* This file is part of jpegpixi, a program to interpolate pixels in
   JFIF image files.
   Copyright (C) 2002, 2003, 2004, 2005 Martin Dickopp

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
#if HAVE_ERRNO_H
# include <errno.h>
#endif
#ifndef errno
extern int errno;
#endif

#if STDC_HEADERS
# include <ctype.h>
#else
# define isspace(c) ((c) == ' ' || (c) == '\t')
# define isdigit(c) ((c) >= '0' && (c) <= '9')
#endif

#if HAVE_NLS
# include <locale.h>
#endif

#include "jpegpixi.h"
#include "rbtree.h"
#include "optpixi.h"


/* Name of this program.  */
#define PROGRAM_NAME PACKAGE_NAME



/* Display text in response to the --help command line option.  */
extern void display_help_text (void);

/* Display text in response to the --version command line option.  */
extern void display_version_text (void);



static char *read_line (FILE *f, const char *filename, char **buffer_ptr, size_t *len_buffer_ptr,
                        unsigned int *lineno_ptr) gcc_attr_nonnull (());
static struct point_dimdir_s *parse_struct_point_dimdir_s (struct point_dimdir_s *point,
                                                           const char *str, unsigned int image_width,
                                                           unsigned int image_height) gcc_attr_nonnull (());
static int point_dimdir_s_validate_and_consolidate (void *a, const void *b) gcc_attr_nonnull (());



int
main (int argc, char *argv [])
{
    int i;
    enum method_t method = linear;
    const char *infilename, *outfilename;
    unsigned int image_width, image_height;
    struct rbtree points;



    /* Store program invocation name.  */
    invocation_name = argv [0] != 0 && *(argv [0]) != '\0' ? argv [0] : PROGRAM_NAME;


    /* Set up Native Language Support.  */
#if HAVE_NLS
    if (setlocale (LC_ALL, "") != 0)
    {
        if (bindtextdomain (PACKAGE_TARNAME, LOCALEDIR) == 0 || textdomain (PACKAGE_TARNAME) == 0)
            mem_alloc_failed ();
    }
    else
    {
        fprintf (stderr, "%s: cannot set locale\n"
                 "Make sure the `LC_*' and `LANG' environment variables are set to valid values.\n", invocation_name);
        exit (1);
    }
#endif


    /* Parse command line options.  */
    if ((i = parse_options (invocation_name, argc, argv)) == -1)
        goto error_try_help;

    if (opt_help)
    {
        display_help_text ();
        exit (0);
    }

    if (opt_version)
    {
        display_version_text ();
        exit (0);
    }

    if (opt_method)
    {
        if (strcmp ("0", arg_method) == 0 || strcmp ("av", arg_method) == 0
            || strcmp ("average", arg_method) == 0)
            method = average;
        else if (strcmp ("1", arg_method) == 0 || strcmp ("li", arg_method) == 0
                 || strcmp ("linear", arg_method) == 0)
            method = linear;
        else if (strcmp ("2", arg_method) == 0 || strcmp ("qu", arg_method) == 0
                 || strcmp ("quadratic", arg_method) == 0)
            method = quadratic;
        else if (strcmp ("3", arg_method) == 0 || strcmp ("cu", arg_method) == 0
                 || strcmp ("cubic", arg_method) == 0)
            method = cubic;
        else
        {
            fprintf (stderr, _("%s: invalid interpolation method `%s'\n"), invocation_name, arg_method);
            goto error_try_help;
        }
    }


    /* Read filenames from command line.  */
    if (i < argc)
        infilename = argv [i++];
    else
    {
        fprintf (stderr, _("%s: source filename missing\n"), invocation_name);
      error_try_help:
        fprintf (stderr, _("Try `%s --help' for more information.\n"), invocation_name);
        exit (1);
    }

    if (i < argc)
        outfilename = argv [i++];
    else
    {
        fprintf (stderr, _("%s: destination filename missing\n"), invocation_name);
        goto error_try_help;
    }


    /* Open files and read header.  */
    init_files (strcmp (infilename, "-")  != 0 ? infilename  : 0,
                strcmp (outfilename, "-") != 0 ? outfilename : 0,
                &image_width, &image_height);


    /* Read pixel block specifications from a file.  */
    rbtree_create (&points);

    if (opt_blocks_file)
    {
        FILE *f;
        size_t len_line = 80;
        char *line = xmalloc (len_line);
        unsigned int lineno = 0;

        if ((f = fopen (arg_blocks_file, "r")) == 0)
        {
            fprintf (stderr, _("%s: %s: %s\n"), invocation_name, arg_blocks_file, strerror (errno));
            exit (1);
        }

        while (read_line (f, arg_blocks_file, &line, &len_line, &lineno))
        {
            struct point_dimdir_s new;

            if (parse_struct_point_dimdir_s (&new, line, image_width, image_height) != 0)
            {
                struct point_dimdir_s *old;
                int exists;

                if ((old = rbtree_insert (&points, &new, sizeof new, point_s_cmp, &exists)) == 0)
                    mem_alloc_failed ();
                if (exists)
                    fprintf (stderr, _("%s: %s:%u: %u,%u,%u,%u ignored: overlaps with %u,%u,%u,%u\n"),
                             invocation_name, arg_blocks_file, lineno,
                             new.p.x, new.p.y, new.p.x_size, new.p.y_size,
                             old->p.x, old->p.y, old->p.x_size, old->p.y_size);
            }
            else
            {
                fprintf (stderr, _("%s: %s:%u: invalid pixel block specification ignored\n"),
                         invocation_name, arg_blocks_file, lineno);
            }
        }

        if (fclose (f) == EOF)
        {
            fprintf (stderr, _("%s: %s: %s\n"), invocation_name, arg_blocks_file, strerror (errno));
            exit (1);
        }

        free (line);
    }


    /* Read pixel block specifications from the command line.  */
    while (i < argc)
    {
        struct point_dimdir_s new;

        if (parse_struct_point_dimdir_s (&new, argv [i], image_width, image_height) != 0)
        {
            struct point_dimdir_s *old;
            int exists;

            if ((old = rbtree_insert (&points, &new, sizeof new, point_s_cmp, &exists)) == 0)
                mem_alloc_failed ();
            if (exists)
                fprintf (stderr, _("%s: %u,%u,%u,%u ignored: overlaps with %u,%u,%u,%u\n"), invocation_name,
                         new.p.x, new.p.y, new.p.x_size, new.p.y_size,
                         old->p.x, old->p.y, old->p.x_size, old->p.y_size);
        }
        else
        {
            fprintf (stderr, _("%s: invalid pixel block specification `%s'\n"), invocation_name, argv [i]);
            goto error_try_help;
        }
        ++i;
    }


    /* Consolidate pixels.  */
    consolidate_pixels (&points, point_dimdir_s_validate_and_consolidate);


    /* Interpolate pixels in specified file.  */
    init_interpolator ();
    process_file (&points, method);

    exit (0);
}



/* Read a line from a file.  Dynamically resize buffer if necessary.
   Return pointer to the line, or a null pointer at the end of the file.  */
static char *
read_line (FILE *const f, const char *const filename, char **const buffer_ptr, size_t *const len_buffer_ptr,
           unsigned int *const lineno_ptr)
{
    size_t i = 0;
    int line_nonempty = 0, no_comment = 1;

    ++*lineno_ptr;

    while (1)
    {
#if HAVE_FGETC_UNLOCKED
        const int c = fgetc_unlocked (f);
#else
        const int c = fgetc (f);
#endif

        if (i >= *len_buffer_ptr)
            *buffer_ptr = xrealloc (*buffer_ptr, *len_buffer_ptr *= 2);

        if (c == EOF)
        {
            if (ferror (f))
            {
                fprintf (stderr, _("%s: %s: %s\n"), invocation_name, filename, strerror (errno));
                exit (1);
            }

            (*buffer_ptr) [i] = '\0';
            return line_nonempty ? *buffer_ptr : 0;
        }
        else if (c == '\n')
        {
            if (line_nonempty)
            {
                (*buffer_ptr) [i] = '\0';
                return *buffer_ptr;
            }
            else
            {
                i = 0;
                no_comment = 1;
                ++*lineno_ptr;
            }
        }
        else if (c == '#')
            no_comment = 0;
        else if (no_comment && c != '\0')
        {
            (*buffer_ptr) [i++] = c;
            if (!isspace (c))
                line_nonempty = 1;
        }
    }
}



/* Parse coordinate and size specification, and dimension and direction of
   the interpolation. Return `point' on succes, 0 otherwise.  */
static struct point_dimdir_s *
parse_struct_point_dimdir_s (struct point_dimdir_s *const point, const char *str,
                             const unsigned int image_width, const unsigned int image_height)
{
    while (isspace ((unsigned char)*str))
        ++str;

    if (*str == 'h' || *str == 'H')
        point->d = horizontal;
    else if (*str == 'v' || *str == 'V')
        point->d = vertical;
    else
    {
        point->d = twodim;
        if (*str != '2' || isdigit ((unsigned char)str [1]) || str [1] == '.' || str [1] == '%'
            || str [1] == ',' || str [1] == '/' || str [1] == ';')
            goto parse_x;
    }
    ++str;

    while (isspace ((unsigned char)*str))
        ++str;

    if (*str == ':')
        ++str;
    else if (point->d == twodim && (*str == ',' || *str == '/' || *str == ';'))
    {
        ++str;
        point->p.x = 2;
        goto parse_y;
    }
    else
        return 0;

    while (isspace ((unsigned char)*str))
        ++str;

  parse_x:
    switch (parse_number (&str, &point->p.x))
    {
      case -1:
        return 0;
      case 0:
        point->p.x = REL_TO_ABS (point->p.x, image_width);
    }

    while (isspace ((unsigned char)*str))
        ++str;

    if (*str == ',' || *str == '/' || *str == ';')
        ++str;
    else
        return 0;

  parse_y:
    while (isspace ((unsigned char)*str))
        ++str;

    switch (parse_number (&str, &point->p.y))
    {
      case -1:
        return 0;
      case 0:
        point->p.y = REL_TO_ABS (point->p.y, image_height);
    }

    while (isspace ((unsigned char)*str))
        ++str;

    if (*str == ',' || *str == '/' || *str == ';')
        ++str;
    else if (*str == '\0')
    {
        point->p.x_size = 1;
        point->p.y_size = 1;
        return point;
    }
    else
        return 0;

    while (isspace ((unsigned char)*str))
        ++str;

    switch (parse_number (&str, &point->p.x_size))
    {
      case -1:
        return 0;
      case 0:
        point->p.x_size = REL_TO_ABS (point->p.x_size, image_width);
    }
    if (point->p.x_size == 0)
        return 0;

    while (isspace ((unsigned char)*str))
        ++str;

    if (*str == ',' || *str == '/' || *str == ';')
        ++str;
    else if (*str == '\0')
    {
        point->p.y_size = point->p.x_size;
        return point;
    }
    else
        return 0;

    while (isspace ((unsigned char)*str))
        ++str;

    switch (parse_number (&str, &point->p.y_size))
    {
      case -1:
        return 0;
      case 0:
        point->p.y_size = REL_TO_ABS (point->p.y_size, image_height);
    }
    if (point->p.y_size == 0)
        return 0;

    while (isspace ((unsigned char)*str))
        ++str;

    return *str == '\0' ? point : 0;
}



/* Validate and consolidate two point structures.  */
static int
point_dimdir_s_validate_and_consolidate (void *const a, const void *const b)
{
    struct point_dimdir_s *const point_a = a;
    const struct point_dimdir_s *const point_b = b;

    if (point_a->d != point_b->d)
    {
        fprintf (stderr, _("%s: incompatible adjacent points %u,%u,%u,%u (%s) and %u,%u,%u,%u (%s) ignored\n"),
                 invocation_name, point_b->p.x, point_b->p.y, point_b->p.x_size, point_b->p.y_size,
                 point_b->d == vertical ? _("vertical") : point_b->d == horizontal ? _("horizontal") : _("2-dim"),
                 point_a->p.x, point_a->p.y, point_a->p.x_size, point_a->p.y_size,
                 point_a->d == vertical ? _("vertical") : point_a->d == horizontal ? _("horizontal") : _("2-dim"));
        return 0;
    }

    return 1;
}
