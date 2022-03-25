
set(todo_src main.c)
add_executable(todo ${todo_src})
target_include_directories(todo PRIVATE ${CMAKE_SOURCE_DIR}/bin)
target_link_libraries(todo LINK_PUBLIC libtodo)
