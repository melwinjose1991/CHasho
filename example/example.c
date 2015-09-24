#include<stdio.h>
#include"../src/chasho.h"

void main(){
	hashtable_t *table;
	char* value;	
	char* key = "1abc0";

	table = ht_create(16);
	ht_set(table, key, "Some Value");
	value = ht_get(table, key);

	ht_free(table);
}
