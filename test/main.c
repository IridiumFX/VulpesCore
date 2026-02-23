#include <stdio.h>

#include "Test.h"

int main(int argc, char *argv[])
{
	printf("=================================\n");
	printf("  Starting VulpesCore Test Suite \n");
	printf("=================================\n");

	// Run all the test suites.
	RUN_TEST_SUITE(test_suite_VPS_Data);
	RUN_TEST_SUITE(test_suite_VPS_DataReader);
	RUN_TEST_SUITE(test_suite_VPS_StreamReader);
	RUN_TEST_SUITE(test_suite_VPS_List);
	RUN_TEST_SUITE(test_suite_VPS_Dictionary);
	RUN_TEST_SUITE(test_suite_VPS_ScopedDictionary);
	RUN_TEST_SUITE(test_suite_VPS_Set);
	RUN_TEST_SUITE(test_suite_VPS_StreamWriter);
	RUN_TEST_SUITE(test_suite_VPS_Endian);

	// Note: The success/failure counts are managed within each suite's file.
	// A more advanced runner would aggregate these.

	printf("\n=================================\n");
	printf("      Test Suite Complete        \n");
	printf("=================================\n");

	return 0;
}