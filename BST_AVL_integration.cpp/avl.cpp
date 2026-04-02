#include "avl.h"
#include<assert.h>


static uint32_t max(uint32_t l, uint32_t r){
    return l<r?l:r;
}

static void update(AVLNode* node){
    node->height=1+max(avl_height(node->left),avl_height(node->right));
    node->cnt=1+avl_cnt(node->right)+avl_cnt(node->left);

}

static int getBalance(AVLNode* n){
    return (int)avl_height(n->left)-(int)avl_height(n->right);
}

static AVLNode *LR(AVLNode* z, AVLNode* x, AVLNode* y){
    AVLNode* T2=x->left;
    AVLNode* p=z->parent;

    x->left=z;
    z->right=T2;

    if(T2)T2->parent=z;

    x->parent=p;
    z->parent=x;

    if(p){
        if(p->left==z)p->left=x;
        else{
            p->right=x;
        }
    }
    update(z);
    update(x);
    return x;
}

static AVLNode* RR(AVLNode* z, AVLNode* x, AVLNode* y){
    AVLNode* T2=x->right;
    AVLNode* p=z->parent;

    x->right=z;
    z->left=T2;

    if(T2)T2->parent=z;

    x->parent=p;
    z->parent=x;

    if(p){
        if(p->left==z)p->left=x;
        else p->right=x;
    }

    update(z);
    update(x);
    return x;
}

static AVLNode* rebalance(AVLNode* z){
    int balance=getBalance(z);
    if(balance>1 || balance<(-1)){
        AVLNode* x=(avl_height(z->left)>avl_height(z->right)?z->left:z->right);
        AVLNode *y = (avl_height(x->left) > avl_height(x->right)) ? x->left : x->right;

        if(z->left==x && x->left==y){
            return RR(z,x,y);
        }
        else if(z->right==x && x->right==y){
            return LR(z,x,y);
        }
        else if(z->right==x && x->left==y){
            RR(x,y,nullptr);
            return LR(z,x,y);
        }
        else{
            LR(x,y,nullptr);
            return RR(z,x,y);
        }
    }
    return z;
}

AVLNode* avl_fix(AVLNode* node){
    AVLNode* root=node;
    while(node){
        update(node);
        AVLNode* new_sub=rebalance(node);
        if(!new_sub->parent){
            root=new_sub;
        }
        node=new_sub->parent;
    }
    return root;
}


static AVLNode* avl_del_esy(AVLNode* node){
    AVLNode* child=node->left?node->left:node->right;
    AVLNode* parent=node->parent;

    if(child)child->parent=parent;
    if(!parent){return child;}

    if(parent->left==node)parent->left=child;
    else{
        parent->right=child;
    }
    return avl_fix(parent);
}

static AVLNode* avl_del(AVLNode* node){
    if(!node->left || !node->right){
        return avl_del_esy(node);
    }
    AVLNode* victim=node->right;
    while(victim->left)victim=victim->left;
    AVLNode* root=avl_del_esy(victim);

    victim->left=node->left;
    victim->right=node->right;
    victim->parent=node->parent;

    if(victim->left)victim->left->parent=victim;
    if(victim->right)victim->right->parent=victim;

    if(!victim->parent)root=victim;
    else{
        if(victim->parent->left==node){
            victim->parent->left=victim;
        }
        else{
            victim->parent->right=victim;
        }
    }

    return avl_fix(victim);
}

