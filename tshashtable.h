#ifndef _HASH_MAP_
#define _HASH_MAP_

typedef struct bucket Bucket;
typedef struct ts_hashtable TSHashTable;
typedef struct ts_hashtable_iterator TSHashTableIterator;

/* Initialasies hash map*/
TSHashTable *ht_init(int capacity);
/* Insert into hash map */
int ht_put(TSHashTable *ht, char *key, int value);
/* Reomves entry */
int ht_remove_entry(TSHashTable *ht, char *key);
/* Returns the keys of the hash table as an array */
char **ht_keys(TSHashTable *ht);
/* Checks if hash map is empty */
int ht_is_empty(TSHashTable *ht);
/* Print contents of hash map */
void ht_print(TSHashTable *ht);
/* Wipes hash map */
void ht_destory(TSHashTable *ht);
/* Initialises hash map iterator */
TSHashTableIterator *ht_iter_init(TSHashTable *ht);
/* Returns the next bucket in the array */
Bucket *ht_iter_next(TSHashTableIterator *hti);
/* Destorys the iterator */
void ht_iter_destroy(TSHashTableIterator *hti);

#endif 