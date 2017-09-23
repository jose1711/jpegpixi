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

#ifndef JPEGPIXI_H
#define JPEGPIXI_H 1


#include "jpegwrapper.h"


/* Valid range for DCT coefficients.  */
#if BITS_IN_JSAMPLE == 8
# define MIN_DCT_COEFF -1024
# define MAX_DCT_COEFF  1023
#else
# define MIN_DCT_COEFF -16384
# define MAX_DCT_COEFF  16383
#endif


/* Denominator for relative quantities. */
#define DENOM (DENOM_SQRT * DENOM_SQRT)

/* Square root of denominator for relative quantities. */
#define DENOM_SQRT 10000

/* Convert relative to absolute numbers. Care must be taken not to overflow integers.  */
#define REL_TO_ABS(n,m) \
    ((((n) / DENOM_SQRT) * (m) + ((n) % DENOM_SQRT) * (m) / DENOM_SQRT) / DENOM_SQRT)


/* Red-black tree.  */
struct rbtree;

/* Interpolation methods.  */
enum method_t {average = 0, linear, quadratic, cubic};

/* Dimension and direction of the interpolation.  */
enum dimdir_t {vertical = 0, horizontal, twodim};


/* Memory representation of JFIF image.  */
struct jfif;

/* Coordinates and size of a point.  */
struct point_s {
    unsigned int x;
    unsigned int y;
    unsigned int x_size;
    unsigned int y_size;
};

/* Coordinates and size of a point, and dimension and direction of the interpolation.  */
struct point_dimdir_s {
    struct point_s p;
    enum dimdir_t d;
};

/* Coordinates, size and luminosity of a point.  */
struct point_lumi_s {
    struct point_s p;
    int lumi;
};

/* Coordinate pair.  In the one-dimensional case, only the y coordinate is used.  */
struct xy_s {
    int x;
    int y;
};

/* Data needed to interpolate by calculating a weighted sum of pixel values.  */
struct weight_s {
    int x_size;
    int y_size;
    unsigned int is_twodim:1;
    int poly_order;
    struct xy_s *pos;
    size_t num_pos;
    const double **weights;
};


/* Open files and read header of input file.  */
extern void init_files (const char *infilename, const char *outfilename,
                        unsigned int *image_width, unsigned int *image_height) gcc_attr_nonnull ((3, 4));

/* Copy a JFIF image file to another, interpolate pixels.  */
extern void process_file (const struct rbtree *points, enum method_t method) gcc_attr_nonnull (());

/* Initialize interpolator.  */
extern void init_interpolator (void);

/* Interpolate a pixel.  */
extern void interpolate (const struct jpeg_decompress_struct *jpg, struct jfif *jfif,
                         const struct point_dimdir_s *point, enum method_t method) gcc_attr_nonnull (());

/* Initialize memory representation of JFIF image.  */
extern struct jfif *jfif_init (struct jpeg_decompress_struct *jpg, jvirt_barray_ptr *coeff) gcc_attr_nonnull (());

/* Finalize memory representation of JFIF image; store modified DCT blocks.  */
extern void jfif_fini (struct jfif *const jfif) gcc_attr_nonnull (());

/* Return the value of a pixel.  Coordinates are downsampled.  */
extern double get_pixel (struct jfif *jfif, int icomp, int x, int y) gcc_attr_nonnull (());

/* Set the value of a pixel.  Coordinates are downsampled.  */
extern void set_pixel (struct jfif *jfif, int icomp, int x, int y, double value) gcc_attr_nonnull (());

/* Perform forward discrete cosine transformation and quantization.  */
extern void fdct (JCOEF *freqs, const double *values, const UINT16 *quant) gcc_attr_nonnull (());

/* Perform inverse discrete cosine transformation and dequantization.  */
extern void idct (double *values, const JCOEF *freqs, const UINT16 *quant) gcc_attr_nonnull (());

/* Calculate weights for interpolation.  */
extern struct weight_s *get_weights (int x_size, int y_size, const int is_twodim, int poly_order) gcc_attr_pure;

/* Parse a number.  Return 1 if it is absolute, 0 if it is relative, -1 in case of an error.  */
extern int parse_number (const char **strptr, unsigned int *numptr) gcc_attr_nonnull (());

/* Consolidate adjacent points into larger points.  */
extern void consolidate_pixels (struct rbtree *points,
                                int (*validate_and_consolidate) (void *a, const void *b)) gcc_attr_nonnull (());

/* Compare two point structures.  They compare equal if the points overlap.  */
extern int point_s_cmp (const void *a, const void *b) gcc_attr_nonnull (());


#endif
