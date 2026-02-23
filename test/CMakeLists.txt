# Define the test executable target.
add_executable(CoreTests)

# Add the source files for the test runner and individual test suites.
target_sources(CoreTests PRIVATE
		main.c
		test_VPS_Data.c
		test_VPS_DataReader.c
		test_VPS_StreamReader.c
		test_VPS_List.c
		test_VPS_Dictionary.c
		test_VPS_Set.c
		test_VPS_ScopedDictionary.c
		test_VPS_StreamWriter.c
		test_VPS_Endian.c
)

# The tests need to include headers from the VulpesCore library.
target_include_directories(CoreTests PRIVATE
		../include
)

# Link the test executable against the VulpesCore library.
# This ensures that the tests have access to the components they are testing.
target_link_libraries(CoreTests PRIVATE VulpesCore)