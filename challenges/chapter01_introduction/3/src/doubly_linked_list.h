#pragma once

#include <stddef.h>

/**
 * Doubly linked node that references both its next and previous nodes and holds
 * a pointer to a string.
 */
typedef struct Node {
    struct Node *next;
    struct Node *previous;
    char *data;
} Node;

/**
 * Doubly linked list that holds two values: pointers to its first and last
 * nodes.
 */
typedef struct List {
    Node *first_node;
    Node *last_node;
} List;

/**
 * Inserts a node after a given node.
 * @param list list to insert the @p new_node.
 * @param node where the @p new_node will be inserted.
 * @param new_node node to insert.
 */
void insert_node_after(List *list, Node *node, Node *new_node);

/**
 * Inserts a node before a given node.
 * @param list list to insert the @p new_node.
 * @param node where the @p new_node will be inserted.
 * @param new_node node to insert.
 */
void insert_node_before(List *list, Node *node, Node *new_node);

/**
 * Removes and frees a @p node from the @p list.
 * @param list list to remove the node.
 * @param node node to remove.
 */
void remove_node(List *list, Node *node);

/**
 * Finds the first node that contains a string equal to @p data.
 * @param list list to search.
 * @param data data for the string to search.
 * @return a pointer to the node, if it was found, or NULL.
 */
Node *find_node_with_data(List *list, const char *data);

/**
 * Allocates and constructs a Node with @p data that doesn't point to other
 * nodes. @p data is allocated on the heap with malloc due to the instructions
 * of the challenge.
 * @param data string for the node to hold.
 * @param buffer_size size of the allocated buffer with malloc.
 * @return a pointer to the node.
 */
Node *init_node(const char *data, size_t buffer_size);

/**
 * Constructs an list with one node.
 * @param node node to insert.
 * @return the list that was created.
 */
List init_list(Node *node);

/**
 * Frees all the nodes and the data they contain.
 * The pointer @p list itself is not freed.
 * @param list list to delete.
 */
void delete_list(List *list);
