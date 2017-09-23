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

#include "rbtree.h"
#include "jpegpixi.h"



/* Memory representation of JFIF image.  */
struct jfif {
    struct jpeg_decompress_struct *jpg;
    jvirt_barray_ptr *coeff;
    struct rbtree dct_block_tree;
};

/* DCT block.  */
struct dct_block {
    int icomp;                   /* Component.  */
    int x;                       /* Position of the block within the component.  */
    int y;
    JCOEFPTR coeff;              /* Coefficients in frequency space.  */
    const UINT16 *quant;         /* Quantization table.  */
    double values [DCTSIZE2];    /* Values in pixel space.  */
    int dirty;                   /* Flag: modified?  */
};



/* Previously requested DCT block.  */
struct dct_block *prev_dct_block;



static struct dct_block *get_block (struct jfif *jfif, int icomp, int x_block, int y_block) gcc_attr_nonnull (());
static int dct_block_cmp (const void *a, const void *b) gcc_attr_pure gcc_attr_nonnull (());



/* Initialize memory representation of JFIF image.  */
struct jfif *
jfif_init (struct jpeg_decompress_struct *const jpg, jvirt_barray_ptr *const coeff)
{
    struct jfif *const jfif = xmalloc (sizeof (struct jfif));

    jfif->jpg = jpg;
    jfif->coeff = coeff;
    rbtree_create (&jfif->dct_block_tree);

    prev_dct_block = 0;

    return jfif;
}



/* Finalize memory representation of JFIF image; store modified DCT blocks.  */
void
jfif_fini (struct jfif *const jfif)
{
    struct dct_block *block = rbtree_first (&jfif->dct_block_tree);

    while (block != 0)
    {
        if (block->dirty)
            fdct (block->coeff, block->values, block->quant);

        block = rbtree_next (block);
    }

    rbtree_destroy (&jfif->dct_block_tree);
    free (jfif);
}



/* Return the value of a pixel.  Coordinates are downsampled.  */
double
get_pixel (struct jfif *const jfif, const int icomp, const int x, const int y)
{
    const struct dct_block *const block = get_block (jfif, icomp, x / DCTSIZE, y / DCTSIZE);

    return block->values [(x % DCTSIZE) + DCTSIZE * (y % DCTSIZE)];
}



/* Set the value of a pixel.  Coordinates are downsampled.  */
void
set_pixel (struct jfif *const jfif, const int icomp, const int x, const int y, const double value)
{
    struct dct_block *const block = get_block (jfif, icomp, x / DCTSIZE, y / DCTSIZE);

    block->values [(x % DCTSIZE) + DCTSIZE * (y % DCTSIZE)] = CLAMP (value, (double)(-CENTERJSAMPLE) * 8.0,
                                                                    (double)(MAXJSAMPLE - CENTERJSAMPLE) * 8.0);
    block->dirty = 1;
}



/* Get or create a DCT block.  */
static struct dct_block *
get_block (struct jfif *const jfif, const int icomp, const int x_block, const int y_block)
{
    struct dct_block tmp, *block;


    /* Test if the previously requested block is requested again (very common).  */
    if (prev_dct_block != 0 && prev_dct_block->icomp == icomp
        && prev_dct_block->x == x_block && prev_dct_block->y == y_block)
        return prev_dct_block;

    /* Try to find DCT block.  */
    tmp.icomp = icomp;
    tmp.x = x_block;
    tmp.y = y_block;

    block = rbtree_find (&jfif->dct_block_tree, &tmp, dct_block_cmp);

    /* DCT block does not yet exist.  Create it and store it in the tree.  */
    if (block == 0)
    {
        const jpeg_component_info *const comp = &(jfif->jpg->comp_info [icomp]);
        const JBLOCKROW row = (jfif->jpg->mem->access_virt_barray) ((j_common_ptr)jfif->jpg, jfif->coeff [icomp],
                                                                    (JDIMENSION)y_block, (JDIMENSION)1, TRUE) [0];
        const UINT16 *const quant = jfif->jpg->quant_tbl_ptrs [comp->quant_tbl_no]->quantval;

        tmp.coeff = row [x_block];
        tmp.quant = quant;
        idct (tmp.values, row [x_block], quant);
        tmp.dirty = 0;

        if ((block = rbtree_insert (&jfif->dct_block_tree, &tmp, sizeof tmp, dct_block_cmp, 0)) == 0)
            mem_alloc_failed ();
    }

    prev_dct_block = block;
    return block;
}



/* Compare the components and coordinates of two DCT blocks.  */
static int
dct_block_cmp (const void *const a, const void *const b)
{
    const struct dct_block *const dct_block_a = a;
    const struct dct_block *const dct_block_b = b;

    if (dct_block_a->icomp < dct_block_b->icomp)
        return -1;
    else if (dct_block_a->icomp > dct_block_b->icomp)
        return 1;
    else if (dct_block_a->x < dct_block_b->x)
        return -1;
    else if (dct_block_a->x > dct_block_b->x)
        return 1;
    else if (dct_block_a->y < dct_block_b->y)
        return -1;
    else if (dct_block_a->y > dct_block_b->y)
        return 1;
    else
        return 0;
}
