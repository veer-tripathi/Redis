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

// iterate all nodes in hashmap
template <typename Func>
void hm_for_each(Hmap* hmap, Func fn) {
    // First, help ongoing rehashing
    // hm_help_rehashing(hmap);

    // ---- iterate newer table ----
    if (hmap->newer.tab) {
        size_t cap = hmap->newer.mask + 1;
        for (size_t i = 0; i < cap; ++i) {
            Hnode* node = hmap->newer.tab[i];
            while (node) {
                Hnode* next = node->next;   // safe if fn deletes node
                fn(node);
                node = next;
            }
        }
    }

    // ---- iterate older table (rehashing in progress) ----
    if (hmap->older.tab) {
        size_t cap = hmap->older.mask + 1;
        for (size_t i = hmap->migrate_pos; i < cap; ++i) {
            Hnode* node = hmap->older.tab[i];
            while (node) {
                Hnode* next = node->next;
                fn(node);
                node = next;
            }
        }
    }
}
