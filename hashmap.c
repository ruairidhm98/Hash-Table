/************************************************
 * Hashmap which maps a string to an int        *
 * Uses linear probing for collision handling   *
 *                                              *
 ************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

/* Entry in bucket array */
struct bucket {
    char *key;
    int value;
};

/* Structure that holds buckets */
struct hashmap {
    int size;
    int capacity;
    Bucket *buckets;
};

/* Hash function used to compute index into bucket array */
static unsigned long hash_function(char *key, int capacity) {
    
    unsigned long hash;
    
    hash = 5381;
    int c;

    while (c = *key++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % capacity;
}

/* Returns a pointer to an empty hash table if successfull, NULL otherwise */
HashMap *ht_init(int capacity) {

    HashMap *ht;
    int i;

    ht = (HashMap *) malloc(sizeof(HashMap));
    /* Print error message and return NULL if memory allocation fails */
    if (!ht) {
        fprintf(stderr, "Error: memory allocation failed\n");
        free((void *) ht);
        ht = NULL;
        return ht;
    }
    ht -> size = 0;
    ht -> capacity = capacity;
    ht -> buckets = (Bucket *) calloc(capacity, sizeof(Bucket));
    if (!ht -> buckets) {
        fprintf(stderr, "Error: memory allocation failed\n");
        free((void *) ht -> buckets);
        free((void *) ht);
        ht = NULL;
    }

    return ht;
}

/* Insert into hash map, returns 1/0 for success failure */
int ht_put(HashMap *ht, char *key, int value) {

    Bucket newBucket;
    char *tempKey;
    int idx;

    /* If the hash table is full, don't insert */
    if (!ht || ht -> size == ht -> capacity) {
        fprintf(stderr, "Error: can't insert into hash table\n");
        return 0;
    }
    newBucket.key = key;
    newBucket.value = value;
    
    /* Compute the index */
    idx = (int) hash_function(key, ht -> capacity);
    tempKey = ht -> buckets[idx].key;
    /* If the space is empty, insert into the array and we are done */
    if (!tempKey) {
        ht -> buckets[idx].key = key;
        ht -> buckets[idx].value = value;
    }
    /* If there is a duplicate key, then overwrite it */
    else if (!strcmp(tempKey, key)) 
        ht -> buckets[idx].value = value;
    /* The space is already taken, so do a linear search to find the next free space */
    else {
        while(1) {

            /* Move along 1 (or start from the beginning) */
            idx = (idx + 1) % ht -> capacity;
            /* Free space has been found */
            if (!ht -> buckets[idx].key) {
                ht -> buckets[idx].key = key;
                ht -> buckets[idx].value = value;
                break;
            }

        }

    }
    ht -> size++;

    return 1;
}

