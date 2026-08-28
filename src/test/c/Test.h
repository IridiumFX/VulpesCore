// --- Two conventions live in this suite ---
//
// VulpesCore functions report VPS_TYPE_RESULT: VPS_OK (0) is success and any
// non-zero value is the source line that raised the failure. Assert on those
// with TEST_ASSERT_OK / TEST_ASSERT_FAIL, which print that line. The harness
// keeps the boolean form, because RUN_TEST reads a test's return value as
// 1 = passed; assert on plain values and on predicates (Find, Contains) with
// TEST_ASSERT.

// --- Test Runner Macros ---

// Asserts that a call succeeded (returned VPS_OK). On failure the non-zero
// result is the source line that raised it.
#define TEST_ASSERT_OK(expr) \
{ \
VPS_TYPE_RESULT _vps_result = (expr); \
if (_vps_result) { \
printf("    [FAIL] %s:%d: %s returned %lu\n", __FILE__, __LINE__, #expr, (unsigned long)_vps_result); \
return 0; \
} \
}

// Asserts that a call failed (returned a non-zero result).
#define TEST_ASSERT_FAIL(expr) \
if (!(expr)) { \
printf("    [FAIL] %s:%d: expected failure from %s\n", __FILE__, __LINE__, #expr); \
return 0; \
}


#define TEST_ASSERT(condition) \
if (!(condition)) { \
printf("    [FAIL] Assertion failed at %s:%d: %s\n", __FILE__, __LINE__, #condition); \
return 0; \
}

#define RUN_TEST(test_func) \
printf("  Running test: %s\n", #test_func); \
if (test_func()) { \
printf("    [PASS]\n"); \
success_count++; \
} else { \
failure_count++; \
}

#define RUN_TEST_SUITE(suite_func) \
printf("\n--- Running test suite: %s ---\n", #suite_func); \
suite_func();

// --- Test Suite Function Declarations ---
void test_suite_VPS_Data(void);
void test_suite_VPS_Data_extended(void);
void test_suite_VPS_DataReader(void);
void test_suite_VPS_StreamReader(void);
void test_suite_VPS_List(void);
void test_suite_VPS_Dictionary(void);
void test_suite_VPS_ScopedDictionary(void);
void test_suite_VPS_Set(void);
void test_suite_VPS_StreamWriter(void);
void test_suite_VPS_Endian(void);
void test_suite_VPS_ConcurrentDictionary();
void test_suite_VPS_SwissDictionary();
