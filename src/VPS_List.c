#include <stdlib.h>
#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_List.h>

char VPS_List_Allocate
(
	struct VPS_List **item
)
{
	if (!item)
	{
		return 0;
	}

	struct VPS_List *subject = calloc(1, sizeof(struct VPS_List));
	if (!subject)
	{
		return 0;
	}

	*item = subject;

	return 1;
}

char VPS_List_Construct
(
	struct VPS_List *item,
	void *data,
	char (*data_release)(void *data),
	char (*node_data_release)(void *data)
)
{
	if (!item)
	{
		return 0;
	}

	VPS_List_Clear(item);
	
	item->data = data;
	item->data_release = data_release;
	item->node_data_release = node_data_release;
	item->count = 0;
	
	return 1;
}

char VPS_List_Deconstruct
(
	struct VPS_List *item
)
{
	if (!item)
	{
		return 0; 
	}

	VPS_List_Clear(item);
	
	return 1;
}

char VPS_List_Release
(
	struct VPS_List *item
)
{
	if (item)
	{
		free(item);
	}

	return 1;
}

char VPS_List_Clear
(
	struct VPS_List *item
)
{
	struct VPS_List_Node *node;
	if (!item)
	{
		return 0;
	}

	while (item->head)
	{
		node = item->head;
		if (item->node_data_release)
		{
			item->node_data_release(node->data);
		}

		VPS_List_RemoveHead(item, 0);
		VPS_List_Node_Deconstruct(node);
		VPS_List_Node_Release(node);
	}

	item->count = 0;
	return 1;
}

char VPS_List_AddHead
(
	struct VPS_List *item,
	struct VPS_List_Node *node
)
{
	if (!item || !node)
	{
		return 0;
	}

	VPS_List_Node_Remove(node);

	node->parent = item;
	node->back = 0;
	node->next = item->head;

	if (item->head)
	{
		item->head->back = node;
	}
	else
	{
		item->tail = node;
	}

	item->head = node;
	item->count++;

	return 1;
}

char VPS_List_AddTail
(
	struct VPS_List *item,
	struct VPS_List_Node *node
)
{
	if (!item || !node)
	{
		return 0;
	}

	VPS_List_Node_Remove(node);

	node->parent = item;
	node->next = 0;
	node->back = item->tail;

	if (item->tail)
	{
		item->tail->next = node;
	}
	else
	{
		item->head = node;
	}

	item->tail = node;
	item->count++;

	return 1;
}

char VPS_List_RemoveHead
(
	struct VPS_List *item,
	struct VPS_List_Node **node
)
{
	struct VPS_List_Node *temp;

	if (!item || !item->head)
	{
		return 0;
	}

	temp = item->head;
	if (node)
	{
		*node = temp;
	}

	item->head = temp->next;
	if (item->head)
	{
		item->head->back = 0;
	}
	else
	{
		item->tail = 0;
	}

	temp->parent = 0;
	temp->next = 0;
	temp->back = 0;
	
	item->count--;
	return 1;
}

char VPS_List_RemoveTail
(
	struct VPS_List *item,
	struct VPS_List_Node **node
)
{
	struct VPS_List_Node *temp;

	if (!item || !item->tail)
	{
		return 0;
	}

	temp = item->tail;
	if (node)
	{
		*node = temp;
	}

	item->tail = temp->back;
	if (item->tail)
	{
		item->tail->next = 0;
	}
	else
	{
		item->head = 0;
	}

	temp->parent = 0;
	temp->next = 0;
	temp->back = 0;

	item->count--;
	return 1;
}

char VPS_List_Apply
(
	struct VPS_List *item,
    struct VPS_List_Node *start,
	char (*fn)(struct VPS_List_Node *node, void *context),
	void *context,
	char exit_on_error,
	struct VPS_List_Node **error_node
)
{
	struct VPS_List_Node *temp;
	char result;

	if (!item || !fn)
	{
		if (exit_on_error && error_node)
		{
			*error_node = 0;
		}

		return 0;
	}

	temp = item->head;

	if (start)
	{
		temp = start;
		if (start->parent != item)
		{
			if (exit_on_error && error_node)
			{
				*error_node = start;
			}
			return 0;
		}
	}

	while (temp)
	{
		result = fn(temp, context);
		if (!result)
		{
			if (exit_on_error && error_node)
			{
				*error_node = temp;
			}

			return 0;
		}

		temp = temp->next;
	}

	return 1;
}

char VPS_List_Find
(
	struct VPS_List *item,
	struct VPS_List_Node *start,
	char (*match)(struct VPS_List_Node *node, void *context),
	void *context,
	struct VPS_List_Node **result
)
{
	struct VPS_List_Node *temp;
	char found;

	if (!item || !match || !result)
	{
		return 0;
	}

	temp = item->head;

	if (start)
	{
		temp = start;
		if (start->parent != item)
		{
			*result = 0;

			return 0;
		}
	}

	while (temp)
	{
		found = match(temp, context);
		if (found)
		{
			*result = temp;
			return 1;
		}

		temp = temp->next;
	}

	return 0;
}

char VPS_List_Move
(
	struct VPS_List *item,
	struct VPS_List_Node *start,
	char (*condition)(struct VPS_List_Node *node, void *context),
	struct VPS_List *destination,
	void *context
)
{
	struct VPS_List_Node *temp;
	struct VPS_List_Node *next_node;
	char found;

	if (!item || !destination || !condition)
	{
		return 0;
	}

	if (item == destination)
	{
		return 1;
	}

	temp = item->head;
	if (start)
	{
		if (start->parent != item)
		{
			return 0;
		}
		temp = start;
	}

	while (temp)
	{
		next_node = temp->next;

		found = condition(temp, context);
		if (found)
		{
			VPS_List_Node_Remove(temp);
			VPS_List_AddTail(destination, temp);
		}

		temp = next_node;
	}

	return 1;
}

char VPS_List_Node_Allocate
(
	struct VPS_List_Node **item
)
{
	struct VPS_List_Node *node;

	if (!item)
	{
		return 0;
	}

	node = calloc(1, sizeof(struct VPS_List_Node));
	if (!node)
	{
		*item = 0;

		return 0;
	}

	*item = node;

	return 1;
}

char VPS_List_Node_Construct
(
	struct VPS_List_Node *item,
	void *data
)
{
	if (!item)
	{
		return 0;
	}

	item->data = data;

	return 1;
}

char VPS_List_Node_Deconstruct
(
	struct VPS_List_Node *item
)
{
	if (!item)
	{
		return 0;
	}

	return 1;
}

char VPS_List_Node_Release
(
	struct VPS_List_Node *item
)
{
	if (!item)
	{
		return 0;
	}

	free(item);

	return 1;
}

char VPS_List_Node_InsertBefore
(
	struct VPS_List_Node *item,
	struct VPS_List_Node *node
)
{
	if (!item || !item->parent || !node || node->parent || node->next || node->back)
	{
		return 0;
	}

	node->parent = item->parent;
	node->next = item;
	node->back = item->back;

	if (item->back)
	{
		item->back->next = node;
	}
	else
	{
		item->parent->head = node;
	}

	item->back = node;
	item->parent->count++;

	return 1;
}

char VPS_List_Node_InsertAfter
(
	struct VPS_List_Node *item,
	struct VPS_List_Node *node
)
{
	if (!item || !item->parent || !node || node->parent || node->next || node->back)
	{
		return 0;
	}

	node->parent = item->parent;
	node->back = item;
	node->next = item->next;

	if (item->next)
	{
		item->next->back = node;
	}
	else
	{
		item->parent->tail = node;
	}

	item->next = node;
	item->parent->count++;

	return 1;
}

char VPS_List_Node_Remove
(
	struct VPS_List_Node *item
)
{
	if (!item || !item->parent)
	{
		return 0;
	}

	item->parent->count--;

	if (item->back)
	{
		item->back->next = item->next;
	}
	else
	{
		item->parent->head = item->next;
	}

	if (item->next)
	{
		item->next->back = item->back;
	}
	else
	{
		item->parent->tail = item->back;
	}

	item->parent = 0;
	item->next = 0;
	item->back = 0;

	return 1;
}
