///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Graph μέσω λιστών γειτνίασης.
//
///////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>

#include "ADTGraph.h"
#include "ADTMap.h"
#include "ADTList.h"
#include "ADTPriorityQueue.h"
#include "ADTVector.h"


struct graph {
    Map vertex;
    int size;
    CompareFunc compare;
    DestroyFunc destroy_vertex;
    HashFunc hash_func;
};

struct adj_node{
    Pointer vertex;
    int weight;
    Graph owner;
};

struct pq_path{
    Pointer w;
    int dest;
};



int compare_adjnode(Pointer a, Pointer b){
    AdjNode one = a;
    AdjNode two = b;
    return one->owner->compare(one->vertex, two->vertex);
}



AdjNode find_adjnode(List list, Pointer vertex1){
    for(ListNode node = list_first(list); node!=LIST_EOF; node = list_next(list, node)){
        AdjNode search = list_node_value(list, node);
        if(search->owner->compare(search->vertex , vertex1) == 0){
            return search;
        }
    }

return NULL;
}


AdjNode find_adjnode_pq(PriorityQueue pq, Pointer vertex){
//    while(pq != NULL){
       // AdjNode max = pqueue_max(pq);
        //if(max->owner->compare(max->vertex, vertex) == 0){
      //      return max;
    //    }
  //      pqueue_remove_max(pq);
//    }

return NULL;
}


// Δημιουργεί και επιστρέφει ένα γράφο, στον οποίο τα στοιχεία (οι κορυφές)
// συγκρίνονται με βάση τη συνάρτηση compare. Αν destroy_vertex != NULL, τότε
// καλείται destroy_vertex(vertex) κάθε φορά που αφαιρείται μια κορυφή.

Graph graph_create(CompareFunc compare, DestroyFunc destroy_vertex){
    Graph graph = malloc(sizeof(*graph));

    graph->compare = compare;

    graph->destroy_vertex = destroy_vertex;
    
   
    graph->vertex =map_create(compare, NULL, NULL);


    graph->size = 0;

    return graph;
}

// Επιστρέφει τον αριθμό στοιχείων (κορυφών) που περιέχει ο γράφος graph.

int graph_size(Graph graph){
    return graph->size;
}

// Προσθέτει μια κορυφή στο γράφο.

void graph_insert_vertex(Graph graph, Pointer vertex){

    List list_v = malloc(sizeof(list_v));
    list_v = list_create(NULL);
    

    map_insert(graph->vertex, vertex, list_v);
    graph->size++;
}

// Επιστρέφει λίστα με όλες τις κορυφές του γράφου. Η λίστα δημιουργείται σε κάθε
// κληση και είναι ευθύνη του χρήστη να κάνει list_destroy.

List graph_get_vertices(Graph graph){
    List list_vertex = list_create(NULL);

    for(MapNode node = map_first(graph->vertex); node!= MAP_EOF; node = map_next(graph->vertex, node)){
        Pointer key = map_node_key(graph->vertex, node);
        list_insert_next(list_vertex, LIST_EOF, key);
    }

    return list_vertex;
}

// Διαγράφει μια κορυφή από τον γράφο (αν υπάρχουν ακμές διαγράφονται επίσης).

void graph_remove_vertex(Graph graph, Pointer vertex){
    MapNode removed = map_find_node(graph->vertex, vertex);
    List removed_list = map_node_value(graph->vertex, removed);

    if(list_size(removed_list) != 0){
        for(ListNode node = list_first(removed_list); node!= LIST_EOF; node = list_next(removed_list, node)){

            AdjNode vertex_adjnode = list_node_value(removed_list, node);

            Pointer vertex_node = vertex_adjnode->vertex;

            MapNode find_vertex = map_find_node(graph->vertex, vertex_node);

            List to_removed_list = map_node_value(graph->vertex, find_vertex);


            AdjNode search = find_adjnode(to_removed_list, vertex);
            ListNode to_remove = list_find_node(to_removed_list, search , (CompareFunc) compare_adjnode);

            list_remove(to_removed_list, to_remove);
        }
    }

    map_remove(graph->vertex, vertex);
}

// Προσθέτει μια ακμή με βάρος weight στο γράφο.

void graph_insert_edge(Graph graph, Pointer vertex1, Pointer vertex2, uint weight){
    AdjNode node1 = malloc(sizeof(node1));
    node1->vertex = vertex1;
    node1->weight = weight;
    node1->owner = graph;
    MapNode node_search = map_find_node(graph->vertex, vertex2);
    List search = map_node_value(graph->vertex, node_search);
    list_insert_next(search, LIST_EOF, node1);

    AdjNode node2 = malloc(sizeof(node2));
    node2->vertex = vertex2;
    node2->weight = weight;
    node2->owner = graph;


    node_search = map_find_node(graph->vertex, vertex1);
    search = map_node_value(graph->vertex, node_search);
    list_insert_next(search, LIST_EOF, node2);
}

// Αφαιρεί μια ακμή από το γράφο.

void graph_remove_edge(Graph graph, Pointer vertex1, Pointer vertex2){
    MapNode removed = map_find_node(graph->vertex, vertex1);
    List removed_list = map_node_value(graph->vertex, removed);
    AdjNode to_remove = find_adjnode(removed_list, vertex2);
    ListNode node_removed = list_find_node(removed_list, to_remove, (CompareFunc) compare_adjnode);
    list_remove(removed_list, node_removed);

    removed = map_find_node(graph->vertex, vertex2);
    removed_list = map_node_value(graph->vertex, removed);
    to_remove = find_adjnode(removed_list, vertex1);
    node_removed = list_find_node(removed_list, to_remove, (CompareFunc) compare_adjnode);
    list_remove(removed_list, node_removed);

}

// Επιστρέφει το βάρος της ακμής ανάμεσα στις δύο κορυφές, ή UINT_MAX αν δεν είναι γειτονικές.

uint graph_get_weight(Graph graph, Pointer vertex1, Pointer vertex2){
    MapNode node = map_find_node(graph->vertex, vertex1);
    List list = map_node_value(graph->vertex, node);

    AdjNode search = find_adjnode(list, vertex1);

    return search->weight;
}



// Επιστρέφει λίστα με τους γείτονες μιας κορυφής. Η λίστα δημιουργείται σε κάθε
// κληση και είναι ευθύνη του χρήστη να κάνει list_destroy.

List graph_get_adjacent(Graph graph, Pointer vertex){
    MapNode node = map_find_node(graph->vertex, vertex);
    List list = map_node_value(graph->vertex, node);
    return list;
}

// Επιστρέφει (σε λίστα) το συντομότερο μονοπάτι ανάμεσα στις κορυφές source και
// target, ή κενή λίστα αν δεν υπάρχει κανένα μονοπάτι. Η λίστα δημιουργείται σε
// κάθε κληση και είναι ευθύνη του χρήστη να κάνει list_destroy.

List graph_shortest_path(Graph graph, Pointer source, Pointer target){

    //Map dist = map_create(graph->compare, NULL, NULL);
   // map_set_hash_function(dist, graph->hash_func);
    //Map prev = map_create(graph->compare, NULL, NULL);
  //  map_set_hash_function(prev, graph->hash_func);


//    map_insert(dist, source, create_int(0));
    //map_insert(prev, source, NULL);


    //PriorityQueue pq = pqueue_create((CompareFunc) compare_adjnode_destination, NULL, NULL);

   //     MapNode node_source = map_find_node(graph->vertex, source);

 //       AdjNode pq_path_node = malloc(sizeof(pq_path_node));
     //   pq_path_node->w = map_node_key(graph->vertex, node_source);
       // pq_path_node->weight = 0;
      //  pq_path_node->owner = graph;
    //    PriorityQueueNode pq_node =  pqueue_insert(pq, pq_path_node);

  //      map_insert(array, pq_path_node->w, pq_path_node->w);

//        while(pq != NULL){
           // AdjNode w_node = pqueue_node_value(pq, pq_node);
          //  Pointer w_pointer = w_node->vertex;
            //MapNode find = map_find_node(graph->map, w_pointer);
          //  List neighbors = map_node_value(graph->map, find);

        //    for(ListNode list_node = list_first(neighbors); list_node != LIST_EOF; list_node = list_next(neighbors, list_node)){
      //          AdjNode temp = list_node_value(neighbors, list_node);
    //            int alt = w_node->weight + temp->weight;

  //              if(map_find_node(dist, temp->vertex) == MAP_EOF || (graph->compare(map_node_value(dist,  {
                   
//return NULL;
        //        }
      //              MapNode dist_node = map_find_node(dist, temp->vertex);
    //                Pointer dist_neighbor = map_node_value(dist, dist_node);



  //          }

            




//        }

return NULL;
}

// Ελευθερώνει όλη τη μνήμη που δεσμεύει ο γράφος.
// Οποιαδήποτε λειτουργία πάνω στο γράφο μετά το destroy είναι μη ορισμένη.

void graph_destroy(Graph graph);



void graph_set_hash_function(Graph graph, HashFunc hash_func){
    map_set_hash_function(graph->vertex, hash_func);
}
