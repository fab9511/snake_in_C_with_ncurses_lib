#include "snakeLib.h"

void initializeGame(GameState *state) {
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); //nie czekaj na input    
    start_color();
   
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // kolor snake'a
    init_pair(2, COLOR_RED, COLOR_BLACK);    // kolor owocu, gameover
    init_pair(3, COLOR_BLUE, COLOR_BLACK);   // kolor scian, przeszkod
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);   // kolor wyniku, specjalnego owocu
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);   // kolor trujacego owocu
    
    // ustawienie poziomu trudnosci
    applyDifficultySettings(state);
    
    // score
    state->score = 0;
    state->gameOver = false;    
   
    //czas poczatkowy 
    state->startTime = time(NULL); 
    
    //plansza
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            state->board[i][j] = ' ';
    //pionowe
    for(int i = 0; i < HEIGHT; i++) {
        state->board[0][i] = '|';
        state->board[WIDTH-1][i] = '|';
    } 
    //poziome
    for(int i = 0; i < WIDTH; i++) {
        state->board[i][0] = '_';
        state->board[i][HEIGHT-1] = '^';
    } 
    
    // snake
    state->snake.length = 1;
    state->snake.direction = 'D';
    state->snake.segments[0].x = WIDTH / 2;
    state->snake.segments[0].y = HEIGHT / 2;

    // jedzenie 
    srand(time(NULL));
    spawnFood(state, NORMAL);
    spawnFood(state, SPECIAL);
    spawnFood(state, POISON);

    //przeszkody 
    spawnObstacles(state);
 
    clear();    
    // highscores
    attron(COLOR_PAIR(4));
    mvprintw(8 , WIDTH + 5 , "Highscores:");
    for (int i = 0; i < state->scoreCount; i++) {
        mvprintw(10 + 2*i, WIDTH + 5, "%d. %s - %d", i + 1, state->scores[i].name, state->scores[i].score);
    }
    attroff(COLOR_PAIR(4));
}

void applyDifficultySettings(GameState *state) {
    switch (state->difficulty) {
        case EASY:
            state->speed = 100000;
            state->obstacles = 7;
            break;
        case MEDIUM:
            state->speed = 70000;
            state->obstacles = 10;
            break;
        case HARD:
            state->speed = 50000;
            state->obstacles = 12;
            break;
        default:
            state->speed = 100000;
            state->obstacles = 7;
            break;
    }
}

void spawnObstacles(GameState *state) {
    int numObstacles = state->obstacles;
    while (numObstacles > 0) {
        int x = rand() % (WIDTH - 2) + 1; // Losowanie pozycji 
        int y = rand() % (HEIGHT - 2) + 1;

        // czy nie na snake'u
        bool collision = false;
        for (int i = 0; i < state->snake.length; i++) {
            if (state->snake.segments[i].x == x && state->snake.segments[i].y == y) {
                collision = true;
                break;
            }
        }
        
        if (state->board[y][x] == ' ' && !collision) {
            state->board[y][x] = '#'; //dodaj przeszkode 
            numObstacles--;
        }
    }
}

bool checkCollision(GameState *state) {
    int x = state->snake.segments[0].x;
    int y = state->snake.segments[0].y;
        
    // kolizja ściana
    if (x <= 0 || x >= WIDTH-1 || y <= 0 || y >= HEIGHT-1)
        return true;
    
    // kolizja przeszkoda
    if (state->board[x][y] == '#')
       return true;
    
    // kolizja cialo weza
    if (state->snake.length <= 1)
        return false;
    for (int i = 1; i < state->snake.length; i++) {
        if (x == state->snake.segments[i].x && y == state->snake.segments[i].y)
            return true;
    }
 
   return false;
}

void handleInput(GameState *state) {
    int ch = getch(); // input

    switch (ch) {
        case 'w': case KEY_UP:
            if (state->snake.direction != 'S') state->snake.direction = 'W';
            break;
        case 's': case KEY_DOWN:
            if (state->snake.direction != 'W') state->snake.direction = 'S';
            break;
        case 'a': case KEY_LEFT:
            if (state->snake.direction != 'D') state->snake.direction = 'A';
            break;
        case 'd': case KEY_RIGHT:
            if (state->snake.direction != 'A') state->snake.direction = 'D';
            break;
        case 'x':
            state->gameOver = true;
            break;
        case 'p':
            nodelay(stdscr, FALSE); 
            mvprintw(HEIGHT / 2, 4, "Paused. Press any key to continue.");
            refresh();
            getch();
            nodelay(stdscr, TRUE);
            break;
        }
}

void updateGame(GameState *state) {
    // ruch ciala weza
    for (int i = state->snake.length - 1; i > 0; i--)
        state->snake.segments[i] = state->snake.segments[i - 1];

    // Ruch glowy weza
    switch (state->snake.direction) {
        case 'W': state->snake.segments[0].y--; break;
        case 'S': state->snake.segments[0].y++; break;
        case 'A': state->snake.segments[0].x--; break;
        case 'D': state->snake.segments[0].x++; break;
    }

    // sprawdzanie kolizji 
    if(checkCollision(state))
        state->gameOver = true;

    // zjadl owoc ??
    checkEatingFruit(state);
    //zmien pozycje specjalnych i trujacych owocow
    changePositionFood(state); 
}

void drawBoard(GameState *state) {
    //rysowanie planszy
    attron(COLOR_PAIR(3));
    for (int x = 0; x < HEIGHT; x++)
        for (int y = 0; y < WIDTH; y++)
            mvprintw(y, x, "%c", state->board[x][y]);
    attroff(COLOR_PAIR(3));
    
    // snake
    attron(COLOR_PAIR(1));
    mvprintw(state->snake.segments[0].y, state->snake.segments[0].x, "O");
    for (int i = 1; i < state->snake.length; i++)
        mvprintw(state->snake.segments[i].y, state->snake.segments[i].x, "o");
    attroff(COLOR_PAIR(1));
    
    // jedzenie
    drawFood(state);
    
    refresh();
    
    //wynik
    mvprintw(3, WIDTH + 5, "Score: %d", state->score);
}
