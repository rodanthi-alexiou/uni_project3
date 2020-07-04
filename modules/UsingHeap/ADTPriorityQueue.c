///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Priority Queue μέσω σωρού.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


#include "ADTPriorityQueue.h"
#include "ADTVector.h"			// Η υλοποίηση του PriorityQueue χρησιμοποιεί Vector

// Ενα PriorityQueue είναι pointer σε αυτό το struct
struct priority_queue {
	Vector vector;				// Τα δεδομένα, σε Vector ώστε να έχουμε μεταβλητό μέγεθος χωρίς κόπο
	CompareFunc compare;		// Η διάταξη
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του vector.
};

struct priority_queue_node{
	Pointer value;			//το value του κόμβου
	int pos;				//η θέση του κόμβου στο heap που ξεκινάει από το 0
	PriorityQueue owner;	//σε ποιό priority queue ανήκουν οι κόμβοι
};


// Βοηθητικές συναρτήσεις ////////////////////////////////////////////////////////////////////////////

// Προσοχή: στην αναπαράσταση ενός complete binary tree με πίνακα, είναι βολικό τα ids των κόμβων να
// ξεκινάνε από το 1 (ρίζα), το οποίο απλοποιεί τις φόρμουλες για εύρεση πατέρα/παιδιών. Οι θέσεις
// ενός vector όμως ξεκινάνε από το 0. Θα μπορούσαμε απλά να αφήσουμε μία θέση κενή, αλλά δεν είναι ανάγκη,
// μπορούμε απλά να αφαιρούμε 1 όταν διαβάζουμε/γράφουμε στο vector. Για απλοποίηση του κώδικα, η
// πρόσβαση στα στοιχεία του vector γίνεται από τις παρακάτω 2 βοηθητικές συναρτήσεις.


//συνάρτηση για την σύγκριση των value δύο κόμβων
static int compare_pq_nodes(Pointer a, Pointer b){
	PriorityQueueNode one = a;
	PriorityQueueNode two = b;
	return one->owner->compare(one->value,two->value);
}

// Επιστρέφει την τιμή του κόμβου node_id
static Pointer node_value(PriorityQueue pqueue, int node_id) {
	// τα node_ids είναι 1-based, το node_id αποθηκεύεται στη θέση node_id - 1
	PriorityQueueNode node = vector_get_at(pqueue->vector, (node_id-1));	//βρίσκω το pointer στην θέση του vector που βρίσκεται ο κόμβους που ψάχνουμε

	return node == NULL ? NULL : node->value;
}

// Ανταλλάσει τις τιμές των κόμβων που βρίσκονται στις θέσεις node_id1 και node_id2
static void node_swap(PriorityQueue pqueue, int node_id1, int node_id2) {
	// τα node_ids είναι 1-based, το node_id αποθηκεύεται στη θέση node_id - 1
	Pointer value1 = node_value(pqueue, node_id1);
	Pointer value2 = node_value(pqueue, node_id2);
	PriorityQueueNode node1 = pqueue_insert(pqueue, value1); //βρίσκει τους κόμβους με το αντίστοιχο value
	PriorityQueueNode node2 = pqueue_insert(pqueue, value2);
	vector_set_at(pqueue->vector, node_id1-1, node2); //τους ανταλλάσει στην καινούρια θέση
	vector_set_at(pqueue->vector, node_id2-1, node1);
	node1->pos = (node_id2 -1); //αλλάζει το pos των κόμβων
	node2->pos = (node_id1 -1); 

}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από
//       τον node_id που μπορεί να είναι _μεγαλύτερος_ από τον πατέρα του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.

static void bubble_up(PriorityQueue pqueue, int node_id) {

	// Αν φτάσαμε στη ρίζα, σταματάμε
		if(node_id == 1){
		return;
	}
	int parent = node_id / 2;		// Ο πατέρας του κόμβου. Τα node_ids είναι 1-based

	// Αν ο πατέρας έχει μικρότερη τιμή από τον κόμβο, swap και συνεχίζουμε αναδρομικά προς τα πάνω
	if(pqueue->compare(node_value(pqueue, parent), node_value(pqueue, node_id)) < 0) {
		node_swap(pqueue, parent, node_id);
		bubble_up(pqueue, parent);
	}

}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από τον
//       node_id που μπορεί να είναι _μικρότερος_ από κάποιο από τα παιδιά του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.

static void bubble_down(PriorityQueue pqueue, int node_id) {
	// βρίσκουμε τα παιδιά του κόμβου (αν δεν υπάρχουν σταματάμε)
	int left_child = 2 * node_id;
	int right_child = left_child + 1;

	int size = pqueue_size(pqueue);
	if (left_child > size)
		return;

	// βρίσκουμε το μέγιστο από τα 2 παιδιά
	int max_child = left_child;
	if (right_child <= size && pqueue->compare(node_value(pqueue, left_child), node_value(pqueue, right_child)) < 0)
			max_child = right_child;

	// Αν ο κόμβος είναι μικρότερος από το μέγιστο παιδί, swap και συνεχίζουμε προς τα κάτω
	if (pqueue->compare(node_value(pqueue, node_id), node_value(pqueue, max_child)) < 0) {
	
		node_swap(pqueue, node_id, max_child);
		bubble_down(pqueue, max_child);}
	}

// Αρχικοποιεί το σωρό από τα στοιχεία του vector values.

static void naive_heapify(PriorityQueue pqueue, Vector values) {
	// Απλά κάνουμε insert τα στοιχεία ένα ένα.

	int size = vector_size(values);
	for (int i = 0; i < size; i++){
		 pqueue_insert(pqueue, vector_get_at(values, i));
	
	}
}




// Συναρτήσεις του ADTPriorityQueue //////////////////////////////////////////////////

PriorityQueue pqueue_create(CompareFunc compare, DestroyFunc destroy_value, Vector values) {
	assert(compare != NULL);	// LCOV_EXCL_LINE

	PriorityQueue pqueue = malloc(sizeof(*pqueue));
	pqueue->compare = compare;
	pqueue->destroy_value = destroy_value;

	// Δημιουργία του vector που αποθηκεύει τα στοιχεία.
	// ΠΡΟΣΟΧΗ: ΔΕΝ περνάμε την destroy_value στο vector!
	// Αν την περάσουμε θα καλείται όταν κάνουμε swap 2 στοιχεία, το οποίο δεν το επιθυμούμε.
	pqueue->vector = vector_create(0, NULL);
	
	// Αν values != NULL, αρχικοποιούμε το σωρό.
	if (values != NULL){
		naive_heapify(pqueue, values);
	}
	return pqueue;
}

int pqueue_size(PriorityQueue pqueue) {
	return vector_size(pqueue->vector);
}

Pointer pqueue_max(PriorityQueue pqueue) {
	return node_value(pqueue, 1);		// root
}

PriorityQueueNode pqueue_insert(PriorityQueue pqueue, Pointer value) {
	struct priority_queue_node search_node = {.value = value, .owner = pqueue};		//βρίσκω τον κόμβο με το value στο pqueue που δίνεται ως όρισμα
	
	PriorityQueueNode node = vector_find(pqueue->vector, &search_node,(CompareFunc)compare_pq_nodes); //αναζητώ τον κόμβο στο heap μέσω του vector
	if( node != NULL){		//αν υπάρχει ο κόμβος απλώς τον επιστρέφω
		return node;
	} 
	//αν δεν υπάρχει δημιουργώ καινούριο	
	node = malloc(sizeof(*node));



	node->value = value;
	
	node->pos = pqueue_size(pqueue);	//τον βάζω στο τέλος
	
	node->owner = pqueue;

	vector_insert_last(pqueue->vector, node);
	// Ολοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού εκτός από τον τελευταίο, που μπορεί να είναι
	// μεγαλύτερος από τον πατέρα του. Αρα μπορούμε να επαναφέρουμε την ιδιότητα του σωρού καλώντας
	// τη bubble_up γα τον τελευταίο κόμβο (του οποίου το 1-based id ισούται με το νέο μέγεθος του σωρού).
	bubble_up(pqueue, pqueue_size(pqueue));

	return node;
}

void pqueue_remove_max(PriorityQueue pqueue) {
	int last_node = pqueue_size(pqueue);
	assert(last_node != 0);		// LCOV_EXCL_LINE

	// Destroy την τιμή που αφαιρείται
	if (pqueue->destroy_value != NULL)
		pqueue->destroy_value(pqueue_max(pqueue));

	// Αντικαθιστούμε τον πρώτο κόμβο με τον τελευταίο και αφαιρούμε τον τελευταίο
	node_swap(pqueue, 1, last_node);
	vector_remove_last(pqueue->vector);
	
 	// Ολοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού εκτός από τη νέα ρίζα
 	// που μπορεί να είναι μικρότερη από κάποιο παιδί της. Αρα μπορούμε να
 	// επαναφέρουμε την ιδιότητα του σωρού καλώντας τη bubble_down για τη ρίζα.
	bubble_down(pqueue, 1 );
}

DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value) {
	DestroyFunc old = pqueue->destroy_value;
	pqueue->destroy_value = destroy_value;
	return old;
}

void pqueue_destroy(PriorityQueue pqueue) {
	// Αντί να κάνουμε εμείς destroy τα στοιχεία, είναι απλούστερο να
	// προσθέσουμε τη destroy_value στο vector ώστε να κληθεί κατά το vector_destroy.
	vector_set_destroy_value(pqueue->vector, pqueue->destroy_value);
	vector_destroy(pqueue->vector);

	free(pqueue);
}



//// Νέες συναρτήσεις για την εργασία 2 //////////////////////////////////////////

//Επιστρέφω το value του κόμβου node
Pointer pqueue_node_value(PriorityQueue set, PriorityQueueNode node) {
	return node->value;
}

void pqueue_remove_node(PriorityQueue pqueue, PriorityQueueNode node) {
	int pos_node = (node->pos+1);	//βρίσκω την θέση του κόμβου στο vector που είναι μια "θέση" πιο μεγάλη από το pos αφού δεν ξεκινάμε από το 1 αλλά από το 0 την μέτρηση του pos
	int last = pqueue_size(pqueue); 
	
		node_swap(pqueue, pos_node, last);	//ανταλλάσουμε τα περιεχόμενα του κόμβου που θέλουμε να αλλάξουμε με αυτά του τελευταίου

	
	vector_remove_last(pqueue->vector);	//αφαιρούμε τον τελευταίο
		if (node->owner->destroy_value != NULL){
		node->owner->destroy_value(node->value);
	}
	free(node); //ελευθερώνουμε τον κόμβο
	bubble_down(pqueue,pos_node);	// από την θέση που έχει πάει ο παλιός τελευταίος κόμβος κάνουμε bubble_down

	
	
}

void pqueue_update_order(PriorityQueue pqueue, PriorityQueueNode node) {
		int pos_node = (node->pos+1);
		int pos_parent = 2*pos_node;
		//αν το καινούριο value είναι μεγαλύτερο από το value του πατέρα του
		if (pqueue->compare(node_value(pqueue, pos_parent), node_value(pqueue, pos_node)) < 0) {
		node_swap(pqueue, pos_parent, pos_node); //τότε ανταλλάσουμε πατέρα και παιδί
		bubble_up(pqueue, pos_parent); //και κάνουμε bubble up
	}
	else{ //αλλιώς
		bubble_down(pqueue, pos_node);
	}
}
