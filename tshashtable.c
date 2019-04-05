/*************************************************
 * - Thread safe TSHashTable which maps a string *
 *   to an int                                   *
 * - Uses linear probing for collision handling  *
 *                                               *
 *************************************************/
#include "tshashtable.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INVALID_ARGS "Error: command line arguments entered are wrong\n"
#define COND_ERROR   "Error: condition variable failed to create\n"
#define MALLOC_ERROR "Error: memory allocation failed\n"
#define MUTEX_ERROR  "Error: mutex failed ot create\n"
#define DELETE_ERROR "Error: hash table is empty\n"
#define KEY_ERROR    "Error: key is not in table\n"
#define NULL_VALUE   "Error: hash table is NULL\n"

/* Entry in bucket array */
struct bucket {
  char *key; // key which maps to a value
  int value; // the value being mapped to
};

/* Structure that holds buckets */
struct ts_hashtable {
  int done;        // boolean value which will tell us if we are done (to avoid
                   // deadlock)
  int size;        // size of hash table
  int capacity;    // capacity of the hash table
  Bucket *buckets; // the bucket array
  pthread_mutex_t mutex; // mutex used to protect shared data
  pthread_cond_t insert; // condtion variable uesd in inserting
  pthread_cond_t delete; // condition variable used in deleting
};

/* An iterator for the hash map */
struct ts_hashtable_iterator {
  TSHashTable *ht;       // the hash table
  unsigned long next;    // index of where to start from next in the hash table
  pthread_mutex_t mutex; // mutex used to protect shared data
};

/* Hash function used to compute index into bucket array */
static unsigned long hash_function(char *key, int capacity) {
  unsigned long hash;
  int c;
  hash = 5381;
  while ((c = *key++))
    hash = ((hash << 5) + hash) + c;
  return hash % capacity;
}

/* Returns a pointer to an empty hash table if successfull, NULL otherwise */
TSHashTable *ht_init(int capacity) {
  TSHashTable *ht;
  int i;
  ht = (TSHashTable *)malloc(sizeof(TSHashTable));
  /* Print error message and return NULL if memory allocation fails */
  if (!ht) {
    fprintf(stderr, MALLOC_ERROR);
    free((void *)ht);
    ht = NULL;
    return ht;
  }
  ht->done = 0;
  ht->size = 0;
  ht->capacity = capacity;
  ht->buckets = (Bucket *)calloc(capacity, sizeof(Bucket));
  if (!ht->buckets) {
    fprintf(stderr, MALLOC_ERROR);
    free((void *)ht->buckets);
    free((void *)ht);
    ht = NULL;
    return ht;
  }
  /* Print error message and return NULL if mutex fails to create */
  if (pthread_mutex_init(&(ht->mutex), NULL)) {
    fprintf(stderr, MUTEX_ERROR);
    free((void *)ht->buckets);
    free((void *)ht);
    ht = NULL;
    return ht;
  }
  /* Print error message and return NULL if condition variable fails to create
   */
  if (pthread_cond_init(&(ht->insert), NULL)) {
    fprintf(stderr, COND_ERROR);
    pthread_mutex_destroy(&(ht->mutex));
    free((void *)ht->buckets);
    free((void *)ht);
    ht = NULL;
    return ht;
  }
  if (pthread_cond_init(&(ht->delete), NULL)) {
    fprintf(stderr, COND_ERROR);
    pthread_mutex_destroy(&(ht->mutex));
    pthread_cond_destroy(&(ht->insert));
    free((void *)ht->buckets);
    free((void *)ht);
    ht = NULL;
  }
  return ht;
}

/* Insert into hash map, returns 1/0 for success failure */
int ht_put(TSHashTable *ht, char *key, int value) {
  Bucket newBucket;
  char *tempKey;
  int idx;
  if (!ht) {
    fprintf(stderr, NULL_VALUE);
    return 0;
  }
  /* Critical region */
  pthread_mutex_lock(&(ht->mutex));
  /* Wait until there is an availabe space in the hash table to insert a new
   * bucket */
  while (ht->size == ht->capacity && !(ht->done)) {
    printf("== Hash Table is full ==");
    pthread_cond_wait(&(ht->insert), &(ht->mutex));
  }
  if (ht->size < ht->capacity) {
    newBucket.key = key;
    newBucket.value = value;
    /* Compute the index */
    idx = (int)hash_function(key, ht->capacity);
    tempKey = ht->buckets[idx].key;
    /* If the space is empty, insert into the array and we are done */
    if (!tempKey) {
      ht->buckets[idx].key = key;
      ht->buckets[idx].value = value;
    }
    /* If there is a duplicate key, then overwrite it */
    else if (!strcmp(tempKey, key))
      ht->buckets[idx].value = value;
    /* The space is already taken, so do a linear search to find the next free
     * space */
    else {
      while (1) {
        /* Move along 1 (or start from the beginning) */
        idx = (idx + 1) % ht->capacity;
        /* Free space has been found */
        if (!(ht->buckets[idx].key)) {
          ht->buckets[idx].key = key;
          ht->buckets[idx].value = value;
          break;
        }
      }
    }
    ht->size++;
    pthread_cond_signal(&(ht->delete));
  }
  pthread_mutex_unlock(&(ht->mutex));
  return 1;
}

/* Reomves entry, returns 1/0 for success/failure */
int ht_remove_entry(TSHashTable *ht, char *key) {
  int idx, tempidx;
  char *tempKey;
  if (!ht) {
    fprintf(stderr, NULL_VALUE);
    return 0;
  }
  pthread_mutex_lock(&(ht->mutex));
  /* Wait until there is something to be deleted */
  while (!(ht->size) && !(ht->done)) {
    printf("== Hash Table is empty ==");
    pthread_cond_wait(&(ht->delete), &(ht->mutex));
  }
  if (ht->size) {
    idx = (int)hash_function(key, ht->capacity);
    tempKey = ht->buckets[idx].key;
    /* Bucket has been found from hash function, just set key to NULL */
    if (tempKey && !strcmp(key, tempKey)) {
      ht->buckets[idx].key = NULL;
      ht->size--;
    }
    /* Else we must search for this key */
    else {
      tempidx = idx;
      while (1) {
        idx = (idx + 1) % ht->capacity;
        tempKey = ht->buckets[idx].key;
        if (tempKey)
          /* We have found the key */
          if (!strcmp(key, tempKey)) {
            ht->buckets[idx].key = NULL;
            ht->size--;
            break;
          }
        /* We are back to where we started, so key must not exist in table */
        if (tempidx == idx) {
          fprintf(stderr, KEY_ERROR);
          break;
        }
      }
    }
    pthread_cond_signal(&(ht->insert));
  }
  pthread_mutex_unlock(&(ht->mutex));
  return 1;
}

/* Returns the keys of the hash */
char **ht_keys(TSHashTable *ht) {
  char **theKeys;
  int i, j;
  theKeys = (char **)malloc(sizeof(char *) * ht->size);
  if (!theKeys) {
    fprintf(stderr, MALLOC_ERROR);
    free((void *)theKeys);
    theKeys = NULL;
    return theKeys;
  }
  pthread_mutex_lock(&(ht->mutex));
  /* Loop over the hash table and add the keys to theKeys which are not NULL */
  for (i = 0, j = 0; i < ht->capacity; i++)
    if (ht->buckets[i].key) {
      theKeys[i] = (char *)malloc(sizeof(ht->buckets[i].key));
      if (!theKeys[i]) {
        fprintf(stderr, MALLOC_ERROR);
        /* Delete all pointers allocated to avoid memory leaks */
        for (i = 0; i < j; i++) {
          free((void *)theKeys[i]);
          theKeys[i] = NULL;
        }
        free((void *)theKeys);
        theKeys = NULL;
        return theKeys;
      }
      theKeys[j++] = ht->buckets[i].key;
    }
  pthread_mutex_unlock(&(ht->mutex));
  return theKeys;
}

/* Print contents of hash map */
void ht_print(TSHashTable *ht) {
  int i, value;
  char *key;
  Bucket b;
  printf("[");
  if (!ht) {
    fprintf(stderr, NULL_VALUE);
    return;
  }
  pthread_mutex_lock(&(ht->mutex));
  if (!ht->size) {
    printf("]\n");
    return;
  }
  /* Loop over the bucket array, printing the key-value pairs that have value */
  for (i = 0; i < ht->capacity; i++)
    if (ht->buckets[i].key) {
      key = ht->buckets[i].key;
      value = ht->buckets[i].value;
      printf(" <%s, %d> ", key, value);
    }
  pthread_mutex_unlock(&(ht->mutex));
  printf("]\n");
}

/* Returns true if the hash table is empty */
int ht_is_empty(TSHashTable *ht) {
  int result;
  pthread_mutex_lock(&(ht->mutex));
  result = ht ? ht->size == 0 : 0;
  pthread_mutex_unlock(&(ht->mutex));
  return result;
}

/* Set done to be true so we can notify threads */
void ht_set_done_true(TSHashTable *ht) { ht ? ht->done = 1 : NULL; }

/* Sets done to be false so we can notify threads there is more work */
void ht_set_done_false(TSHashTable *ht) { ht ? ht->done = 0 : NULL; }

/* Destroys a hash table objects */
void ht_destroy(TSHashTable *ht) {
  if (ht) {
    pthread_mutex_destroy(&(ht->mutex));
    pthread_cond_destroy(&(ht->insert));
    pthread_cond_destroy(&(ht->delete));
    free((void *)ht->buckets);
    free((void *)ht);
    ht = NULL;
  }
}

/* Initialises hash map iterator */
TSHashTableIterator *ht_iter_init(TSHashTable *ht) {
  TSHashTableIterator *hti;
  hti = (TSHashTableIterator *)malloc(sizeof(TSHashTableIterator));
  if (!hti) {
    fprintf(stderr, MALLOC_ERROR);
    free((void *)hti);
    hti = NULL;
    return hti;
  }
  hti->next = 0;
  hti->ht = ht;
  hti->mutex = ht->mutex;
  pthread_mutex_lock(&(hti->mutex));
  return hti;
}

/* Returns the next available bucket in the hash table */
Bucket *ht_iter_next(TSHashTableIterator *hti) {
  Bucket *bucket, *buckets;
  int i, size, capacity;
  buckets = hti->ht->buckets;
  capacity = hti->ht->capacity;
  size = hti->ht->size;
  bucket = NULL;
  if (size)
    /* Find the first available bucket starting at index hti->next */
    for (i = hti->next; i < capacity; i++)
      /*
       * Bucket has been found, update the value of next and set this value
       * to bucket and then break so we can return
       */
      if (i < capacity)
        if (buckets[i].key) {
          hti->next = i + 1;
          bucket = &buckets[i];
          break;
        }
  return bucket;
}

/* Destorys an iterator object */
void ht_iter_destroy(TSHashTableIterator *hti) {
  if (hti) {
    pthread_mutex_unlock(&(hti->mutex));
    pthread_mutex_destroy(&(hti->mutex));
    free((void *)hti);
    hti = NULL;
  }
}
