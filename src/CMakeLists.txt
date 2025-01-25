cmake_minimum_required(VERSION 3.10)

project(SnakeGame C)
set(CMAKE_C_STANDARD 99)
find_package(Curses REQUIRED)

configure_file(highscores.txt highscores.txt COPYONLY)
add_subdirectory(snakeLibrary)
add_executable(snake main.c)

#target_link_libraries(snake PRIVATE ${CURSES_LIBRARIES})
target_link_libraries(snake PRIVATE snakeLibrary ${CURSES_LIBRARIES})
target_include_directories(snake PRIVATE ${CURSES_INCLUDE_DIRS})
include_directories("${PROJECT_SOURCE_DIR}/include")
