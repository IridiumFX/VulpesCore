// --- Test Runner Macros ---

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