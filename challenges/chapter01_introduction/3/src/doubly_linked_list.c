#include "doubly_linked_list.h"
#include <stdlib.h>
#include <string.h>

void insert_node_after(List *list, Node *node, Node *new_node) {
    new_node->previous = node;
    if (!node->next) {
        new_node->next = 0; // null
        list->last_node = new_node;
    } else {
        new_node->next = node->next;
        node->next->previous = new_node;
    }
    node->next = new_node;
}

void insert_node_before(List *list, Node *node, Node *new_node) {
    new_node->next = node;
    if (!node->previous) {
        new_node->previous = 0; // null
        list->first_node = new_node;
    } else {
        new_node->previous = node->previous;
        node->previous->next = new_node;
    }
    node->previous = new_node;
}

void remove_node(List *list, Node *node) {
    if (!node->previous) {
        list->first_node = node->next;
    } else {
        node->previous->next = node->next;
    }
    if (!node->next) {
        list->last_node = node->previous;
    } else {
        node->next->previous = node->previous;
    }
    free(node->data);
    free(node);
}

Node *find_node_with_data(List *list, const char *data) {
    Node *node = list->first_node;
    while (node) {
        if (strcmp(data, node->data) == 0) {
            return node;
        }
        node = node->next;
    }
    return 0;
}

Node *init_node(const char *data, size_t buffer_size) {
    char *buffer = malloc(buffer_size);
    strncpy(buffer, data, buffer_size);
    Node *node = malloc(sizeof(Node));
    node->data = buffer;
    node->next = 0;     // null
    node->previous = 0; // null
    return node;
}

List init_list(Node *node) {
    List list = {
        .first_node = node,
        .last_node = node,
    };
    return list;
}

void delete_list(List *list) {
    Node *node = list->first_node;
    while (node) {
        Node *temp = node;
        node = temp->next;
        free(temp->data);
        free(temp);
    }
    list->first_node = 0;
    list->last_node = 0;
}
