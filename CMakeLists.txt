cmake_minimum_required(VERSION 3.22)
project(VulpesCore VERSION 0.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define the 'VulpesCore' library target.
add_library(VulpesCore STATIC)

# Add the source files for this library.
target_sources(VulpesCore PRIVATE
		src/VPS_Data.c
		src/VPS_DataReader.c
		src/VPS_DataWriter.c
		src/VPS_List.c
		src/VPS_Decoder.c
		src/VPS_Endian.c
		src/VPS_Hash_Utils.c
		src/VPS_Compare_Utils.c
		src/VPS_Decoder_Base256.c
		src/VPS_Set.c
		src/VPS_StreamReader.c
		src/VPS_StreamWriter.c
		src/VPS_ScopedDictionary.c
		src/VPS_Dictionary.c
		src/VPS_ConcurrentDictionary.c
		src/VPS_SwissDictionary.c
)

# Define the public include directory.
# Any project that links to VulpesCore will automatically get this include path.
target_include_directories(VulpesCore PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

add_subdirectory(test)
