#pragma once

#include <stddef.h>
#include <stdint.h>

//hashtable node,should be embedded into payload
struct Hnode{
    Hnode* next=NULL;
    uint64_t hcode=0;

};

struct HTab
{
    Hnode** tab=NULL;
    size_t mask=0;
    size_t size=0;
};

//the real hashtable interface 
//uses 2 hashmaps for progressive rehashing
struct Hmap{
    HTab newer;
    HTab older;
    size_t migrate_pos=0;
};

Hnode* hm_lookup(Hmap* hmap, Hnode* key, bool (*eq)(Hnode*, Hnode*));
void hm_insert(Hmap* hmap,Hnode* node);
Hnode *hm_delete(Hmap* hmap, Hnode* key, bool (*eq)(Hnode*, Hnode*));
void hm_clear(Hmap* hmap);
size_t hm_size(Hmap* hmap);
