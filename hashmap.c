/************************************************
 * Hashmap which maps a string to an int        *
 * Uses linear probing for collision handling   *
 *                                              *
 ************************************************/

#include <stdio.h>
#include <stdbool.h>
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
