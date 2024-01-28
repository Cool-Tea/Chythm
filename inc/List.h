#ifndef _LIST_H_
#define _LIST_H_

#include <SDL.h>

struct Node {
    void* value;
    struct Node* prev;
    struct Node* next;
};
typedef struct Node Node;

typedef void (*FreeFunc)(void*);
struct List {
    size_t size;
    Node* head;
    Node* tail;
    FreeFunc free;
};
typedef struct List List;

#define isListEmpty(list) (!(list)->size)
#define ListSetFreeMethod(list, func) ((list)->free = func)
#define ListGetFreeMethod(list) ((list)->free)
#define ListForEach(ptr, list) for ((ptr) = (list)->head; (ptr) != NULL; (ptr) = (ptr)->next)

void InitList(List* list);
void FreeList(List* list);
void ListUnlink(List* list, Node* node);
void ListLinkHead(List* list, Node* node);
void ListLinkTail(List* list, Node* node);
Node* ListEmplaceHead(List* list, void* value);
Node* ListEmplaceTail(List* list, void* value);
void ListErase(List* list, Node* node);

#endif