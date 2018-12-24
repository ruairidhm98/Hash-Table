/************************************************
 * HashTable which maps a string to an int        *
 * Uses linear probing for collision handling   *
 *                                              *
 ************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

#define MALLOC_FAILURE "Error: memory allocation failed\n"
#define DELETE_FAILURE "Error: hash table is empty\n"
#define KEY_ERROR      "Error: key is not in table\n"

/* Entry in bucket array */
struct bucket {
    char *key;
    int value;
};

/* Structure that holds buckets */
struct HashTable {
    int size;
    int capacity;
    Bucket *buckets;
};

/* An iterator for the hash map */
struct HashTable_iterator {

};

/* Hash function used to compute index into bucket array */
static unsigned long hash_function(char *key, int capacity) {
    
    unsigned long hash;
    
    hash = 5381;
    int c;

    while ((c = *key++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % capacity;
}

/* Returns a pointer to an empty hash table if successfull, NULL otherwise */
HashTable *ht_init(int capacity) {

    HashTable *ht;
    int i;

    ht = (HashTable *) malloc(sizeof(HashTable));
    /* Print error message and return NULL if memory allocation fails */
    if (!ht) {
        fprintf(stderr, MALLOC_FAILURE);
        free((void *) ht);
        ht = NULL;
        return ht;
    }
    ht -> size = 0;
    ht -> capacity = capacity;
    ht -> buckets = (Bucket *) calloc(capacity, sizeof(Bucket));
    if (!ht -> buckets) {
        fprintf(stderr, MALLOC_FAILURE);
        free((void *) ht -> buckets);
        free((void *) ht);
        ht = NULL;
    }

    return ht;
}

/* Insert into hash map, returns 1/0 for success failure */
int ht_put(HashTable *ht, char *key, int value) {

    Bucket newBucket;
    char *tempKey;
    int idx;

    /* If the hash table is full, don't insert */
    if (!ht || ht -> size == ht -> capacity) {
        fprintf(stderr, MALLOC_FAILURE);
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

/* Reomves entry, returns 1/0 for success/failure */
int ht_remove_entry(HashTable *ht, char *key) {

    char *tempkey;
    int idx, tempidx;
    
    if (!ht -> size) {
        fprintf(stderr, DELETE_FAILURE);
        return 0;
    }

    idx = (int) hash_function(key, ht -> capacity);
    tempkey = ht -> buckets[idx].key;
    /* Bucket has been found from hash function, just set key to NULL */
    if (tempkey && !strcmp(key, tempkey)) {
        ht -> buckets[idx].key = NULL;
        ht -> size--;
    }
    /* Else we must search for this key */
    else {
        tempidx = idx;
        while (1) {
            idx = (idx + 1) % ht -> capacity;
            tempkey = ht -> buckets[idx].key;
            if (tempkey) {
                /* We have found the key */
                if (!strcmp(key, tempkey)) {
                    ht -> buckets[idx].key = NULL;
                    ht -> size--;
                    break;
                }
            }
            /* We are back to where we started, so key must not exist in table */
            if (tempidx == idx) {
                fprintf(stderr, KEY_ERROR);
                break;
            }
        }

    }

    return 1;
}

/* Returns the keys of the hash */
char **ht_keys(HashTable *ht) {

    char **theKeys;
    int i, j;

    theKeys = (char **) malloc(sizeof(char *) * ht -> size);
    if (!theKeys) {
        fprintf(stderr, MALLOC_FAILURE);
        free((void *) theKeys);
        theKeys = NULL;
        return theKeys;
    }
    /* Loop over the hash table and add the keys to theKeys which are not NULL */
    for (i = 0, j = 0; i < ht -> capacity; i++) 
        if (ht -> buckets[i].key) {
            theKeys[i] = (char *) malloc(sizeof(ht -> buckets[i].key));
            if (!theKeys[i]) {
                fprintf(stderr, MALLOC_FAILURE);
                for (i = 0; i < j; i++) free((void *) theKeys[i]);
                free((void *) theKeys);
                theKeys = NULL;
                return theKeys;
            }
            theKeys[j++] = ht -> buckets[i].key;
        }

    return theKeys;
}

/* Print contents of hash map */
void ht_print(HashTable *ht) {

    Bucket b;
    int i;

    printf("[");
    if (!ht -> size) {
        printf("]\n");
        return;
    }
    /* Loop over the bucket array, printing the key-value pairs that have value */
    for (i = 0; i < ht -> capacity; i++) {
        if (ht -> buckets[i].key) 
            printf(" <%s, %d> ", ht -> buckets[i].key, ht -> buckets[i].value);
    } 
    printf("]\n");
    
}

/* Returns true if the hash table is empty */
int ht_is_empty(HashTable *ht) { return ht ? ht -> size == 0 : 0; }

/* Destroys a hash table objects */
void ht_destroy(HashTable *ht) {
    if(ht) {
        free((void *) ht -> buckets);
        free((void *) ht);
        ht = NULL;
    }
}

int main() {

    HashTable *ht;
    char **keys;
    int i;

    ht = ht_init(10);
    ht_put(ht, "hello", 1);
    ht_put(ht, "there", 2);
    ht_put(ht, "ther", 6);
    ht_put(ht, "the", 5);
    ht_put(ht, "theree", 4);
    ht_put(ht, "th", 3);
    ht_print(ht);
    keys = ht_keys(ht);
    for (i = 0; i < ht -> size; i++) printf("%s\n", keys[i]);
    ht_destroy(ht);
    free((void *) keys);
    for (i = 0; i < ht -> size; i++) free((void *) keys[i]);

    return 0;
}

