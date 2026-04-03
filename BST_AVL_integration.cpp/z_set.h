#pragma once 
#include "avl.h"
#include "h_map.h"


struct ZSet{
    AVLNode* root =NULL;
    Hmap hmap;
};

struct ZNode{
    AVLNode tree;
    Hnode hmap;
    double score=0;
    size_t len=0;
    char name[0];
};

bool zset_insert(ZSet* zset,const char* name, size_t len,double score);
ZNode* zset_lookup(ZSet* zset, const char* name, size_t len);
void zset_delete(ZSet* zset, ZNode* node);
ZNode* zset_seek(ZSet* zset, double score, const char* name, size_t len);
void zset_clear(ZSet* zset);
ZNode* znode_offset(ZNode* znode, int64_t offset);
