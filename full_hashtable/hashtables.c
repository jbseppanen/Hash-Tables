#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Hash table key/value pair with linked list pointer.

  Note that an instance of `LinkedPair` is also a node in a linked list.
  More specifically, the `next` field is a pointer pointing to the the 
  next `LinkedPair` in the list of `LinkedPair` nodes. 
 */
int dbg_counter = 0;
typedef struct LinkedPair
{
  char *key;
  char *value;
  struct LinkedPair *next;
} LinkedPair;

/*
  Hash table with linked pairs.
 */
typedef struct HashTable
{
  int capacity;
  LinkedPair **storage;
} HashTable;

/*
  Create a key/value linked pair to be stored in the hash table.
 */
LinkedPair *create_pair(char *key, char *value)
{
  LinkedPair *pair = malloc(sizeof(LinkedPair));
  pair->key = strdup(key);
  pair->value = strdup(value);
  pair->next = NULL;

  return pair;
}

/*
  Use this function to safely destroy a hashtable pair.
 */
void destroy_pair(LinkedPair *pair)
{
  if (pair != NULL)
  {
    free(pair->key);
    free(pair->value);
    free(pair);
  }
}

/*
  djb2 hash function

  Do not modify this!
 */
unsigned int hash(char *str, int max)
{
  unsigned long hash = 5381;
  int c;
  unsigned char *u_str = (unsigned char *)str;

  while ((c = *u_str++))
  {
    hash = ((hash << 5) + hash) + c;
  }

  return hash % max;
}

/*
  Fill this in.

  All values in storage should be initialized to NULL
 */
HashTable *create_hash_table(int capacity)
{
  HashTable *ht = malloc(sizeof(HashTable));
  ht->storage = calloc(capacity, sizeof(LinkedPair *));
  ht->capacity = capacity;
  return ht;
}

/*
  Fill this in.

  Inserting values to the same index with different keys should be
  added to the corresponding LinkedPair list.

  Inserting values to the same index with existing keys can overwrite
  the value in th existing LinkedPair list.
 */
void hash_table_insert(HashTable *ht, char *key, char *value)
{
  int index = hash(key, ht->capacity);
  if (ht->storage[index] != NULL)
  {
    LinkedPair *next_pair = ht->storage[index];
    while (next_pair->next != NULL)
    {
      if (strcmp(next_pair->next->key, key) == 0)
      {
        next_pair = next_pair->next;
        free(next_pair->value);
        next_pair->value = strdup(value);
        return;
      }
      else
      {
        next_pair = next_pair->next;
      }
    }
    LinkedPair *pair = create_pair(key, value);
    next_pair->next = pair;
  }
  else
  {
    ht->storage[index] = create_pair(key, value);
  }
}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys and remove matching LinkedPairs safely.

  Don't forget to free any malloc'ed memory!
 */
void hash_table_remove(HashTable *ht, char *key)
{
  int index = hash(key, ht->capacity);
  if (ht->storage[index] != NULL)
  {
    LinkedPair *pair = ht->storage[index];
    LinkedPair *previous_pair = NULL;
    while (pair != NULL)
    {
      if (strcmp(pair->key, key) == 0)
      {
        LinkedPair *next_pair = pair->next;
        destroy_pair(pair);
        if (previous_pair != NULL)
        {
          previous_pair->next = next_pair;
        }
        return;
      }
      else
      {
        previous_pair = pair;
        pair = pair->next;
      }
    }
  }
  fprintf(stderr, "Unable to remove entry with key: %s\n", key);
}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys.

  Return NULL if the key is not found.
 */
char *hash_table_retrieve(HashTable *ht, char *key)
{
  int index = hash(key, ht->capacity);
  if (ht->storage[index] != NULL)
  {
    if (strcmp(ht->storage[index]->key, key) == 0)
    {
      return ht->storage[index]->value;
    }
    else
    {
      LinkedPair *next_pair = ht->storage[index]->next;
      while (next_pair != NULL)
      {
        if (strcmp(next_pair->key, key) == 0)
        {
          return next_pair->value;
        }
        else
        {
          next_pair = next_pair->next;
        }
      }
    }
  }
  fprintf(stderr, "Unable to find entry with key: %s\n", key);
  return NULL;
}

/*
  Fill this in.

  Don't forget to free any malloc'ed memory!
 */
void destroy_hash_table(HashTable *ht)
{
  for (int i = 0; i < ht->capacity; i++)
  {
    if (ht->storage[i] != NULL)
    {
      LinkedPair *pair = ht->storage[i];
      LinkedPair *next_pair = ht->storage[i];
      while (next_pair != NULL)  
      {
        pair = next_pair;
        next_pair = next_pair->next;
        destroy_pair(pair);
      }
    }
  }
  free(ht->storage);
  free(ht);
}

/*
  Fill this in.

  Should create a new hash table with double the capacity
  of the original and copy all elements into the new hash table.

  Don't forget to free any malloc'ed memory!
 */
HashTable *hash_table_resize(HashTable *ht)
{
  HashTable *new_ht = create_hash_table(ht->capacity * 2);
  for (int i = 0; i < ht->capacity; i++)
  {
    LinkedPair *pair = ht->storage[i];
    while (pair != NULL)
    {
        hash_table_insert(new_ht, ht->storage[i]->key, ht->storage[i]->value);
        pair = pair->next;
    }
  }
  destroy_hash_table(ht);
  return new_ht;
}

#ifndef TESTING
int main(void)
{
  struct HashTable *ht = create_hash_table(2);

  hash_table_insert(ht, "line_1", "Tiny hash table\n");
  hash_table_insert(ht, "line_2", "Filled beyond capacity\n");
  hash_table_insert(ht, "line_3", "Linked list saves the day!\n");

  printf("%s", hash_table_retrieve(ht, "line_1"));
  printf("%s", hash_table_retrieve(ht, "line_2"));
  printf("%s", hash_table_retrieve(ht, "line_3"));

  int old_capacity = ht->capacity;
  ht = hash_table_resize(ht);
  int new_capacity = ht->capacity;

  printf("Resizing hash table from %d to %d.\n", old_capacity, new_capacity);

  destroy_hash_table(ht);

  return 0;
}
#endif
