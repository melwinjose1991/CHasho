#ifndef HASH_HEADER_H
#define HASH_HEADER_H

// Code Ref : https://gist.github.com/tonious/1377667

struct entry_s {
	char *key;
	char *value;
	struct entry_s *next;
};
typedef struct entry_s entry_t;

struct hashtable_s {
	int size;
	struct entry_s **table;
};
typedef struct hashtable_s hashtable_t;

// initializes hashTable
hashtable_t *ht_create( int size );

entry_t *ht_create_entry( char *key, char *value );

void ht_set( hashtable_t *hashtable, char *key, char *value );

char *ht_get( hashtable_t *hashtable, char *key );

void ht_free( hashtable_t *hashtable );

#endif
