/* This file is part of a red-black tree implementation.
   Copyright (C) 2003, 2004 Martin Dickopp

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this file; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301,
   USA.  */

#ifndef HDR_RBTREE
#define HDR_RBTREE 1


/* Red-black tree.  The member should not be accessed directly.  */
struct rbtree {
    struct rbtree_node *root;
};


/* Create a tree.  */
#define rbtree_create(tree) ((void)((tree)->root = 0))

/* Destroy a tree.  Free all associated memory.  */
#define rbtree_destroy(tree) ((void)((tree)->root != 0 ? (rbtree_destroy1 ((tree)->root), 0) : 0))


/* Internal function; should not be called directly.  */
extern void rbtree_destroy1 (struct rbtree_node *node) gcc_attr_nonnull (());

/* Insert a new node into a tree.  The data is copied; the location of the copied data is returned,
   and if `exists_ptr' is not a null pointer, the integer at the location pointed to is set to zero.
   If the node already exists, the location of the existing node data is returned, and if `exists_ptr'
   is not a null pointer, the integer at the location pointed to is set to one.  If memory allocation
   fails, a null pointer is returned.  */
extern void *rbtree_insert (struct rbtree *tree, const void *data, size_t data_len,
                            int (*compare) (const void *a, const void *b),
                            int *exists_ptr) gcc_attr_nonnull ((1, 2, 4));

/* Delete a node.  The `data' parameter must point to the data associated with an existing node.  */
extern void rbtree_delete (struct rbtree *tree, void *data) gcc_attr_nonnull (());

/* Find a node.  Return the data associated with the node, or a null pointer if the node does not exist.  */
extern void *rbtree_find (const struct rbtree *tree, const void *data,
                          int (*compare) (const void *a, const void *b)) gcc_attr_pure gcc_attr_nonnull (());

/* Return the data associated with the first node, or a null pointer if the tree is empty.  */
extern void *rbtree_first (const struct rbtree *tree) gcc_attr_pure gcc_attr_nonnull (());

/* Return the data associated with the next node, or a null pointer if no more nodes exist.  */
extern void *rbtree_next (const void *data) gcc_attr_pure gcc_attr_nonnull (());


#endif
