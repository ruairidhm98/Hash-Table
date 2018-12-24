#ifndef _HASH_MAP_
#define _HASH_MAP_

typedef struct bucket Bucket;
typedef struct hashtable HashTable;
typedef struct hashtable_iterator HashTableIterator;

/* Initialasies hash map*/
HashTable *ht_init(int capacity);
/* Insert into hash map */
int ht_put(HashTable *ht, char *key, int value);
/* Reomves entry */
int ht_remove_entry(HashTable *ht, char *key);
/* Returns the keys of the hash table as an array */
char **ht_keys(HashTable *ht);
/* Checks if hash map is empty */
int ht_is_empty(HashTable *ht);
/* Print contents of hash map */
void ht_print(HashTable *ht);
/* Wipes hash map */
void ht_destory(HashTable *ht);
/* Initialises hash map iterator */
HashTableIterator *ht_iter_init(HashTable *ht);
/* Returns the next bucket in the array */
Bucket *ht_iter_next(HashTableIterator *hti);
/* Destorys the iterator */
void ht_iter_destroy(HashTableIterator *hti);

#endif 