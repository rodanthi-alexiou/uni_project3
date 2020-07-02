//////////////////////////////////////////////////////////////////
//
// Unit tests για τον ADT Graph.
// Οποιαδήποτε υλοποίηση οφείλει να περνάει όλα τα tests.
//
//////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "ADTGraph.h"
#include "ADTMap.h"
#include "ADTList.h"



// Δημιουργούμε μια ειδική compare συνάρτηση
int compare_ints(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}


void test_create(void) {

	Graph graph = graph_create(compare_ints,NULL);
	//graph_set_hash_function(graph, hash_int);

	TEST_ASSERT(graph != NULL);
	TEST_ASSERT(graph_size(graph) == 0);
	
}

int* create_int(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}

void test_insert(void) {

	Graph graph = graph_create(compare_ints,NULL);
	graph_set_hash_function(graph, hash_int);
	

	for(int i = 0; i< 10; i++){
		Pointer ver = create_int(i);
		graph_insert_vertex(graph, ver);
		TEST_ASSERT(graph_size(graph) == i+1);
	}
	
}


void test_graph_get_vertices(void){

	Graph graph = graph_create(compare_ints,NULL);
	graph_set_hash_function(graph, hash_int);
	

	for(int i = 0; i< 10; i++){
		Pointer ver = create_int(i);
		graph_insert_vertex(graph, ver);
	}

List list = graph_get_vertices(graph);
TEST_ASSERT(list_size(list) == 10);

}

void test_graph_insert_edge(void){
	Graph graph = graph_create(compare_ints,NULL);
	graph_set_hash_function(graph, hash_int);

		for(int i = 0; i< 10; i++){
		Pointer ver = create_int(i);
		graph_insert_vertex(graph, ver);
	}

	graph_insert_edge(graph, create_int(0), create_int(1), 10);
	List list = graph_get_adjacent(graph, create_int(0));
	TEST_ASSERT(list_size(list) == 1);
//	TEST_ASSERT(graph_get_weight(graph, create_int(0), create_int(1))  == 10);

	list = graph_get_adjacent(graph, create_int(1));
	TEST_ASSERT(list_size(list) == 1);

}


void test_graph_remove_vertex(void){
	Graph graph = graph_create(compare_ints,NULL);
	graph_set_hash_function(graph, hash_int);

		for(int i = 0; i< 10; i++){
		Pointer ver = create_int(i);
		graph_insert_vertex(graph, ver);
	}

	graph_insert_edge(graph, create_int(0), create_int(1), 10);
	graph_insert_edge(graph, create_int(0), create_int(2), 10);
	graph_insert_edge(graph, create_int(1), create_int(2), 10);

	List list = graph_get_adjacent(graph, create_int(1));
	TEST_ASSERT(list_size(list) == 2);

	graph_remove_vertex(graph, create_int(0));
	TEST_ASSERT(list_size(list) == 1);

	graph_remove_edge(graph, create_int(1), create_int(2));
	TEST_ASSERT(list_size(list) == 0);


}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	// { "create", test_create },

	{ "graph_create", test_create },
	{ "graph_insert", test_insert },
	{ "graph_get_vertices", test_graph_get_vertices },
	{ "graph_insert_edge", test_graph_insert_edge },
	{ "graph_remove_vertex", test_graph_remove_vertex },
	
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
}; 