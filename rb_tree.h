#pragma once
#ifndef _RB_TREE_
#define _RB_TREE_

#include "exception.h"
#include "allocate.h"
#include "type_traits.h"
#include "initialized.h"
#include "iterator.h"

typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false; //红色
const __rb_tree_color_type __rb_tree_black = true;

struct __rb_tree_node_base
{
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base* base_ptr;

    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x)
    {
        while (x->left != 0)
            x = x->left;
        return x;
    }

    static base_ptr maximum(base_ptr x)
    {
        while (x->right != 0) 
            x = x->right;
        return x;
    }
};

struct __rb_tree_base_iterator
{
    typedef __rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;
    
    base_ptr node; //指向红黑树对应节点 与红黑树产生结合

    void increment()
    {
        if (node->right != 0)
        {
            node = node->right;
            while (node->left != 0)
                node = node->left;
        }
        else 
        {
            base_ptr y = node->parent;
            while (node == y->right)
            {
                node = y;
                y = y->parent;
            }
            if (node->right != y)
                node = y;
        }
    }

    void decrement()
    {
        //此时node指向root 还有一个header与root互相指
        if (node->color == __rb_tree_red && 
            node->parent->parent == node)
            node = node->right;
        else if (node->left != 0)  //如果左边不为0 到左子树的最右节点的父节点
        {
            base_ptr y = node->left;
            while (y->right != 0)
                y = y->right;
            node = y;
        }
        else 
        {  
            base_ptr y = node->parent;
            while (node == y->left)
            {
                node = y;
                y = y->parent;
            }
            node = y;


        }
    }
};


#endif
