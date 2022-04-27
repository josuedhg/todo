
set(todo_src main.c commands.c)

configure_file(configure.h.in configure.h @ONLY)
add_executable(todocli ${todo_src} ${commands})
target_include_directories(todocli PRIVATE ${CMAKE_BINARY_DIR}/bin)
target_include_directories(todocli PRIVATE ${CMAKE_SOURCE_DIR}/bin)
target_link_libraries(todocli LINK_PUBLIC todo)
