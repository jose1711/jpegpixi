/* This file is part of jpegpixi, a program to interpolate pixels in
   JFIF image files.
   Copyright (C) 2002, 2003 Martin Dickopp

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

#include "jpegpixi.h"



static double poly_term (size_t i_coeff, int x, int y, int poly_order) gcc_attr_const;
static void matrix_inv (double *a, size_t size);



/* Calculate weights for interpolation.  */
struct weight_s *
get_weights (const int x_size, const int y_size, const int is_twodim, const int poly_order)
{
    struct weight_s *w;
    const size_t num_coeff = (is_twodim
                              ? ((size_t)poly_order + 1) * ((size_t)poly_order + 1)
                              : (size_t)poly_order + 1);
    double *matrix;  /* num_coeff * num_coeff */
    double *vector0; /* num_pos   * num_coeff */
    double *vector1; /* num_pos   * num_coeff */
    size_t ix, iy, j;
    int x, y;


    w = xmalloc (sizeof (struct weight_s));
    w->x_size = x_size;
    w->y_size = y_size;
    w->poly_order = poly_order;
    w->is_twodim = is_twodim;


    /* Determine coordinates of pixels to be sampled.  */
    w->num_pos = 0;

    if (is_twodim)
    {
        size_t len_pos = 8;

        w->pos = xmalloc (sizeof (struct xy_s) * len_pos);

        for (y = -poly_order; y < y_size + poly_order; ++y)
            for (x = -poly_order; x < x_size + poly_order; ++x)
                if ((x < 0 && y < 0 && -x - y < poly_order + 2)
                    || (x < 0 && y >= y_size && -x + y - y_size < poly_order + 1)
                    || (x >= x_size && y < 0 && x - y - x_size < poly_order + 1)
                    || (x >= x_size && y >= y_size && x + y - x_size - y_size < poly_order)
                    || (x < 0 && y >= 0 && y < y_size) || (x >= x_size && y >= 0 && y < y_size)
                    || (y < 0 && x >= 0 && x < x_size) || (y >= y_size && x >= 0 && x < x_size))
                {
                    if (w->num_pos >= len_pos)
                        w->pos = xrealloc (w->pos, sizeof (struct xy_s) * (len_pos *= 2));

                    w->pos [w->num_pos].x = x;
                    w->pos [w->num_pos++].y = y;
                }

        w->pos = xrealloc (w->pos, sizeof (struct xy_s) * w->num_pos);
    }
    else
    {
        /* In the one-dimensional case, only the y coordinate and y size is used.  */
        w->pos = xmalloc (sizeof (struct xy_s) * 2 * poly_order);

        for (y = -poly_order; y < 0; ++y)
        {
            w->pos [w->num_pos].x = 0;
            w->pos [w->num_pos++].y = y;
        }

        for (y = y_size; y < y_size + poly_order; ++y)
        {
            w->pos [w->num_pos].x = 0;
            w->pos [w->num_pos++].y = y;
        }
    }


    /* Allocate memory.  */
    matrix = xmalloc (sizeof (double) * num_coeff * num_coeff);
    vector0 = xmalloc (sizeof (double) * w->num_pos * num_coeff);
    vector1 = xmalloc (sizeof (double) * w->num_pos * num_coeff);


    /* Calculate coefficient matrix and vector.  */
    for (iy = 0; iy < num_coeff; ++iy)
    {
        for (ix = 0; ix < num_coeff; ++ix)
            matrix [iy * num_coeff + ix] = 0.0;

        for (j = 0; j < w->num_pos; ++j)
        {
            vector0 [iy * w->num_pos + j] = poly_term (iy, w->pos [j].x, w->pos [j].y, poly_order);

            for (ix = 0; ix < num_coeff; ++ix)
                matrix [iy * num_coeff + ix] += (vector0 [iy * w->num_pos + j]
                                                 * poly_term (ix, w->pos [j].x, w->pos [j].y, poly_order));
        }
    }


    /* Invert matrix.  */
    matrix_inv (matrix, num_coeff);


    /* Multiply inverse matrix with vector.  */
    for (iy = 0; iy < num_coeff; ++iy)
        for (j = 0; j < w->num_pos; ++j)
        {
            vector1 [iy * w->num_pos + j] = 0.0;

            for (ix = 0; ix < num_coeff; ++ix)
                vector1 [iy * w->num_pos + j] += matrix [iy * num_coeff + ix] * vector0 [ix * w->num_pos + j];
        }


    /* Store weights.  */
    w->weights = xmalloc (sizeof (double *) * x_size * y_size);

    for (y = 0; y < y_size; ++y)
        for (x = 0; x < x_size; ++x)
        {
            double *const weights = xmalloc (sizeof (double) * w->num_pos);
            w->weights [x + x_size * y] = weights;

            for (j = 0; j < w->num_pos; ++j)
            {
                weights [j] = 0.0;

                for (iy = 0; iy < num_coeff; ++iy)
                    weights [j] += vector1 [iy * w->num_pos + j] * poly_term (iy, x, y, poly_order);

                weights [j] *= (double)w->num_pos;
            }
        }


    free (vector1);
    free (vector0);
    free (matrix);

    return w;
}



/* Calculate one term of the polynomial.  */
static double
poly_term (const size_t i_coeff, const int x, const int y, const int poly_order)
{
    const size_t x_power = i_coeff / ((size_t)poly_order + 1);
    const size_t y_power = i_coeff % ((size_t)poly_order + 1);
    int result;
    size_t i;

    result = 1;

    for (i = 0; i < x_power; ++i)
        result *= x;
    for (i = 0; i < y_power; ++i)
        result *= y;

    return (double)result;
}



/* Invert a quadratic matrix.  */
static void
matrix_inv (double *const a, const size_t size)
{
    double *const b = xmalloc (sizeof (double) * size * size);
    size_t ix, iy, j;


    /* Copy matrix to new location.  */
    memcpy (b, a, sizeof (double) * size * size);

    /* Set destination matrix to unit matrix.  */
    for (iy = 0; iy < size; ++iy)
        for (ix = 0; ix < size; ++ix)
            a [iy * size + ix] = ix == iy ? 1.0 : 0.0;

    /* Convert matrix to upper triangle form.  */
    for (iy = 0; iy < size - 1; ++iy)
        for (j = iy + 1; j < size; ++j)
        {
            const double factor = b [j * size + iy] / b [iy * size + iy];

            for (ix = 0; ix < size; ++ix)
            {
                b [j * size + ix] -= factor * b [iy * size + ix];
                a [j * size + ix] -= factor * a [iy * size + ix];
            }
        }

    /* Convert matrix to diagonal form.  */
    for (iy = size - 1; iy > 0; --iy)
        for (j = 0; j < iy; ++j)
        {
            const double factor =  b [j * size + iy] / b [iy * size + iy];

            for (ix = 0; ix < size; ++ix)
                a [j * size + ix] -= factor * a [iy * size + ix];
        }

    /* Convert matrix to unit matrix.  */
    for (iy = 0; iy < size; ++iy)
        for (ix = 0; ix < size; ++ix)
            a [iy * size + ix] /= b [iy * size + iy];

    free (b);
}
