#include <stdlib.h>
#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_List.h>

VPS_TYPE_RESULT VPS_List_Allocate
(
	struct VPS_List **item
)
{
	if (!item)
	{
		return VPS_FAIL;
	}

	struct VPS_List *subject = calloc(1, sizeof(struct VPS_List));
	if (!subject)
	{
		return VPS_FAIL;
	}

	*item = subject;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Construct
(
	struct VPS_List *item,
	void *data,
	VPS_TYPE_RESULT (*data_release)(void *data),
	VPS_TYPE_RESULT (*node_data_release)(void *data)
)
{
	if (!item)
	{
		return VPS_FAIL;
	}

	VPS_List_Clear(item);
	
	item->data = data;
	item->data_release = data_release;
	item->node_data_release = node_data_release;
	item->count = 0;
	
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Deconstruct
(
	struct VPS_List *item
)
{
	if (!item)
	{
		return VPS_FAIL;
	}

	VPS_List_Clear(item);

	// Release the user context exactly once; stays idempotent by nulling both.
	if (item->data_release)
	{
		item->data_release(item->data);
	}
	item->data = 0;
	item->data_release = 0;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Release
(
	struct VPS_List *item
)
{
	if (item)
	{
		VPS_List_Deconstruct(item);

		free(item);
	}

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Clear
(
	struct VPS_List *item
)
{
	struct VPS_List_Node *node;
	if (!item)
	{
		return VPS_FAIL;
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
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_AddHead
(
	struct VPS_List *item,
	struct VPS_List_Node *node
)
{
	if (!item || !node)
	{
		return VPS_FAIL;
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

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_AddTail
(
	struct VPS_List *item,
	struct VPS_List_Node *node
)
{
	if (!item || !node)
	{
		return VPS_FAIL;
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

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_RemoveHead
(
	struct VPS_List *item,
	struct VPS_List_Node **node
)
{
	struct VPS_List_Node *temp;

	if (!item || !item->head)
	{
		return VPS_FAIL;
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
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_RemoveTail
(
	struct VPS_List *item,
	struct VPS_List_Node **node
)
{
	struct VPS_List_Node *temp;

	if (!item || !item->tail)
	{
		return VPS_FAIL;
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
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Apply
(
	struct VPS_List *item,
    struct VPS_List_Node *start,
	VPS_TYPE_RESULT (*fn)(struct VPS_List_Node *node, void *context),
	void *context,
	char exit_on_error,
	struct VPS_List_Node **error_node
)
{
	struct VPS_List_Node *temp;
	char all_succeeded;
	VPS_TYPE_RESULT result;

	if (!item || !fn)
	{
		if (error_node)
		{
			*error_node = 0;
		}

		return VPS_FAIL;
	}

	temp = item->head;

	if (start)
	{
		temp = start;
		if (start->parent != item)
		{
			if (error_node)
			{
				*error_node = start;
			}
			return VPS_FAIL;
		}
	}

	all_succeeded = 1;

	while (temp)
	{
		result = fn(temp, context);
		if (result)
		{
			if (all_succeeded && error_node)
			{
				// Report the first failing node.
				*error_node = temp;
			}
			all_succeeded = 0;

			if (exit_on_error)
			{
				return VPS_FAIL;
			}
		}

		temp = temp->next;
	}

	// all_succeeded stays a flag: it records whether every node worked, and
	// the caller learns which one failed through error_node.
	return all_succeeded ? VPS_OK : VPS_FAIL;
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

VPS_TYPE_RESULT VPS_List_Move
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
		return VPS_FAIL;
	}

	if (item == destination)
	{
		return VPS_OK;
	}

	temp = item->head;
	if (start)
	{
		if (start->parent != item)
		{
			return VPS_FAIL;
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

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Node_Allocate
(
	struct VPS_List_Node **item
)
{
	struct VPS_List_Node *node;

	if (!item)
	{
		return VPS_FAIL;
	}

	node = calloc(1, sizeof(struct VPS_List_Node));
	if (!node)
	{
		*item = 0;

		return VPS_FAIL;
	}

	*item = node;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Node_Construct
(
	struct VPS_List_Node *item,
	void *data
)
{
	if (!item)
	{
		return VPS_FAIL;
	}

	item->data = data;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Node_Deconstruct
(
	struct VPS_List_Node *item
)
{
	if (!item)
	{
		return VPS_FAIL;
	}

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Node_Release
(
	struct VPS_List_Node *item
)
{
	if (!item)
	{
		return VPS_FAIL;
	}

	free(item);

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Node_InsertBefore
(
	struct VPS_List_Node *item,
	struct VPS_List_Node *node
)
{
	if (!item || !item->parent || !node || node->parent || node->next || node->back)
	{
		return VPS_FAIL;
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

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Node_InsertAfter
(
	struct VPS_List_Node *item,
	struct VPS_List_Node *node
)
{
	if (!item || !item->parent || !node || node->parent || node->next || node->back)
	{
		return VPS_FAIL;
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

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_List_Node_Remove
(
	struct VPS_List_Node *item
)
{
	if (!item || !item->parent)
	{
		return VPS_FAIL;
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

	return VPS_OK;
}
