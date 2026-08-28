#pragma once

typedef unsigned char VPS_TYPE_8U;
typedef signed char VPS_TYPE_8S;
typedef unsigned short VPS_TYPE_16U;
typedef signed short VPS_TYPE_16S;
typedef unsigned int VPS_TYPE_32U;
typedef signed int VPS_TYPE_32S;
typedef unsigned long long VPS_TYPE_64U;
typedef signed long long VPS_TYPE_64S;
typedef void * VPS_TYPE_ADDRESS;
typedef unsigned char * VPS_TYPE_DATA;

typedef unsigned long long VPS_TYPE_SIZE;
typedef long long VPS_TYPE_SPAN;

/**
 * @brief The status type returned by VulpesCore functions.
 * @details VPS_OK (0) means success. Any non-zero value is a failure and
 *          carries the source line that raised it, so a failing call can be
 *          traced to its origin without a debugger.
 *
 *          Note the polarity: this is the inverse of a boolean. `if (result)`
 *          means *failure*.
 */
typedef unsigned long VPS_TYPE_RESULT;

/**
 * @brief Success.
 */
#define VPS_OK ((VPS_TYPE_RESULT)0)

/**
 * @brief Failure, tagged with the line that raised it.
 * @details Always raise failures through this macro rather than returning a
 *          literal, so the encoding stays in one place.
 */
#define VPS_FAIL ((VPS_TYPE_RESULT)__LINE__)

/*
 * Propagating failures
 * --------------------
 * When forwarding a failure from a nested call, return the value you
 * received rather than raising a fresh VPS_FAIL. The original line is the
 * useful one; overwriting it discards the root cause.
 *
 *     result = VPS_Data_Resize(item, new_size);
 *     if (result)
 *     {
 *         return result;
 *     }
 *
 * What does NOT use this type
 * ---------------------------
 * Predicates answer a question rather than report a status, and keep the
 * plain char/boolean form (1 = yes):
 *
 *     VPS_Dictionary_Find, VPS_ScopedDictionary_Find,
 *     VPS_ConcurrentDictionary_Find, VPS_SwissDictionary_Find,
 *     VPS_List_Find, VPS_Set_Contains
 *
 * A lookup miss is a normal outcome, not a fault. The `match` callback of
 * VPS_List_Find and the `condition` callback of VPS_List_Move are likewise
 * predicates, as is the `exit_on_error` argument of VPS_List_Apply, which is
 * a boolean input rather than a result.
 *
 * Functions that yield a value (the VPS_Endian readers, for instance) return
 * that value, not a status.
 */
