cmake_minimum_required(VERSION 3.22)
project(VulpesCore VERSION 0.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define the 'VulpesCore' library target.
add_library(VulpesCore STATIC)

# Add the source files for this library.
target_sources(VulpesCore PRIVATE
		src/main/c/VPS_Data.c
		src/main/c/VPS_DataReader.c
		src/main/c/VPS_DataWriter.c
		src/main/c/VPS_List.c
		src/main/c/VPS_Decoder.c
		src/main/c/VPS_Endian.c
		src/main/c/VPS_Hash_Utils.c
		src/main/c/VPS_Compare_Utils.c
		src/main/c/VPS_Decoder_Base256.c
		src/main/c/VPS_Set.c
		src/main/c/VPS_StreamReader.c
		src/main/c/VPS_StreamWriter.c
		src/main/c/VPS_ScopedDictionary.c
		src/main/c/VPS_Dictionary.c
		src/main/c/VPS_ConcurrentDictionary.c
		src/main/c/VPS_SwissDictionary.c
		src/main/c/VPS_Point.c
		src/main/c/VPS_Size.c
		src/main/c/VPS_Rect.c
)

# Define the public include directory.
# Any project that links to VulpesCore will automatically get this include path.
target_include_directories(VulpesCore PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

add_subdirectory(src/test/c)
