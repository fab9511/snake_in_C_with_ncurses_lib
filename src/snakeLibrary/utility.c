#include "snakeLib.h"

void drawTimer(GameState *state) {
    time_t currentTime = time(NULL);
    int elapsedTime = (int)(currentTime - state->startTime);

    mvprintw(2, WIDTH + 5, "Time: %02d:%02d", elapsedTime / 60, elapsedTime % 60);
}
