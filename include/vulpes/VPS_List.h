#pragma once
#include <vulpes/VPS_Types.h>

struct VPS_List_Node;

struct VPS_List
{
    struct VPS_List_Node *head;
    struct VPS_List_Node *tail;
    VPS_TYPE_SIZE count;

    void *data;

    char (*data_release)(void *data);
    char (*node_data_release)(void *data);
};

struct VPS_List_Node
{
    struct VPS_List_Node *back;
    struct VPS_List_Node *next;

    struct VPS_List *parent;

    void *data;
};

char VPS_List_Allocate
(
    struct VPS_List **item
);
char VPS_List_Construct
(
    struct VPS_List *item,
    void *data,
    char (*data_release)(void *data),
    char (*node_data_release)(void *data)
);
char VPS_List_Deconstruct
(
    struct VPS_List *item
);
char VPS_List_Release
(
    struct VPS_List *item
);
char VPS_List_Clear
(
    struct VPS_List *item
);
char VPS_List_AddHead
(
    struct VPS_List *item,
    struct VPS_List_Node *node
);
char VPS_List_AddTail
(
    struct VPS_List *item,
    struct VPS_List_Node *node
);
char VPS_List_RemoveHead
(
    struct VPS_List *item,
    struct VPS_List_Node **node
);
char VPS_List_RemoveTail
(
    struct VPS_List *item,
    struct VPS_List_Node **node
);
char VPS_List_Apply
(
    struct VPS_List *item,
    struct VPS_List_Node *start,
    char (*fn)(struct VPS_List_Node *node, void *context),
    void *context,
    char exit_on_error,
    struct VPS_List_Node **error_node
);
char VPS_List_Find
(
    struct VPS_List *item,
    struct VPS_List_Node *start,
    char (*match)(struct VPS_List_Node *node, void *context),
    void *context,
    struct VPS_List_Node **result
);
char VPS_List_Move
(
    struct VPS_List *item,
    struct VPS_List_Node *start,
    char (*condition)(struct VPS_List_Node *node, void *context),
    struct VPS_List *destination,
    void *context
);

char VPS_List_Node_Allocate
(
    struct VPS_List_Node **item
);
char VPS_List_Node_Construct
(
    struct VPS_List_Node *item,
    void *data
);
char VPS_List_Node_Deconstruct
(
    struct VPS_List_Node *item
);
char VPS_List_Node_Release
(
    struct VPS_List_Node *item
);
char VPS_List_Node_InsertBefore
(
    struct VPS_List_Node *item,
    struct VPS_List_Node *node
);
char VPS_List_Node_InsertAfter
(
    struct VPS_List_Node *item,
    struct VPS_List_Node *node
);
char VPS_List_Node_Remove
(
    struct VPS_List_Node *item
);
