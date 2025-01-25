cmake_minimum_required(VERSION 3.10)

include_directories("${PROJECT_SOURCE_DIR}/include")

set(SOURCES
    food.c
    gameplay.c
    highscores.c
    menu.c
    snake.c
    utility.c
)

add_library(snakeLibrary STATIC ${SOURCES})
