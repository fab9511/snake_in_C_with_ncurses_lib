#include "snakeLib.h"

int main() {
    GameState state;
    state.scoreCount = loadHighScores(state.scores);
    
    start: 
    displayMenu(&state);
    initializeGame(&state);
    
    while(!state.gameOver) {
        handleInput(&state);
        updateGame(&state);
        drawBoard(&state);
        drawTimer(&state);
        usleep(state.speed);
    }
    
    mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "GAME OVER!");
    mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 5, "Score: %d", state.score);
    refresh();
    nodelay(stdscr, FALSE); // czekaj na wcisniecie klawisza 
    getch();
    
    //zapis wyniku
    getGameScore(&state);

    displayHighscores(&state, state.scoreCount);
    goto start;

    return 0;
}
