
set(todo_src main.c)
add_executable(todocli ${todo_src})
target_include_directories(todocli PRIVATE ${CMAKE_SOURCE_DIR}/bin)
target_link_libraries(todocli LINK_PUBLIC todo)
