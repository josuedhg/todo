cmake_minimum_required(VERSION 3.18)

project(todo)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(TODOTXT_FILE_NAME ".todo.txt" CACHE STRING "TODO file name")

include(CTest)
include(GNUInstallDirs)
include(CMakeOptions.txt)
include_directories(lib)

if(${COVERAGE})
	include(CodeCoverage)
endif(${COVERAGE})

add_subdirectory(lib)
add_subdirectory(bin)
if(${TESTS})
add_subdirectory(tests)
	if(${COVERAGE})
		coverage_target(. */tests/* check)
	endif(${COVERAGE})
endif(${TESTS})

message(STATUS "TESTS " ${TESTS})
message(STATUS "COVERAGE " ${COVERAGE})
message(STATUS "todo.txt file name " ${TODOTXT_FILE_NAME})
