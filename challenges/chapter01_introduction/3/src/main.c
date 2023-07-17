#include "doubly_linked_list.h"
#include <assert.h>
#include <string.h>

#define TEST_STRING "test"

static void test_init_list(void) {
    Node *node = init_node(TEST_STRING, sizeof(TEST_STRING));
    List list = init_list(node);
    assert(list.first_node == node && list.last_node == node);
    delete_list(&list);
    assert(list.first_node == 0 && list.last_node == 0);
}

static void test_insert_after(void) {
    List list = init_list(init_node(TEST_STRING, sizeof(TEST_STRING)));
    const char first_message[] = "message to assert.";
    insert_node_after(&list, list.first_node,
                      init_node(first_message, sizeof(first_message)));
    assert(strcmp(first_message, list.last_node->data) == 0);

    const char second_message[] = "second message to assert.";
    insert_node_after(&list, list.first_node,
                      init_node(second_message, sizeof(second_message)));
    assert(strcmp(second_message, list.first_node->next->data) == 0);
    assert(strcmp(first_message, list.last_node->data) == 0);

    delete_list(&list);
}

static void test_insert_before(void) {
    List list = init_list(init_node(TEST_STRING, sizeof(TEST_STRING)));
    const char first_message[] = "message to assert.";
    insert_node_before(&list, list.last_node,
                       init_node(first_message, sizeof(first_message)));
    assert(strcmp(first_message, list.first_node->data) == 0);

    const char second_message[] = "second message to assert.";
    insert_node_before(&list, list.first_node,
                       init_node(second_message, sizeof(second_message)));
    assert(strcmp(second_message, list.first_node->data) == 0);
    assert(strcmp(first_message, list.first_node->next->data) == 0);

    delete_list(&list);
}

static void test_remove_node(void) {
    Node *node = init_node(TEST_STRING, sizeof(TEST_STRING));
    List list = init_list(node);
    // test_init_list already checks if this node was added successfully
    remove_node(&list, node);
    assert(list.first_node == 0 && list.last_node == 0);
    delete_list(&list);
}

static void test_find_node(void) {
    List list = init_list(init_node(TEST_STRING, sizeof(TEST_STRING)));
    const char pattern[] = "string to search for";
    Node *node_to_search = init_node(pattern, sizeof(pattern));
    insert_node_after(&list, list.first_node, node_to_search);
    // Add a few nodes so the node to be searched isn't in the end or beginning
    for (int i = 0; i < 10; i++) {
        insert_node_after(&list, list.first_node,
                          init_node(TEST_STRING, sizeof(TEST_STRING)));
    }
    for (int i = 0; i < 10; i++) {
        insert_node_after(&list, list.last_node,
                          init_node(TEST_STRING, sizeof(TEST_STRING)));
    }
    assert(find_node_with_data(&list, pattern) == node_to_search);
    delete_list(&list);
}

int main(void) {
    test_init_list();
    test_insert_after();
    test_insert_before();
    test_remove_node();
    test_find_node();
    return 0;
}
