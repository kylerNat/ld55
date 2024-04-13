//TODO: I should make define_hashmap macros instead of doing the hacky #define arguments and #include "hashmap.h"
//no include guards, since this file can be reused to define different types of hashmaps
#ifdef value_type

#include <utils/hashmap_common.h>

#ifndef hash_function
#define hash_function djb2
#endif //hash_function

#define traverse_hashmap(key, keys, max_entries, exists_case, empty_case, full_case) traverse_hashmap_(key, hash_function(key), keys, max_entries, exists_case, empty_case, full_case)

#ifndef key_type
#define key_type char*
#endif

#ifndef hashmapname
#define hashmapname CONCAT(CONCAT(CONCAT(key_type, _), value_type), _hashmap)
#endif

#define kash_t CONCAT(hashmapname, _kash_t)

struct kash_t
{
    key_type key;
    uint hash;
};

//TODO: performance: non-linear probing & buckets
struct hashmapname
{
    key_type* keys;
    value_type* values;
    uint max_entries;

    inline value_type& operator[](key_type key)
    {
        traverse_hashmap(key, keys, max_entries,
                         {return values[index];},
                         {return values[index];},
                         {return values[index];}); //TODO: handle the case where the hashmap is full
    }

    //for slight perfomance optimization
    inline value_type& operator[](kash_t key_with_hash)
    {
        traverse_hashmap_(key_with_hash.key, key_with_hash.hash, keys, max_entries,
                          {return values[index];},
                          {return values[index];},
                          {return values[index];}); //TODO: handle the case where the hashmap is full
    }
};

void init_hashmap(hashmapname* table, int max_entries)
{
    table->keys = (key_type*) stalloc_clear(max_entries*sizeof(key_type));
    table->values = (value_type*) stalloc_clear(max_entries*sizeof(value_type));
    table->max_entries = max_entries;
}

bool in_map(hashmapname map, key_type key)
{
    traverse_hashmap(key, map.keys, map.max_entries,
                     return true;,
                     return false;,
                     return false;);
}

uint hash_index(hashmapname map, key_type key)
{
    traverse_hashmap(key, map.keys, map.max_entries,
                     return index;,
                     return -index-1;,
                     return MAP_FULL;);
}

uint add_entry(hashmapname map, key_type key, value_type value)
{
    auto& keys = map.keys;
    auto& max_entries = map.max_entries;
    traverse_hashmap(key, map.keys, map.max_entries,
                     { //entry already exists for this key
                         //TODO:
                         log_warning("entry already exists for: ", key, ". overwriting old value ", map.values[index],
                                     " with ", value);
                         map.values[index] = value;
                         return index;
                     },
                     { //empty cell found
                         map.keys[index] = key;
                         map.values[index] = value;
                         return index;
                     },
                     { //hashmap is full
                         #ifdef EXPANDABLE_HASHMAP
                         //TODO:
                         return MAP_FULL;
                         #else
                         log_error("non-expandable hashmap is full, cannot add entry: (", key, ", ", value, ")");
                         #endif
                     });
}

uint remove_entry(hashmapname map, key_type key)
{
    auto& keys = map.keys;
    auto& max_entries = map.max_entries;
    traverse_hashmap(key, map.keys, map.max_entries,
                     { //entry already exists for this key
                         int i;
                         int i_free = index;
                         for(i = index+1;
                             map.keys[i] && i != index;
                             i=(i+1)%max_entries)
                         {
                             int i_desired = hash_function(map.keys[i])%max_entries;
                             if((i_desired <= i_free && i_free < i)
                                || (i_desired > i
                                    && (i_desired <= i_free || i_free < i)))
                             {
                                 map.keys[i_free] = map.keys[i];
                                 map.values[i_free] = map.values[i];
                                 i_free = i;
                             }
                         }
                         map.keys[i_free] = 0;
                         map.values[i_free] = {0}; //TODO: make this work with all types
                         return index;
                     },
                     { //empty cell found
                         return index;
                     },
                     { //hashmap is full
                         return MAP_FULL;
                     });
}

inline value_type hash_value(hashmapname map, key_type key)
{
    return map.values[hash_index(map, key)];
}

#undef value_type
#undef hash_function
#undef traverse_hashmap
#undef key_type
#undef EXPANDABLE_HASHMAP
#undef hashmapname
#undef kash_t
#endif //ifdef value_type
