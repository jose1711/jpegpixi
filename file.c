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

#include "jpegpixi.h"
#include "rbtree.h"
#include "optpixi.h"



/* File names (needed by display_error_message).  */
static const char *infilename1, *outfilename1;

/* Files.  */
static FILE *infile, *outfile;

/* Jpeglib objects.  */
static struct jpeg_error_mgr errmgr;
static struct jpeg_decompress_struct injpg;
static struct jpeg_compress_struct outjpg;



static void display_error_message (j_common_ptr jpg);



/* Open files and read header of input file.  */
void
init_files (const char *const infilename, const char *const outfilename,
            unsigned int *const image_width, unsigned int *const image_height)
{
    /* Initialize jpeglib objects.  */
    infilename1 = infilename;
    outfilename1 = outfilename;

    jpeg_std_error (&errmgr);
    errmgr.output_message = &display_error_message;

    jpeg_create_decompress (&injpg);
    injpg.err = &errmgr;
    jpeg_create_compress (&outjpg);
    outjpg.err = &errmgr;


    /* Open input file.  */
    if (infilename != 0)
    {
        if ((infile = fopen (infilename, "rb")) == 0)
        {
            fprintf (stderr, _("%s: %s: %s\n"), invocation_name, infilename, strerror (errno));
            exit (1);
        }
    }
    else
        infile = stdin;

    jpeg_stdio_src (&injpg, infile);


    /* Save all markers, unless file is to be stripped.  */
    if (!opt_strip)
    {
        int i;

        jpeg_save_markers (&injpg, JPEG_COM, 0xffff);
        for (i = 0; i < 16; ++i)
            jpeg_save_markers (&injpg, JPEG_APP0 + i, 0xffff);
    }


    /* Read header, and display information about the image.  */
    jpeg_read_header (&injpg, TRUE);

    if (injpg.image_width <= 0 || injpg.image_height <= 0)
    {
        fprintf (stderr, _("%s: %s: Image has zero size\n"), invocation_name, infilename);
        exit (1);
    }

    *image_width = injpg.image_width;
    *image_height = injpg.image_height;

    if (opt_info)
    {
        /* TRANSLATORS: Please use the "multiplication sign" (Unicode character <U00D7>)
           in place of the "x". */
        fprintf (stderr, _("%s: %s: size %ux%u, colorspace "), invocation_name,
                 infilename != 0 ? infilename : _("STDIN"),
                 (unsigned int)injpg.image_width, (unsigned int)injpg.image_height);

        switch (injpg.jpeg_color_space)
        {
          case JCS_GRAYSCALE:
            fputs (_("grayscale"), stderr);
            break;

          case JCS_RGB:
            fputs ("RGB", stderr);
            break;

          case JCS_YCbCr:
            fputs ("YUV", stderr);
            break;

          case JCS_CMYK:
            fputs ("CMYK", stderr);
            break;

          case JCS_YCCK:
            fputs ("YUVK", stderr);
            break;

          default:
            fputs (_("unknown"), stderr);
        }

        fputs (_(", sampling"), stderr);

        {
            int icomp;

            for (icomp = 0; icomp < injpg.num_components; ++icomp)
            {
                const jpeg_component_info *const comp = &(injpg.comp_info [icomp]);

                /* TRANSLATORS: Please use the "multiplication sign" (Unicode character <U00D7>)
                   in place of the "x". */
                fprintf (stderr, _(" %dx%d"), comp->h_samp_factor, comp->v_samp_factor);
            }
        }

        putc ('\n', stderr);
    }


    /* Create output file.  */
    if (outfilename != 0)
    {
        if ((outfile = fopen (outfilename, "wb")) == 0)
        {
            fprintf (stderr, _("%s: %s: %s\n"), invocation_name, outfilename, strerror (errno));
            exit (1);
        }
    }
    else
        outfile = stdout;

    jpeg_stdio_dest (&outjpg, outfile);
}



/* Copy a JFIF image file to another, interpolate pixels.  */
void
process_file (const struct rbtree *const points, const enum method_t method)
{
    jvirt_barray_ptr *coeff;


    /* Copy DCT coefficients from input to output file.  */
    coeff = jpeg_read_coefficients (&injpg);
    jpeg_copy_critical_parameters (&injpg, &outjpg);
    outjpg.optimize_coding = TRUE;
    jpeg_write_coefficients (&outjpg, coeff);


    /* Copy saved markers from input to output file, unless file is to be stripped.  */
    if (!opt_strip)
    {
        jpeg_saved_marker_ptr marker;

        for (marker = injpg.marker_list; marker != 0; marker = marker->next)
        {
            /* JFIF and Adobe markers are taken care of by the library, so don't write duplicates.  */
            if ((!outjpg.write_JFIF_header || marker->marker != JPEG_APP0 || marker->data_length < 5
                 || GETJOCTET (marker->data [0]) != 0x4a || GETJOCTET (marker->data [1]) != 0x46
                 || GETJOCTET (marker->data [2]) != 0x49 || GETJOCTET (marker->data [3]) != 0x46
                 || GETJOCTET (marker->data [4]) != 0)
                && (!outjpg.write_Adobe_marker || marker->marker != JPEG_APP0 + 14 || marker->data_length < 5
                    || GETJOCTET (marker->data [0]) != 0x41 || GETJOCTET (marker->data [1]) != 0x64
                    || GETJOCTET (marker->data [2]) != 0x6f || GETJOCTET (marker->data [3]) != 0x62
                    || GETJOCTET (marker->data [4]) != 0x65))
                jpeg_write_marker (&outjpg, marker->marker, marker->data, marker->data_length);
        }
    }


    /* Interpolate pixels.  */
    {
        struct point_dimdir_s *point = rbtree_first (points);

        if (point != 0)
        {
            struct jfif *const jfif = jfif_init (&injpg, coeff);

            while (point != 0)
            {
                if (point->p.x + point->p.x_size <= injpg.image_width
                    && point->p.y + point->p.y_size <= injpg.image_height
                    && point->p.x_size <= injpg.image_width
                    && point->p.y_size <= injpg.image_height)
                    interpolate (&injpg, jfif, point, method);
                else
                    /* TRANSLATORS: Please use the "multiplication sign" (Unicode character <U00D7>)
                       in place of the "x". */
                    fprintf (stderr, _("%s: %s: %u,%u,%u,%u ignored: Image size %ux%u\n"),
                             invocation_name, infilename1 != 0 ? infilename1 : _("STDIN"),
                             point->p.x, point->p.y, point->p.x_size, point->p.y_size,
                             (unsigned int)injpg.image_width, (unsigned int)injpg.image_height);

                point = rbtree_next (point);
            }

            jfif_fini (jfif);
        }
    }


    /* Finish processing and close files.  */
    jpeg_finish_compress (&outjpg);
    jpeg_finish_decompress (&injpg);

    if (outfilename1 != 0 && fclose (outfile) == EOF)
    {
        fprintf (stderr, _("%s: %s: %s\n"), invocation_name, outfilename1, strerror (errno));
        exit (1);
    }

    if (infilename1 != 0 && fclose (infile) == EOF)
    {
        fprintf (stderr, _("%s: %s: %s\n"), invocation_name, infilename1, strerror (errno));
        exit (1);
    }


    /* Delete jpeglib objects.  */
    jpeg_destroy_decompress (&injpg);
    jpeg_destroy_compress (&outjpg);
}



/* Display an error message.  */
static void
display_error_message (const j_common_ptr jpg)
{
    const char *filename;
    char buffer [JMSG_LENGTH_MAX];


    if (jpg->is_decompressor)
        filename = infilename1 != 0 ? infilename1 : _("STDIN");
    else
        filename = outfilename1 != 0 ? outfilename1 : _("STDOUT");

    (*jpg->err->format_message) (jpg, buffer);
    fprintf (stderr, _("%s: %s: %s\n"), invocation_name, filename, buffer);
}
