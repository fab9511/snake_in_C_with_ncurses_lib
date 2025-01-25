#include "snakeLib.h"

void growSnake(GameState *state) {
    if (state->snake.length < MAX_LENGTH)
        state->snake.length++;
}
