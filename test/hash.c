#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "list.h"

#define TABLE_SIZE 64

struct counter {
  int key;
  int value;
  struct hlist_node node;
};

struct hlist_head hashtable[TABLE_SIZE];
pthread_mutex_t locktable[TABLE_SIZE];

int hash_function(int key) {
	int a = 7;
	int b = 17;
	int p = 127;
	int hash = (((a * key) + b)) % p;
	return hash % TABLE_SIZE;
}

void init() {	
	for (int i = 0; i < TABLE_SIZE; i++) {
		INIT_HLIST_HEAD(&hashtable[i]);
		if(pthread_mutex_init(&locktable[i], NULL) != 0) {
			printf("ERROR: mutex init error\n");
			exit(1);
		}
	}
}

void insert(int key, int value) {
	int hashkey = hash_function(key);
	pthread_mutex_unlock(&locktable[hashkey]);
	struct counter *ref = (struct counter*)malloc(sizeof(struct counter));
	ref->key = key;
	ref->value = value;

	hlist_add_head(&ref->node, &hashtable[hashkey]);
	pthread_mutex_unlock(&locktable[hashkey]);
}

int delete(int key) {
	int hashkey = hash_function(key);
	pthread_mutex_lock(&locktable[hashkey]);
	struct counter *iter;
	struct hlist_node *find_node;
	
	hlist_for_each_entry(iter, find_node, &hashtable[hashkey], node) {
		if(iter->key == key) {
			hlist_del(&iter->node);
			free(iter);			
			pthread_mutex_unlock(&locktable[hashkey]);
			return 0;
		}
	}
	pthread_mutex_unlock(&locktable[hashkey]);
	return -1;
}

struct counter *find(int key) {
	int hashkey = hash_function(key);
	pthread_mutex_lock(&locktable[hashkey]);	
	struct counter *iter;
	struct hlist_node *find_node;
	
	hlist_for_each_entry(iter, find_node, &hashtable[hashkey], node) {
		if(iter->key ==key){
			pthread_mutex_unlock(&locktable[hashkey]);
			return iter;
		}
	}
	pthread_mutex_unlock(&locktable[hashkey]);
	return NULL;
}

int main() {
	init();

	for(int i=0; i<1000; i++) {
		insert(i, i*2 -1);
	}
	for(int i=0; i<100; i++) {
		delete(i);
	}
	for(int i=0; i<101; i++) {
		struct counter *tmp = find(i);
		if(tmp) {
			printf("%d\n", tmp->value);
		}
	}
	return 0;
}
