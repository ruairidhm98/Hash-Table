#ifndef _HASH_MAP_
#define _HASH_MAP_

typedef struct bucket Bucket;
typedef struct hashmap HashMap;
typedef struct hashmap_iterator HashMapIterator;

/* Initialasies hash map*/
HashMap *ht_init(int capacity);
/* Insert into hash map */
int ht_put(HashMap *ht, char *key, int value);
/* Reomves entry */
int ht_remove_entry(HashMap *ht, char *key);
/* Returns the keys of the hash table as an array */
char **ht_keys(HashMap *ht);
/* Checks if hash map is empty */
int ht_is_empty(HashMap *ht);
/* Print contents of hash map */
void ht_print(HashMap *ht);
/* Wipes hash map */
void ht_destory(HashMap *ht);

#endif