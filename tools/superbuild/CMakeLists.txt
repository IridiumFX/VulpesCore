cmake_minimum_required(VERSION 3.22)
project(VulpesSuperbuild VERSION 0.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

#
# Builds the three repositories against each other's *working trees* rather
# than their pinned submodules.
#
# Each repository guards its dependencies with `if(NOT TARGET ...)`, so
# defining the targets here first makes the submodule copies inert. Without
# this, a change to VulpesCore is invisible to VulpesIFF and VulpesEarth --
# they would compile their own pinned copies and report a meaningless pass.
#
# Point it at the directory holding all three checkouts:
#
#   cmake -S VulpesCore/tools/superbuild -B <build> -DVULPES_ROOT=<dir>
#

if(NOT DEFINED VULPES_ROOT)
	get_filename_component(VULPES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
endif()

foreach(repo VulpesCore VulpesIFF VulpesEarth)
	if(NOT EXISTS "${VULPES_ROOT}/${repo}/CMakeLists.txt")
		message(FATAL_ERROR "no ${repo} under VULPES_ROOT=${VULPES_ROOT}")
	endif()
endforeach()

message(STATUS "Superbuild root: ${VULPES_ROOT}")

# Order matters: each is defined before anything that would pull in its
# submodule copy.
add_subdirectory("${VULPES_ROOT}/VulpesCore"  "${CMAKE_BINARY_DIR}/VulpesCore")
add_subdirectory("${VULPES_ROOT}/VulpesIFF"   "${CMAKE_BINARY_DIR}/VulpesIFF")
add_subdirectory("${VULPES_ROOT}/VulpesEarth" "${CMAKE_BINARY_DIR}/VulpesEarth")
