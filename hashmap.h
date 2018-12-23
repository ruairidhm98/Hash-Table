#ifndef _HASH_MAP_
#define _HASH_MAP_

typedef struct bucket Bucket;
typedef struct hashmap HashMap;

/* Initialasies hash map*/
HashMap *ht_init(int);
/* Insert into hash map */
int ht_put(HashMap *, char *, int);
/* Reomves entry */
void ht_remove_entry(HashMap *, char *);
/* Checks if hash map is empty */
bool ht_is_empty(HashMap *);
/* Print contents of hash map */
void ht_print_hash(HashMap *);
/* Wipes hash map */
void ht_deinit(HashMap *);
/* Hash function */
static unsigned long hash_function(HashMap *, char *);

#endif