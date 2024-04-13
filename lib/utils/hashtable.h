//TODO: I should make define_hashtable macros instead of doing the hacky #define arguments and #include "hashtable.h"
//no include guards, since this file can be reused to define different types of hashtables
#include <utils/hashtable_common.h>

#ifndef hash_function
#define hash_function djb2
#endif //hash_function

#define traverse_hashtable(key, keys, max_entries, exists_case, empty_case, full_case) traverse_hashtable_(key, hash_function(key), keys, max_entries, exists_case, empty_case, full_case)

#ifndef key_type
#define key_type char*
#endif

#ifndef hashtablename
#define hashtablename CONCAT(key_type, _hashtable)
#endif

#define kash_t CONCAT(hashtablename, _kash_t)

struct kash_t
{
    key_type key;
    uint hash;
};

struct hashtablename
{
    key_type* keys;
    uint max_entries;

    inline bool operator[](key_type key)
    {
        traverse_hashtable(key, keys, max_entries,
                         {return false;},
                         {return true;},
                         {return false;}); //TODO: handle the case where the hashtable is full
    }

    //for slight perfomance optimization
    inline bool operator[](kash_t key_with_hash)
    {
        traverse_hashtable_(key_with_hash.key, key_with_hash.hash, keys, max_entries,
                         {return false;},
                         {return true;},
                         {return false;}); //TODO: handle the case where the hashtable is full
    }
};

uint hash_index(hashtablename table, key_type key)
{
    traverse_hashtable(key, table.keys, table.max_entries,
                     return index;,
                     return -index-1;,
                     return TABLE_FULL;);
}

uint add_entry(hashtablename table, key_type key, value_type value)
{
    auto& keys = table.keys;
    auto& max_entries = table.max_entries;
    traverse_hashtable(key, table.keys, table.max_entries,
                     { //entry already exists for this key
                         //TODO:
                         log_warning(key, " already in hashtable ", table.values[index]);
                         return index;
                     },
                     { //empty cell found
                         table.keys[index] = key;
                         return index;
                     },
                     { //hashtable is full
                         #if(EXPANDABLE_HASHTABLE)
                         //TODO:
                         return TABLE_FULL;
                         #else
                         log_error("non-expandable hashtable is full, cannot add: ", key);
                         #endif
                     });
}

uint remove_entry(hashtablename table, key_type key)
{
    auto& keys = table.keys;
    auto& max_entries = table.max_entries;
    traverse_hashtable(key, table.keys, table.max_entries,
                     { //entry already exists for this key
                         int i;
                         int i_free = index;
                         for(i = index+1;
                             table.keys[i] && i != index;
                             i=(i+1)%max_entries)
                         {
                             int i_desired = hash_function(table.keys[i])%max_entries;
                             if((i_desired <= i_free && i_free < i)
                                || (i_desired > i
                                    && (i_desired <= i_free || i_free < i)))
                             {
                                 table.keys[i_free] = table.keys[i];
                                 i_free = i;
                             }
                         }
                         table.keys[i_free] = 0;
                         return index;
                     },
                     { //empty cell found
                         return index;
                     },
                     { //hashtable is full
                         return TABLE_FULL;
                     });
}

#undef hash_function
#undef traverse_hashtable
#undef key_type
#undef EXPANDABLE_HASHTABLE
#undef hashtablename
#undef kash_t
