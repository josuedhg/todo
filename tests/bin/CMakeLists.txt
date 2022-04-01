file(GLOB c_sources ${CMAKE_SOURCE_DIR}/bin/*.c)

file(GLOB c_tests test_*.c)

add_link_options(-Wl,--wrap=access)
add_link_options(-Wl,--wrap=fopen)
add_link_options(-Wl,--wrap=fclose)
add_link_options(-Wl,--wrap=getline)
add_link_options(-Wl,--wrap=fwrite)

foreach(file ${c_tests})
	get_filename_component(test_name ${file} NAME_WE)
	add_executable(${test_name} ${file} ${c_sources} ${CMAKE_SOURCE_DIR}/tests/instrument.c)

	target_link_libraries(${test_name} cmocka)

	add_test(NAME ${test_name}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMAND ${test_name})
	add_dependencies(check ${test_name})
endforeach()
