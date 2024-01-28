#include "List.h"

void InitList(List* list) {
    list->size = 0;
    list->head = list->tail = NULL;
    list->free = NULL;
}

void FreeList(List* list) {
    if (isListEmpty(list)) return;
    Node* ptr = list->head;
    while (ptr != NULL) {
        Node* cur = ptr;
        ptr = ptr->next;
        if (list->free) list->free(cur->value);
        free(cur);
    }
    InitList(list);
}

void ListUnlink(List* list, Node* node) {
    if (node->prev) node->prev->next = node->next;
    else list->head = node->next;
    if (node->next) node->next->prev = node->prev;
    else list->tail = node->prev;
    node->next = node->prev = NULL;
    list->size--;
}

void ListLinkHead(List* list, Node* node) {
    if (isListEmpty(list)) {
        list->head = list->tail = node;
        node->next = node->prev = NULL;
    }
    else {
        node->next = list->head;
        node->prev = NULL;
        list->head->prev = node;
        list->head = node;
    }
    list->size++;
}

void ListLinkTail(List* list, Node* node) {
    if (isListEmpty(list)) {
        list->head = list->tail = node;
        node->next = node->prev = NULL;
    }
    else {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}

Node* ListEmplaceHead(List* list, void* value) {
    Node* node = malloc(sizeof(Node));
    if (node == NULL) return node;
    node->value = value;
    ListLinkHead(list, node);
    return node;
}

Node* ListEmplaceTail(List* list, void* value) {
    Node* node = malloc(sizeof(Node));
    if (node == NULL) return node;
    node->value = value;
    ListLinkTail(list, node);
    return node;
}

void ListErase(List* list, Node* node) {
    ListUnlink(list, node);
    if (list->free) list->free(node->value);
    free(node);
}