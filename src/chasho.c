#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "chasho.h"

#define DEBUG_HASH 0
#define DEBUG_HASH_MSG(msg, ...      )       if(DEBUG_HASH==1)printf(msg,## __VA_ARGS__);

/* Create a new hashtable. */
hashtable_t *ht_create( int size ) {
	DEBUG_HASH_MSG("HASH : ht_create() start\n");

	hashtable_t *hashtable = NULL;
	int i;

	if( size < 1 ) return NULL;

	/* Allocate the table itself. */
	if( ( hashtable = malloc( sizeof( hashtable_t ) ) ) == NULL ) {
		return NULL;
	}

	/* Allocate pointers to the head nodes. */
	if( ( hashtable->table = malloc( sizeof( entry_t * ) * size ) ) == NULL ) {
		return NULL;
	}
	for( i = 0; i < size; i++ ) {
		hashtable->table[i] = NULL;
	}

	hashtable->size = size;

	DEBUG_HASH_MSG("HASH : ht_create() end\n");
	return hashtable;
}

/* Hash a string for a particular hash table. */
int ht_hash( hashtable_t *hashtable, char *key ) {
	DEBUG_HASH_MSG("HASH : ht_hash() start\n");

	unsigned long int hashval=0;
	int i = 0;
	int len = strlen(key);
	char ch;
	int exp;
	/* Convert our string to an integer */
	for(i=0;i<len;i++){
		ch = key[i];
		exp = len - 1 - i;
		hashval+=(ch * pow((double)31, (double)exp));
	}
	DEBUG_HASH_MSG("HASH : ht_hash() : key:%s\thashval:%lu\tBUCKET:%lu\n", key, hashval, hashval % hashtable->size);

	DEBUG_HASH_MSG("HASH : ht_hash() end\n");
	return hashval % hashtable->size;
}

/* Create a key-value pair. */
entry_t *ht_create_entry( char *key, char *value ) {
	DEBUG_HASH_MSG("HASH : ht_create_entry() start\n");

	entry_t *newpair;

	if(strlen(key)<=0 || strlen(value)<=0){
		return NULL;
	}

	if( ( newpair = malloc( sizeof( entry_t ) ) ) == NULL ) {
		return NULL;
	}

	if( ( newpair->key = strdup( key ) ) == NULL ) {
		return NULL;
	}

	if( ( newpair->value = strdup( value ) ) == NULL ) {
		return NULL;
	}

	newpair->next = NULL;

	DEBUG_HASH_MSG("HASH : ht_create_entry() end\n");
	return newpair;
}

/* Insert a key-value pair into a hash table. */
void ht_set( hashtable_t *hashtable, char *key, char *value ) {
	DEBUG_HASH_MSG("HASH : ht_set() start\n");

	int bucket_index = 0;
	entry_t *newpair = NULL;
	entry_t *next = NULL;
	entry_t *last = NULL;

	bucket_index = ht_hash( hashtable, key );
	DEBUG_HASH_MSG("HASH : ht_set() : bucket_index:%d for pair(%s,%s)\n",bucket_index, key, value);

	next = hashtable->table[ bucket_index ];

	DEBUG_HASH_MSG("HASH : ht_set() started looping the chain\n");
	while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
		DEBUG_HASH_MSG("HASH : ht_set() : current_key:%s\n",next->key);
		last = next;
		next = next->next;
	}
	DEBUG_HASH_MSG("HASH : ht_set() : done looping the chain\n");

	/* There's already a pair.  Let's replace that string. */
	if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {
		DEBUG_HASH_MSG("HASH : ht_set() : KEY:%s already exists, replacing %s with new value %s\n", key, next->value, value);
		free( next->value );
		next->value = strdup( value );

	} else {	/* Nope, could't find it.  Time to grow a pair. */
		DEBUG_HASH_MSG("HASH : ht_set() : KEY not found, creating new-pair\n");
		newpair = ht_create_entry( key, value );

		/* We're at the start of the linked list in this bucket_index. */
		if( next == hashtable->table[ bucket_index ] ) {
			newpair->next = next;
			hashtable->table[ bucket_index ] = newpair;
			DEBUG_HASH_MSG("HASH : ht_set() : newPair inserted at the start of the chain\n");

		}else if ( next == NULL ) {	/* We're at the end of the linked list in this bucket_index. */
			last->next = newpair;
			DEBUG_HASH_MSG("HASH : ht_set() : newPair inserted at the end of the chain\n");

		} else  {	/* We're in the middle of the list. */
			DEBUG_HASH_MSG("HASH : ht_set() : newPair inserted between key:%s and key:%s\n", last->key, next->key);
			newpair->next = next;
			last->next = newpair;
		}
	}

	DEBUG_HASH_MSG("HASH : ht_set() end\n");
}

/* Retrieve a key-value pair from a hash table. */
char *ht_get( hashtable_t *hashtable, char *key ) {
	DEBUG_HASH_MSG("HASH : ht_get() : start\n");

	int bucket_index = 0;
	entry_t *pair;

	bucket_index = ht_hash( hashtable, key );
	DEBUG_HASH_MSG("HASH : ht_get() : for KEY:%s bucket_index:%d\n", key, bucket_index);

	/* Loop through the chain to find out key */
	pair = hashtable->table[ bucket_index ];
	DEBUG_HASH_MSG("HASH : ht_get() : started looping the chain\n");
	while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) > 0 ) {
		DEBUG_HASH_MSG("HASH : ht_get() : current_key:%s\n",pair->key);
		pair = pair->next;
	}
	DEBUG_HASH_MSG("HASH : ht_get() : done looping the chain\n");

	if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
		DEBUG_HASH_MSG("HASH : ht_get() : key NOT FOUND !!!\n");
		return NULL;
	} else {
		DEBUG_HASH_MSG("HASH : ht_get() : key FOUND !!!\n");
		return pair->value;
	}

	DEBUG_HASH_MSG("HASH : ht_get() : end\n");
}

/* Free the entries and the hash table itself */
void ht_free( hashtable_t *hashtable ){
	DEBUG_HASH_MSG("HASH : free_table() : start\n");

	entry_t *next = NULL, *temp;
	int i,buckets;
	if(hashtable == NULL){
		DEBUG_HASH_MSG("HASH : free_table() : hashtable EMPTY !!! \n");
		return;
	}
	buckets = hashtable->size;
	for(i=0;i<buckets;i++){	//loop through the buckets
		next = hashtable->table[ i ];
		while( next != NULL ){	//loop through each buckets entries
			DEBUG_HASH_MSG("HASH : free_table() :: current_key:%s:%s\t", next->key, next->value);
			free(next->key);
			free(next->value);
			temp = next;
			next = next->next;
			free(temp);
			DEBUG_HASH_MSG(":: FREEd key, value and the entry struct\n");
		}
		DEBUG_HASH_MSG("HASH : free_table() : FREEd bucket:%d\n", i);
	}
	free(hashtable->table);
	hashtable->table = NULL;
	free(hashtable);
	DEBUG_HASH_MSG("HASH : free_table() : FREEd buckets and the hashTable\n");

	DEBUG_HASH_MSG("HASH : free_table() : end\n");
}
