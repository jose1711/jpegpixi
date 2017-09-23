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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "util.h"

#if !STDC_HEADERS && HAVE_MALLOC_H
# include <malloc.h>
#endif
#if !HAVE_DECL_MALLOC
extern void *malloc ();
#endif

#include "rbtree.h"


/* Alignment of the data associated with a tree node.  */
#define ALIGNMENT 16

/* Size of `struct rbtree_node' rounded up to an integer multiple of `ALIGNMENT'.  */
#define SIZEOF_RBTREE_NODE (sizeof (struct rbtree_node) + (ALIGNMENT - 1) \
                            - (sizeof (struct rbtree_node) - 1) % ALIGNMENT)

/* Get data pointer from `struct rbtree_node' pointer.  */
#define NODE2DATA(node) ((void *)((char *)(node) + SIZEOF_RBTREE_NODE))

/* Get `struct rbtree_node' pointer from data pointer.  */
#define DATA2NODE(data) ((struct rbtree_node *)((char *)(data) - SIZEOF_RBTREE_NODE))



/* Color.  */
enum rbtree_color {BLACK = 0, RED = 1};

/* Tree node.  */
struct rbtree_node {
    struct rbtree_node *l;      /* Left child node.  */
    struct rbtree_node *r;      /* Right child node.  */
    struct rbtree_node *p;      /* Parent node.  */
    enum rbtree_color color;    /* Color.  */
};



static void rbtree_rot_l (struct rbtree *tree, struct rbtree_node *node) gcc_attr_nonnull (());
static void rbtree_rot_r (struct rbtree *tree, struct rbtree_node *node) gcc_attr_nonnull (());



/* Helper function: recursively destroy `node' and all nodes below it.  */
void
rbtree_destroy1 (struct rbtree_node *node)
{
    while (1)
    {
        struct rbtree_node *const tmp = node->r;

        if (node->l != 0)
            rbtree_destroy1 (node->l);

        free (node);

        if (tmp != 0)
            node = tmp;
        else
            break;
    }
}



/* Insert a new node into a tree.  The data is copied; the location of the copied data is returned,
   and if `exists_ptr' is not a null pointer, the integer at the location pointed to is set to zero.
   If the node already exists, the location of the existing node data is returned, and if `exists_ptr'
   is not a null pointer, the integer at the location pointed to is set to one.  If memory allocation
   fails, a null pointer is returned.  */
void *
rbtree_insert (struct rbtree *const tree, const void *const data, size_t data_len,
               int (*const compare) (const void *a, const void *b), int *const exists_ptr)
{
    struct rbtree_node *node;
    void *data_copy;


    {
        struct rbtree_node *node_p = 0, *tmp = tree->root;

        while (tmp != 0)
        {
            const int cmp = compare (data, NODE2DATA (tmp));

            node_p = tmp;

            if (cmp == 0)
            {
                if (exists_ptr != 0)
                    *exists_ptr = 1;
                return NODE2DATA (tmp);
            }
            else if (cmp < 0)
                tmp = tmp->l;
            else
                tmp = tmp->r;
        }

        node = malloc (SIZEOF_RBTREE_NODE + data_len);
        if (node == 0)
            return 0;

        data_copy = NODE2DATA (node);
        node->l = 0;
        node->r = 0;
        node->p = node_p;
        node->color = RED;
        memcpy (data_copy, data, data_len);
    }

    if (node->p == 0)
        tree->root = node;
    else if (compare (data, NODE2DATA (node->p)) < 0)
        node->p->l = node;
    else
        node->p->r = node;

    while (node != tree->root && node->p->color != BLACK)
        if (node->p == node->p->p->l)
        {
            if (node->p->p->r != 0 && node->p->p->r->color != BLACK)
            {
                node->p->p->r->color = BLACK;
                node->p->p->color = RED;
                node->p->color = BLACK;
                node = node->p->p;
            }
            else
            {
                if (node == node->p->r)
                {
                    node = node->p;
                    rbtree_rot_l (tree, node);
                }

                node->p->p->color = RED;
                node->p->color = BLACK;
                rbtree_rot_r (tree, node->p->p);
            }
        }
        else
        {
            if (node->p->p->l != 0 && node->p->p->l->color != BLACK)
            {
                node->p->p->l->color = BLACK;
                node->p->p->color = RED;
                node->p->color = BLACK;
                node = node->p->p;
            }
            else
            {
                if (node == node->p->l)
                {
                    node = node->p;
                    rbtree_rot_r (tree, node);
                }

                node->p->p->color = RED;
                node->p->color = BLACK;
                rbtree_rot_l (tree, node->p->p);
            }
        }

    tree->root->color = BLACK;
    if (exists_ptr != 0)
        *exists_ptr = 0;
    return data_copy;
}



/* Delete a node.  The `data' parameter must point to the data associated with an existing node.  */
void
rbtree_delete (struct rbtree *const tree, void *const data)
{
    struct rbtree_node *const node = DATA2NODE (data);
    struct rbtree_node *del_node, *tmp, *tmp_p;


    del_node = node;

    if (del_node->r == 0)
        tmp = del_node->l;
    else
    {
        if (del_node->l != 0)
        {
            del_node = del_node->r;
            while (del_node->l != 0)
                del_node = del_node->l;
        }

        tmp = del_node->r;
    }

    if (del_node != node)
    {
        node->l->p = del_node;
        del_node->l = node->l;

        if (del_node != node->r)
        {
            tmp_p = del_node->p;
            if (tmp != 0)
                tmp->p = del_node->p;
            del_node->p->l = tmp;
            del_node->r = node->r;
            node->r->p = del_node;
        }
        else
            tmp_p = del_node;

        if (tree->root == node)
            tree->root = del_node;
        else if (node->p->l == node)
            node->p->l = del_node;
        else
            node->p->r = del_node;

        del_node->p = node->p;

        {
            const enum rbtree_color tmp_color = del_node->color;
            del_node->color = node->color;
            node->color = tmp_color;
        }

        del_node = node;
    }
    else
    {
        tmp_p = node->p;

        if (tmp != 0)
            tmp->p = node->p;

        if (tree->root == node)
            tree->root = tmp;
        else if (node->p->l == node)
            node->p->l = tmp;
        else
            node->p->r = tmp;
    }

    if (del_node->color == BLACK)
    {
        while (tmp != tree->root && (tmp == 0 || tmp->color == BLACK))
            if (tmp == tmp_p->l)
            {
                struct rbtree_node *tmp_p_r = tmp_p->r;

                if (tmp_p_r->color != BLACK)
                {
                    tmp_p_r->color = BLACK;
                    tmp_p->color = RED;
                    rbtree_rot_l (tree, tmp_p);
                    tmp_p_r = tmp_p->r;
                }

                if ((tmp_p_r->l == 0 || tmp_p_r->l->color == BLACK)
                    && (tmp_p_r->r == 0 || tmp_p_r->r->color == BLACK))
                {
                    tmp_p_r->color = RED;
                    tmp = tmp_p;
                    tmp_p = tmp->p;
                }
                else
                {
                    if (tmp_p_r->r == 0 || tmp_p_r->r->color == BLACK)
                    {
                        tmp_p_r->l->color = BLACK;
                        tmp_p_r->color = RED;
                        rbtree_rot_r (tree, tmp_p_r);
                        tmp_p_r = tmp_p->r;
                    }

                    tmp_p_r->color = tmp_p->color;
                    tmp_p->color = BLACK;
                    if (tmp_p_r->r != 0)
                        tmp_p_r->r->color = BLACK;
                    rbtree_rot_l (tree, tmp_p);
                    break;
                }
            }
            else
            {
                struct rbtree_node *tmp_p_l = tmp_p->l;

                if (tmp_p_l->color != BLACK)
                {
                    tmp_p_l->color = BLACK;
                    tmp_p->color = RED;
                    rbtree_rot_r (tree, tmp_p);
                    tmp_p_l = tmp_p->l;
                }

                if ((tmp_p_l->l == 0 || tmp_p_l->l->color == BLACK)
                    && (tmp_p_l->r == 0 || tmp_p_l->r->color == BLACK))
                {
                    tmp_p_l->color = RED;
                    tmp = tmp_p;
                    tmp_p = tmp->p;
                }
                else
                {
                    if (tmp_p_l->l == 0 || tmp_p_l->l->color == BLACK)
                    {
                        tmp_p_l->r->color = BLACK;
                        tmp_p_l->color = RED;
                        rbtree_rot_l (tree, tmp_p_l);
                        tmp_p_l = tmp_p->l;
                    }

                    tmp_p_l->color = tmp_p->color;
                    tmp_p->color = BLACK;
                    if (tmp_p_l->l != 0)
                        tmp_p_l->l->color = BLACK;
                    rbtree_rot_r (tree, tmp_p);
                    break;
                }
            }

        if (tmp != 0)
            tmp->color = BLACK;
    }

    free (del_node);
}



/* Find a node.  Return the data associated with the node, or a null pointer if the node does not exist.  */
void *
rbtree_find (const struct rbtree *const tree, const void *const data,
             int (*const compare) (const void *a, const void *b))
{
    const struct rbtree_node *node = tree->root;

    while (node != 0)
    {
        const int cmp = compare (data, NODE2DATA (node));

        if (cmp == 0)
            return NODE2DATA (node);
        else if (cmp < 0)
            node = node->l;
        else
            node = node->r;
    }

    return 0;
}



/* Return the data associated with the first node, or a null pointer if the tree is empty.  */
void *
rbtree_first (const struct rbtree *const tree)
{
    const struct rbtree_node *node = tree->root;

    if (node == 0)
        return 0;

    while (node->l != 0)
        node = node->l;

    return NODE2DATA (node);
}



/* Return the data associated with the next node, or a null pointer if no more nodes exist.  */
void *
rbtree_next (const void *const data)
{
    const struct rbtree_node *node = DATA2NODE (data), *tmp;

    if (node->r != 0)
    {
        node = node->r;
        while (node->l != 0)
            node = node->l;

        return NODE2DATA (node);
    }

    tmp = node->p;
    while (tmp != 0 && tmp->r == node)
    {
        node = tmp;
        tmp = tmp->p;
    }

    return tmp != 0 ? NODE2DATA (tmp) : 0;
}



/* Rotate a node to the left.  */
static void
rbtree_rot_l (struct rbtree *const tree, struct rbtree_node *const node)
{
    struct rbtree_node *const tmp = node->r;

    node->r = tmp->l;
    if (tmp->l != 0)
        tmp->l->p = node;
    tmp->p = node->p;
    if (tmp->p == 0)
        tree->root = tmp;
    else if (node == node->p->l)
        node->p->l = tmp;
    else
        node->p->r = tmp;
    tmp->l = node;
    node->p = tmp;
}



/* Rotate a node to the right.  */
static void
rbtree_rot_r (struct rbtree *const tree, struct rbtree_node *const node)
{
    struct rbtree_node *const tmp = node->l;

    node->l = tmp->r;
    if (tmp->r != 0)
        tmp->r->p = node;
    tmp->p = node->p;
    if (tmp->p == 0)
        tree->root = tmp;
    else if (node == node->p->r)
        node->p->r = tmp;
    else
        node->p->l = tmp;
    tmp->r = node;
    node->p = tmp;
}
