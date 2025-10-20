#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Compare_Utils.h>

char VPS_Compare_Utils_String
(
	void *key_1
	, void *key_2
	, VPS_TYPE_16S *ordering
)
{
	if (!key_1 || !key_2 || !ordering)
	{
		return 0;
	}

	*ordering = (VPS_TYPE_16S) strcmp
	(
		(const char *) key_1
		, (const char *) key_2
	);

	return 1;
}

char VPS_Compare_Utils_8U
(
	void *key_1
	, void *key_2
	, VPS_TYPE_16S *ordering
)
{
	if (!key_1 || !key_2 || !ordering)
	{
		return 0;
	}

	VPS_TYPE_8U val1 = *(VPS_TYPE_8U *) key_1;
	VPS_TYPE_8U val2 = *(VPS_TYPE_8U *) key_2;
	if (val1 < val2)
	{
		*ordering = -1;
	}
	else if (val1 > val2)
	{
		*ordering = 1;
	}
	else
	{
		*ordering = 0;
	}

	return 1;
}

char VPS_Compare_Utils_16U
(
	void *key_1
	, void *key_2
	, VPS_TYPE_16S *ordering
)
{
	if (!key_1 || !key_2 || !ordering)
	{
		return 0;
	}

	VPS_TYPE_16U val1 = *(VPS_TYPE_16U *) key_1;
	VPS_TYPE_16U val2 = *(VPS_TYPE_16U *) key_2;
	if (val1 < val2)
	{
		*ordering = -1;
	}
	else if (val1 > val2)
	{
		*ordering = 1;
	}
	else
	{
		*ordering = 0;
	}

	return 1;
}

char VPS_Compare_Utils_32U
(
	void *key_1
	, void *key_2
	, VPS_TYPE_16S *ordering
)
{
	if (!key_1 || !key_2 || !ordering)
	{
		return 0;
	}

	VPS_TYPE_32U val1 = *(VPS_TYPE_32U *) key_1;
	VPS_TYPE_32U val2 = *(VPS_TYPE_32U *) key_2;
	if (val1 < val2)
	{
		*ordering = -1;
	}
	else if (val1 > val2)
	{
		*ordering = 1;
	}
	else
	{
		*ordering = 0;
	}

	return 1;
}

char VPS_Compare_Utils_64U
(
	void *key_1
	, void *key_2
	, VPS_TYPE_16S *ordering
)
{
	if (!key_1 || !key_2 || !ordering)
	{
		return 0;
	}

	VPS_TYPE_64U val1 = *(VPS_TYPE_64U *) key_1;
	VPS_TYPE_64U val2 = *(VPS_TYPE_64U *) key_2;
	if (val1 < val2)
	{
		*ordering = -1;
	}
	else if (val1 > val2)
	{
		*ordering = 1;
	}
	else
	{
		*ordering = 0;
	}

	return 1;
}
