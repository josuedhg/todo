file(GLOB c_sources ${CMAKE_SOURCE_DIR}/bin/*.c)

file(GLOB c_tests test_*.c)

include_directories(${CMAKE_BINARY_DIR}/tests/bin)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/bin)
configure_file(${CMAKE_SOURCE_DIR}/bin/configure.h.in configure.h @ONLY)

# instrumenting print fuctions
add_link_options(-Wl,--wrap=fprintf)
add_link_options(-Wl,--wrap=printf)

# instrumenting libtodo
add_link_options(-Wl,--wrap=create_todotxt)
add_link_options(-Wl,--wrap=todo_load_tasks)
add_link_options(-Wl,--wrap=todo_clean_tasks)
add_link_options(-Wl,--wrap=destroy_todotxt)
add_link_options(-Wl,--wrap=create_new_task)
add_link_options(-Wl,--wrap=todo_add_task)
add_link_options(-Wl,--wrap=todo_remove_task)
add_link_options(-Wl,--wrap=todo_save_tasks)

foreach(file ${c_tests})
	get_filename_component(test_name ${file} NAME_WE)
	add_executable(${test_name} ${file} ${c_sources} instrument.c)

	target_link_libraries(${test_name} cmocka todo)

	add_test(NAME ${test_name}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMAND ${test_name})
	add_dependencies(check ${test_name})
endforeach()
