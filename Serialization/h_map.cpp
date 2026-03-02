#include <assert.h>
#include <stdlib.h>     // calloc(), free()
#include "h_map.h"


//n must be a power of 2
static void h_init(HTab* htab,size_t n){
    assert(n>0 && ((n&(n-1))==0));
    htab->tab=(Hnode**)(calloc(n, sizeof(Hnode*)));
    htab->mask=n-1;
    htab->size=0;
}

//hashtable insertion
static void h_insert(HTab* htab, Hnode* node){
    size_t pos=node->hcode & htab->mask;
    Hnode* next=htab->tab[pos];
    node->next=next;
    htab->tab[pos]=node;
    htab->size++;
}

// hashtable look up subroutine.
// Pay attention to the return value. It returns the address of
// the parent pointer that owns the target node,
// which can be used to delete the target node.
static Hnode** h_lookup(HTab* htab, Hnode* key, bool (*eq)(Hnode*, Hnode*)){
    if(!htab->tab){
        return NULL;
    }
    size_t pos=key->hcode & htab->mask;
    Hnode** from=&htab->tab[pos];
    for(Hnode* curr;(curr=*from)!=NULL;from=&curr->next){
        if(curr->hcode==key->hcode && eq(curr,key)){
            return from;
        }
    }
    return NULL;
}

static Hnode *h_detach(HTab* htab, Hnode** from){
    Hnode* node=*from;
    *from = node->next;
    htab->size--;
    return node;
}

const size_t k_rehashing_work=128;

static void hm_help_rehashing(Hmap* hmap){
    size_t nwork=0;

    while(nwork<k_rehashing_work && hmap->older.size!=0){
        Hnode** from=&hmap->older.tab[hmap->migrate_pos];
        if(!from){
            hmap->migrate_pos++;
            continue;
        }
        h_insert(&hmap->newer, h_detach(&hmap->older, from));
        nwork++;
    }

    if(hmap->older.size==0 && hmap->older.tab){
        free(hmap->older.tab);
        hmap->older=HTab{};
    }
}


static void hm_trigger_rehashing(Hmap* hmap){
    assert(hmap->older.tab==NULL);
    hmap->older=hmap->newer;
    h_init(&hmap->newer, (hmap->newer.mask+1)*2);
    hmap->migrate_pos=0;
}

Hnode* hm_lookup(Hmap* hmap, Hnode* node, bool (*eq)(Hnode*,Hnode*)){
    hm_help_rehashing(hmap);
    Hnode** from=h_lookup(&hmap->newer,node,eq);
    if(!from){
        from=h_lookup(&hmap->older,node,eq);
    }
    return from?(*from):NULL;
}

const size_t k_max_load_factor=8;

void hm_insert(Hmap* hmap,Hnode* node){
    if(!hmap->newer.tab){
        h_init(&hmap->newer,4);
    }
    h_insert(&hmap->newer,node);
    if(!hmap->older.tab){
        size_t threshold=(hmap->newer.mask+1)*k_max_load_factor;
        if(hmap->newer.size>=threshold){
            hm_trigger_rehashing(hmap);
        }
    }
    hm_help_rehashing(hmap);
}

Hnode* hm_delete(Hmap* hmap, Hnode* node, bool (*eq)(Hnode *, Hnode*)){
    hm_help_rehashing(hmap);
    if(Hnode** from=h_lookup(&hmap->newer,node,eq)){
        return h_detach(&hmap->newer,from);
    }
    if(Hnode** from=h_lookup(&hmap->older,node,eq)){
        return h_detach(&hmap->older,from);
    }
    return NULL;
}

void hm_clear(Hmap* hmap){
    free(hmap->newer.tab);
    free(hmap->older.tab);
    *hmap=Hmap{};
}

size_t hm_size(Hmap *hmap) {
    return hmap->newer.size + hmap->older.size;
}