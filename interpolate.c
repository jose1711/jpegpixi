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

#if HAVE_MATH_H
# include <math.h>
#else
double fabs ();
#endif

#if HAVE_FLOAT_H
# include <float.h>
#endif
#ifndef DBL_MIN
# define DBL_MIN 1e-37
#endif
#ifndef DBL_MAX
# define DBL_MAX 1e37
#endif

#include "rbtree.h"
#include "jpegpixi.h"
#include "optpixi.h"



/* Check if downsampled coordinates are valid.  */
#define XY_VALID(xx,yy) \
    ((xx) >= 0 && (xx) < (int)comp->downsampled_width && (yy) >= 0 && (yy) < (int)comp->downsampled_height)



/* Tree of weight data blocks.  */
struct rbtree weight_s_tree;



static void set_weighted_pixels (struct jfif *jfif, const jpeg_component_info *comp, int icomp,
                                 int x_pos, int y_pos, int x_size, int y_size, enum dimdir_t d, int poly_order);
static int weight_s_cmp (const void *a, const void *b) gcc_attr_pure gcc_attr_nonnull (());



/* Initialize interpolator.  */
void
init_interpolator (void)
{
    rbtree_create (&weight_s_tree);
}



/* Interpolate a pixel block.  */
void
interpolate (const struct jpeg_decompress_struct *const jpg, struct jfif *const jfif,
             const struct point_dimdir_s *const point, const enum method_t method)
{
    int icomp;


    /* Iterate over components.  */
    for (icomp = 0; icomp < jpg->num_components; ++icomp)
    {
        const jpeg_component_info *const comp = &(jpg->comp_info [icomp]);

        /* Downsampled position of the pixel block.  */
        const int x_pos = point->p.x * comp->h_samp_factor / jpg->max_h_samp_factor;
        const int y_pos = point->p.y * comp->v_samp_factor / jpg->max_v_samp_factor;

        /* Downsampled size of the pixel block.  */
        const int x_size = ((point->p.x_size + jpg->max_h_samp_factor / comp->h_samp_factor - 1)
                            / (jpg->max_h_samp_factor / comp->h_samp_factor));
        const int y_size = ((point->p.y_size + jpg->max_v_samp_factor / comp->v_samp_factor - 1)
                            / (jpg->max_v_samp_factor / comp->v_samp_factor));


        /* Show pixel block.  */
        if (opt_verbose && icomp == 0)
            fprintf (stderr, _("%s: interpolating %u,%u,%u,%u (%s)\n"), invocation_name,
                     point->p.x, point->p.y, point->p.x_size, point->p.y_size,
                     point->d == vertical ? _("vertical") : point->d == horizontal ? _("horizontal") : _("2-dim"));


        /* Interpolate pixel.  */
        switch (method)
        {
          case average:
            /* Average of adjacent pixels.  */
            switch (point->d)
            {
              case vertical:
                {
                    int x;

                    for (x = 0; x < x_size; ++x)
                    {
                        int sum_weight = 0;
                        double v = 0.0;

                        if (XY_VALID (x_pos + x, y_pos - 1))
                        {
                            v += get_pixel (jfif, icomp, x_pos + x, y_pos - 1);
                            ++sum_weight;
                        }
                        if (XY_VALID (x_pos + x, y_pos + y_size))
                        {
                            v += get_pixel (jfif, icomp, x_pos + x, y_pos + y_size);
                            ++sum_weight;
                        }

                        if (sum_weight > 0)
                        {
                            int y;

                            v /= (double)sum_weight;

                            for (y = 0; y < y_size; ++y)
                                if (XY_VALID (x_pos + x, y_pos + y))
                                    set_pixel (jfif, icomp, x_pos + x, y_pos + y, v);
                        }
                    }
                }
                break;

              case horizontal:
                {
                    int y;

                    for (y = 0; y < y_size; ++y)
                    {
                        int sum_weight = 0;
                        double v = 0.0;

                        if (XY_VALID (x_pos - 1, y_pos + y))
                        {
                            v += get_pixel (jfif, icomp, x_pos - 1, y_pos + y);
                            ++sum_weight;
                        }
                        if (XY_VALID (x_pos + x_size, y_pos + y))
                        {
                            v += get_pixel (jfif, icomp, x_pos + x_size, y_pos + y);
                            ++sum_weight;
                        }

                        if (sum_weight > 0)
                        {
                            int x;

                            v /= (double)sum_weight;

                            for (x = 0; x < x_size; ++x)
                                if (XY_VALID (x_pos + x, y_pos + y))
                                    set_pixel (jfif, icomp, x_pos + x, y_pos + y, v);
                        }
                    }
                }
                break;

              case twodim:
                {
                    int sum_weight = 0;
                    double v = 0.0;
                    int x, y;

                    for (x = 0; x < x_size; ++x)
                    {
                        if (XY_VALID (x_pos + x, y_pos - 1))
                        {
                            v += get_pixel (jfif, icomp, x_pos + x, y_pos - 1);
                            ++sum_weight;
                        }
                        if (XY_VALID (x_pos + x, y_pos + y_size))
                        {
                            v += get_pixel (jfif, icomp, x_pos + x, y_pos + y_size);
                            ++sum_weight;
                        }
                    }

                    for (y = 0; y < y_size; ++y)
                    {
                        if (XY_VALID (x_pos - 1, y_pos + y))
                        {
                            v += get_pixel (jfif, icomp, x_pos - 1, y_pos + y);
                            ++sum_weight;
                        }
                        if (XY_VALID (x_pos + x_size, y_pos + y))
                        {
                            v += get_pixel (jfif, icomp, x_pos + x_size, y_pos + y);
                            ++sum_weight;
                        }
                    }

                    if (sum_weight > 0)
                    {
                        v /= (double)sum_weight;

                        for (x = 0; x < x_size; ++x)
                            for (y = 0; y < y_size; ++y)
                                if (XY_VALID (x_pos + x, y_pos + y))
                                    set_pixel (jfif, icomp, x_pos + x, y_pos + y, v);
                    }
                }
            }
            break;

          case linear:
            /* (Bi)linear interpolation.  */
            set_weighted_pixels (jfif, comp, icomp, x_pos, y_pos, x_size, y_size, point->d, 1);
            break;

          case quadratic:
            /* (Bi)quadratic interpolation.  */
            set_weighted_pixels (jfif, comp, icomp, x_pos, y_pos, x_size, y_size, point->d, 2);
            break;

          case cubic:
            /* (Bi)cubic interpolation.  */
            set_weighted_pixels (jfif, comp, icomp, x_pos, y_pos, x_size, y_size, point->d, 3);
        }
    }
}



static void
set_weighted_pixels (struct jfif *const jfif, const jpeg_component_info *const comp, const int icomp,
                     const int x_pos, const int y_pos, const int x_size, const int y_size,
                     const enum dimdir_t d, const int poly_order)
{
    const struct weight_s *w;
    int x, y;


    /* Obtain weight data block.  */
    {
        struct weight_s tmp;

        /* In the one-dimensional case, `x_size' must be 1, and the size must be stored in `y_size'.  */
        tmp.x_size = (d == twodim ? x_size : 1);
        tmp.y_size = (d == horizontal ? x_size : y_size);
        tmp.poly_order = poly_order;
        tmp.is_twodim = d == twodim;

        w = rbtree_find (&weight_s_tree, &tmp, weight_s_cmp);

        if (w == 0)
        {
            struct weight_s *const new_w = get_weights (tmp.x_size, tmp.y_size, tmp.is_twodim, poly_order);

            if ((w = rbtree_insert (&weight_s_tree, new_w, sizeof *new_w, weight_s_cmp, 0)) == 0)
                mem_alloc_failed ();
            free (new_w);
        }
    }


    /* Calculate weighted pixel sum.  */
    for (y = 0; y < y_size; ++y)
        for (x = 0; x < x_size; ++x)
            if (XY_VALID (x_pos + x, y_pos + y))
            {
                const double *const weights = w->weights [d == vertical ? y : d == horizontal ? x : x + x_size * y];
                double sum_weight = 0.0, v = 0.0;
                size_t i;

                for (i = 0; i < w->num_pos; ++i)
                {
                    /* In the one-dimensional case, only the y coordinate is used.  */
                    const int xx = x_pos + (d == vertical   ? x : d == horizontal ? w->pos [i].y : w->pos [i].x);
                    const int yy = y_pos + (d == horizontal ? y : w->pos [i].y);

                    if (XY_VALID (xx, yy))
                    {
                        const double weight = weights [i];

                        v += weight * get_pixel (jfif, icomp, xx, yy);
                        sum_weight += weight;
                    }
                }

                if (fabs (v) <= DBL_MIN)
                    set_pixel (jfif, icomp, x_pos + x, y_pos + y, 0.0);
                else if (sum_weight >= DBL_MIN)
                    set_pixel (jfif, icomp, x_pos + x, y_pos + y, v / sum_weight);
                else if (v >= 0.0)
                    set_pixel (jfif, icomp, x_pos + x, y_pos + y, DBL_MAX);
                else
                    set_pixel (jfif, icomp, x_pos + x, y_pos + y, -DBL_MAX);
            }
}



/* Compare two weight data blocks.  */
static int
weight_s_cmp (const void *const a, const void *const b)
{
    const struct weight_s *const weight_s_a = a;
    const struct weight_s *const weight_s_b = b;

    if (weight_s_a->is_twodim < weight_s_b->is_twodim)
        return -1;
    else if (weight_s_a->is_twodim > weight_s_b->is_twodim)
        return 1;
    else if (weight_s_a->poly_order < weight_s_b->poly_order)
        return -1;
    else if (weight_s_a->poly_order > weight_s_b->poly_order)
        return 1;
    else if (weight_s_a->x_size < weight_s_b->x_size)
        return -1;
    else if (weight_s_a->x_size > weight_s_b->x_size)
        return 1;
    else if (weight_s_a->y_size < weight_s_b->y_size)
        return -1;
    else if (weight_s_a->y_size > weight_s_b->y_size)
        return 1;
    else
        return 0;
}
