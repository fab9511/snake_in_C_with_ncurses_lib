#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
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
    if (state.score > 0) {
        char playerName[50];
        mvprintw(HEIGHT / 2 + 3, WIDTH / 2 - 5, "Enter your name: ");
        refresh();
        echo();
        mvgetstr(HEIGHT / 2 + 4, WIDTH / 2 - 5, playerName);
        noecho();
        
        addHighScore(state.scores, &state.scoreCount, playerName, state.score);
        saveHighScores(state.scores, state.scoreCount);
    }    
    
    displayHighscores(&state, state.scoreCount);
    goto start;

    return 0;
}
