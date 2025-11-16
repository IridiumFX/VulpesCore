#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Compare_Utils.h>
#include <vulpes/VPS_Data.h>

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

char VPS_Compare_Utils_Data
(
	void *key_1
	, void *key_2
	, VPS_TYPE_16S *ordering
)
{
	struct VPS_Data *data1;
	struct VPS_Data *data2;
	VPS_TYPE_SIZE len1;
	VPS_TYPE_SIZE len2;
	VPS_TYPE_SIZE min_len;
	int cmp_result;

	if (!key_1 || !key_2 || !ordering)
	{
		return 0;
	}

	data1 = (struct VPS_Data *)key_1;
	data2 = (struct VPS_Data *)key_2;

	len1 = data1->limit - data1->position;
	len2 = data2->limit - data2->position;
	min_len = (len1 < len2) ? len1 : len2;

	if (min_len > 0)
	{
		cmp_result = memcmp
		(
			data1->bytes + data1->position
			, data2->bytes + data2->position
			, min_len
		);
		if (cmp_result != 0)
		{
			*ordering = (cmp_result < 0) ? -1 : 1;

			return 1;
		}
	}

	*ordering = (len1 < len2) ? -1 : (len1 > len2) ? 1 : 0;

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
