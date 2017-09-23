/* This file is part of jpegpixi, a program to interpolate pixels in
   JFIF image files.
   Copyright (C) 2004 Martin Dickopp

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
#include "rbtree.h"



/* Consolidate adjacent points into larger points.  */
void
consolidate_pixels (struct rbtree *const points, int (*const validate_and_consolidate) (void *a, const void *b))
{
    struct point_s *previous_point = rbtree_first (points);

    if (previous_point != 0)
    {
        /* Consolidate horizontally.  */
        {
            struct point_s *point = rbtree_next (previous_point);

            while (point != 0)
            {
                if (point->y == previous_point->y && point->x == previous_point->x + previous_point->x_size)
                {
                    if (validate_and_consolidate (point, previous_point))
                    {
                        point->x = previous_point->x;
                        point->x_size = previous_point->x_size + 1;
                    }
                    else
                    {
                        struct point_s *const tmp = point;

                        point = rbtree_next (point);
                        rbtree_delete (points, tmp);
                    }

                    rbtree_delete (points, previous_point);
                }

                if (point != 0)
                {
                    previous_point = point;
                    point = rbtree_next (point);
                }
            }
        }


        /* Consolidate vertically.  */
        {
            struct point_s *point = rbtree_first (points);

            while (point != 0)
            {
                while (1)
                {
                    struct point_s tmp, *point_below;

                    tmp.x = point->x;
                    tmp.y = point->y + point->y_size;
                    tmp.x_size = point->x_size;
                    tmp.y_size = 1;

                    if ((point_below = rbtree_find (points, &tmp, point_s_cmp)) != 0)
                    {
                        if (validate_and_consolidate (point, point_below))
                        {
                            point->x = MIN (point->x, point_below->x);
                            point->x_size = MAX (point->x + point->x_size,
                                                 point_below->x + point_below->x_size) - point->x;
                            point->y_size = MAX (point->y + point->y_size,
                                                 point_below->y + point_below->y_size) - point->y;
                        }
                        else
                        {
                            struct point_s *const tmp1 = point;

                            point = rbtree_next (point);
                            rbtree_delete (points, tmp1);
                        }

                        rbtree_delete (points, point_below);
                    }
                    else
                        break;
                }

                if (point != 0)
                    point = rbtree_next (point);
            }
        }
    }
}




/* Compare two point structures.  They compare equal if the points overlap.  */
int
point_s_cmp (const void *const a, const void *const b)
{
    const struct point_s *const point_s_a = a;
    const struct point_s *const point_s_b = b;

    if (OVERLAP2D (point_s_a->x, point_s_a->y, point_s_a->x_size, point_s_a->y_size,
                   point_s_b->x, point_s_b->y, point_s_b->x_size, point_s_b->y_size))
        return 0;
    else if (point_s_a->y < point_s_b->y)
        return -1;
    else if (point_s_a->y > point_s_b->y)
        return 1;
    else if (point_s_a->x < point_s_b->x)
        return -1;
    else
        return 1;
}
