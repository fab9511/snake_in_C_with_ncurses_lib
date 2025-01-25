#include "snakeLib.h"

void checkEatingFruit(GameState *state) {
    for (int i = 0; i < 3; i++) {
        if (state->food[i].active &&
            state->snake.segments[0].x == state->food[i].x &&
            state->snake.segments[0].y == state->food[i].y) {
        
            switch (state->food[i].type) {
                case NORMAL:
                    state->score += (state->difficulty == EASY)? 100: (state->difficulty == MEDIUM)? 150:225;
                    growSnake(state);
                    spawnFood(state, NORMAL);
                    break;
                case SPECIAL:
                    state->score += (state->difficulty == EASY)? 230: 300; // Wyższy bonus punktowy
                    growSnake(state);
                    growSnake(state); // Szybszy wzrost
                    spawnFood(state, SPECIAL);
                    break;
                case POISON:
                    state->score -= 200; // Kara punktowa
                    if (state->snake.length > 1) {
                        state->snake.length--; // kara skroc snake'a
                    }
                    else if(state->difficulty == HARD)
                        state->gameOver = true; 
                    else {
                        state->gameOver = true; // Koniec gry gdy 1 segment i poison
                    }
                    spawnFood(state, POISON);
                    break;
            }
            state->food[i].active = false; // Owoc został zjedzony
        }
        state->food[NORMAL].active = true;
    }
}

void spawnFood(GameState *state, FoodType type) {
    bool validPosition;
    int x,y;
 
    do {
        validPosition = true;
        // Losowanie pozycji owoca
        x = 1 + rand() % (WIDTH - 2);
        y = 1 + rand() % (HEIGHT - 2);

        // czy nie na przeszkodzie
        if (state->board[y][x] == '#') 
            validPosition = false;

        // czy nie jest na wezu
        for (int i = 0; i < state->snake.length; i++) {
            if ((x == state->snake.segments[i].x) && (y == state->snake.segments[i].y)) {
                validPosition = false;
                break;
            }
        }
        // czy nie jest na innym owocu
        if ((NORMAL != type) && (x == state->food[NORMAL].x) && (y == state->food[NORMAL].y))
            validPosition = false;
        else if ((SPECIAL != type) && (x == state->food[SPECIAL].x) && (y == state->food[SPECIAL].y))
            validPosition = false;
        else if ((POISON != type) && (x == state->food[POISON].x) && (y == state->food[POISON].y))
            validPosition = false;
    } while (!validPosition);

    state->food[type].x = x;
    state->food[type].y = y;
    state->food[type].type = type;
    state->food[type].active = !(state->food[type].active); //przez chwilę owoc jest nieaktywny na planszy
    state->food[type].lastSpawnTime = time(NULL);
}

void changePositionFood(GameState *state) {
    time_t currentTime = time(NULL);
    const int foodRespawnInterval = 4;
    
    if(difftime(currentTime, state->food[POISON].lastSpawnTime) >= foodRespawnInterval)
        spawnFood(state, POISON);
    if(difftime(currentTime, state->food[SPECIAL].lastSpawnTime) >= foodRespawnInterval-2.4)
        spawnFood(state, SPECIAL);
    if(difftime(currentTime, state->food[NORMAL].lastSpawnTime) >= foodRespawnInterval+2)
        spawnFood(state, NORMAL);
}

void drawFood(GameState *state) {
    for (int i = 0; i < 3; i++) {
        if (state->food[i].active) {
            switch (state->food[i].type) {
                case NORMAL:
                    attron(COLOR_PAIR(2)); // Kolor czerwony
                    mvprintw(state->food[i].y, state->food[i].x, "@");
                    attroff(COLOR_PAIR(2));
                    break;
                case SPECIAL:
                    attron(COLOR_PAIR(4)); // Kolor żółty
                    mvprintw(state->food[i].y, state->food[i].x, "*");
                    attroff(COLOR_PAIR(4));
                    break;
                case POISON:
                    attron(COLOR_PAIR(5)); // Kolor magenta
                    mvprintw(state->food[i].y, state->food[i].x, "X");
                    attroff(COLOR_PAIR(5));
                    break;
            }
        }
    }
}
