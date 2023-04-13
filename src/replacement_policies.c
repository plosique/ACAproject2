//
// This file contains all of the implementations of the replacement_policy
// constructors from the replacement_policies.h file.
//
// It also contains stubs of all of the functions that are added to each
// replacement_policy struct at construction time.
//
// ============================================================================
// NOTE: It is recommended that you read the comments in the
// replacement_policies.h file for further context on what each function is
// for.
// ============================================================================
//

#include "replacement_policies.h"
#include<stdlib.h> 

// LRU Replacement Policy
// ============================================================================
// TODO feel free to create additional structs/enums as necessary

void lru_cache_access(struct replacement_policy *replacement_policy,
                      struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{
    // update the LRU replacement policy state given a new memory access
    uint32_t set_start = set_idx*cache_system->associativity; 
    uint32_t* times = (uint32_t*) replacement_policy->data;
    replacement_policy->time++; 

    struct cache_line *cls = cache_system->cache_lines;   
    for(int id=0;id<cache_system->associativity;id++){
     if(cls[set_start+id].tag == tag){
      times[set_start+id] = replacement_policy->time; 
      return; 
     }
    }


}

uint32_t lru_eviction_index(struct replacement_policy *replacement_policy,
                            struct cache_system *cache_system, uint32_t set_idx)
{
    //return the index within the set that should be evicted.
    uint32_t set_start = set_idx*cache_system->associativity; 
    uint32_t* times = (uint32_t*) replacement_policy->data; 
    int rid = -1; 
    uint32_t max_time = -1; 
    for(int id = 0; id<cache_system->associativity; id++){
      if(rid == -1 || times[set_start+id] < max_time){
        max_time = times[set_start+id]; 
        rid = id; 
      }
    }

    return rid;
}

void lru_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // TODO cleanup any additional memory that you allocated in the
    // lru_replacement_policy_new function.
    free(replacement_policy->data);
}

struct replacement_policy *lru_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    struct replacement_policy *lru_rp = calloc(1, sizeof(struct replacement_policy));
    lru_rp->cache_access = &lru_cache_access;
    lru_rp->eviction_index = &lru_eviction_index;
    lru_rp->cleanup = &lru_replacement_policy_cleanup;
    lru_rp->time = 0;

    // allocate any additional memory to store metadata here and assign to lru_rp->data
    // treat lru_rp->data as a histogram
    uint32_t num_cache_lines = sets*associativity; 
    lru_rp->data =(uint32_t*) malloc(num_cache_lines*sizeof(uint32_t)); 
    uint32_t *data_placeholder = (uint32_t*)lru_rp->data; 
    for(int i=0;i<num_cache_lines;i++){
      data_placeholder[i] = 0;
    }

    return lru_rp;
}

// RAND Replacement Policy
// ============================================================================
void rand_cache_access(struct replacement_policy *replacement_policy,
                       struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{
    // update the RAND replacement policy state given a new memory access
}

uint32_t rand_eviction_index(struct replacement_policy *replacement_policy,
                             struct cache_system *cache_system, uint32_t set_idx)
{
    // return the index within the set that should be evicted
    uint32_t rid = rand() % (cache_system->associativity);
    return rid;

}

void rand_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // cleanup any additional memory that you allocated in the
    // rand_replacement_policy_new function.
}

struct replacement_policy *rand_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    // Seed randomness
    srand(time(NULL));

    struct replacement_policy *rand_rp = malloc(sizeof(struct replacement_policy));
    rand_rp->cache_access = &rand_cache_access;
    rand_rp->eviction_index = &rand_eviction_index;
    rand_rp->cleanup = &rand_replacement_policy_cleanup;

    // allocate any additional memory to store metadata here and assign to
    // rand_rp->data.

    return rand_rp;
}

// LRU_PREFER_CLEAN Replacement Policy
// ============================================================================
void lru_prefer_clean_cache_access(struct replacement_policy *replacement_policy,
                                   struct cache_system *cache_system, uint32_t set_idx,
                                   uint32_t tag)
{
    // update the LRU_PREFER_CLEAN replacement policy state given a new
    // memory access
    // NOTE: you may be able to share code with the LRU policy
    uint32_t set_start = set_idx*cache_system->associativity; 
    uint32_t* times = (uint32_t*) replacement_policy->data;
    replacement_policy->time++; 

    struct cache_line *cls = cache_system->cache_lines;   
    for(int id=0;id<cache_system->associativity;id++){
     if(cls[set_start+id].tag == tag){
      times[set_start+id] = replacement_policy->time; 
      return; 
     }
    }
}

uint32_t lru_prefer_clean_eviction_index(struct replacement_policy *replacement_policy,
                                         struct cache_system *cache_system, uint32_t set_idx)
{
    // return the index within the set that should be evicted.
    uint32_t set_start = set_idx*cache_system->associativity; 
    uint32_t* times = (uint32_t*) replacement_policy->data; 
    struct cache_line* cls = cache_system->cache_lines; 

    //evict clean index if they exist  
    int rid = -1; 
    uint32_t max_time = replacement_policy->time;
    if(cls[set_start].status!=MODIFIED){
      rid = 0;
      max_time = times[set_start];
    }

    for(int id=1;id<cache_system->associativity;id++){
      if(cls[set_start+id].status!=MODIFIED && times[set_start+id] <= max_time){
        rid = id;
        max_time = times[set_start+id]; 
      }
    }
    if(rid!=-1){
      return rid; 
    }
    //no clean indexes exist if this line is reached

    //evict a dirty index 
    rid = 0;  
    max_time = times[set_start];  
    for(int id = 1; id<cache_system->associativity; id++){
      if(times[set_start+id] < max_time){
        max_time = times[set_start+id]; 
        rid = id; 
      }
    }

    return rid;
}

void lru_prefer_clean_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // cleanup any additional memory that you allocated in the
    // lru_prefer_clean_replacement_policy_new function.
    free(replacement_policy->data);
}

struct replacement_policy *lru_prefer_clean_replacement_policy_new(uint32_t sets,
                                                                   uint32_t associativity)
{
    struct replacement_policy *lru_prefer_clean_rp = malloc(sizeof(struct replacement_policy));
    lru_prefer_clean_rp->cache_access = &lru_prefer_clean_cache_access;
    lru_prefer_clean_rp->eviction_index = &lru_prefer_clean_eviction_index;
    lru_prefer_clean_rp->cleanup = &lru_prefer_clean_replacement_policy_cleanup;

    // allocate any additional memory to store metadata here and assign to
    // lru_prefer_clean_rp->data.
    uint32_t num_cache_lines = sets*associativity; 
    lru_prefer_clean_rp->data =(uint32_t*) malloc(num_cache_lines*sizeof(uint32_t)); 
    uint32_t *data_placeholder = (uint32_t*)lru_prefer_clean_rp->data; 
    for(int i=0;i<num_cache_lines;i++){
      data_placeholder[i] = 0;
    }

    return lru_prefer_clean_rp;
}
