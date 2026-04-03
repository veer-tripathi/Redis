#include "z_set.h"
#include "common.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
// #include<h_map.h>

static ZNode* znode_new(const char *name, size_t len, double score){
    ZNode* node=(ZNode*)malloc(sizeof(ZNode)+len);
    avl_init(&node->tree);
    node->hmap.next=nullptr;
    node->hmap.hcode=str_hash((uint8_t*)name,len);
    node->score=score;
    node->len=len;
    memcpy(&node->name[0],name,len);
    return node;
}

static void znode_del(ZNode* node){
    free(node);
}

static size_t min(size_t lhs, size_t rhs){
    return lhs<rhs?lhs:rhs;
}

static bool zless(AVLNode* lhs, double score, const char* name, size_t len){
    ZNode *zl = container_of(lhs, ZNode, tree);

    if (zl->score != score) {
        return zl->score < score;
    }

    size_t m = std::min(zl->len, len);

    for (size_t i = 0; i < m; i++) {
        if (zl->name[i] != name[i]) {
            return zl->name[i] < name[i];
        }
    }

    return zl->len < len;
}

static bool zless(AVLNode* lhs, AVLNode* rhs){
    ZNode* zr=container_of(rhs,ZNode,tree);
    return zless(lhs,zr->score, zr->name,zr->len);
}

static void tree_insert(ZSet* zset, ZNode* node){
    AVLNode* parent=nullptr;
    AVLNode** from=&zset->root;

    while(*from){
        parent=*from;
        from=zless(&node->tree,parent)?&parent->left:&parent->right;
    }
    *from=&node->tree;
    node->tree.parent=parent;
    zset->root=avl_fix(&node->tree);
}

static void zset_update(ZSet* zset, ZNode* node, double score){
    if(node->score==score){
        return ;
    }
    zset->root=avl_del(&node->tree);
    avl_init(&node->tree);
    node->score=score;
    tree_insert(zset,node);
}

bool zset_insert(ZSet* zset, const char* name, double score, size_t len){
    ZNode* node=zset_lookup(zset,name, len);
    if(node){
        zset_update(zset,node,score);
        return false;
    }
    else{
        node=znode_new(name,len,score);
        hm_insert(&zset->hmap,&node->hmap);
        tree_insert(zset,node);
        return true;
    }
}

//a helper lookup in hashtable
struct Hkey{
    Hnode node;
    const char* name=NULL;
    size_t len=0;
};

static bool hcmp(Hnode* node, Hnode* key){
    ZNode* znode=container_of(node,ZNode, hmap);
    Hkey* hkey=container_of(key,Hkey,node);

    if(znode->len!=hkey->len){
        return false;
    }
    return 0==memcmp(znode->name,hkey->name,znode->len);
}

ZNode* zset_lookup(ZSet* zset, const char* name, size_t len){
    if(zset->root==NULL){
        return NULL;
    }
    Hkey key;
    key.len=len;
    key.name=name;
    key.node.hcode=str_hash((uint8_t*)name,len);
    Hnode* found=hm_lookup(&zset->hmap,&key.node,&hcmp);
    return found?container_of(found,ZNode,hmap):NULL;
}

void zset_delete(ZSet* zset, ZNode* node){
    // remove from the hashtable
    Hkey key;
    key.node.hcode=node->hmap.hcode;    
    key.len=node->len;
    key.name=node->name;
    Hnode* found=hm_delete(&zset->hmap,&key.node,&hcmp);
    assert(found);
    //remove from the tree
    zset->root=avl_del(&node->tree);
    znode_del(node);
}

ZNode *zset_seek(ZSet* zset, double score, const char* name, size_t len){
    AVLNode* found=NULL;
    for(AVLNode* node=zset->root;node;){
        if(zless(node,score,name,len)){
            node=node->right;
        }
        else{
            found=node; // canditate
            node=node->left;
        }
    }
    return found? container_of(found,ZNode, tree):NULL;
}

ZNode* znode_offset(ZNode* znode, int64_t offset){
    AVLNode* tnode=znode?avl_offset(&znode->tree,offset):NULL;
    return tnode?container_of(tnode,ZNode,tree):NULL;
}

static void tree_dispose(AVLNode* root){
    if(!root){
        return;
    }
    tree_dispose(root->left);
    tree_dispose(root->right);
    znode_del(container_of(root,ZNode,tree));
}

void zset_clear(ZSet* zset){
    hm_clear(&zset->hmap);
    tree_dispose(zset->root);
    zset->root=NULL;
}