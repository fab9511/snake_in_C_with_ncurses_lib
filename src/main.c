#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define WIDTH 45 
#define HEIGHT 45
#define MAX_LENGTH 100
    
typedef struct {
    int x, y;  // Pozycja
} Segment;

typedef struct {
    Segment segments[MAX_LENGTH]; // Segmenty snake'a
    int length;                   // Aktualna dlugosc 
    char direction;               // Kierunek: 'W', 'A', 'S', 'D'
} Snake;

typedef struct {
    int x, y;  // Pozycja jedzenia
} Food;

typedef struct {
    char board[HEIGHT][WIDTH]; // Plansza gry
    Snake snake;               // gracz
    Food food;                 // Jedzenie
    int score;                 // Wynik
    bool gameOver;             // Czy koniec gry??
} GameState;

void initializeGame(GameState *state);  //yes
void drawBoard(GameState *state);       //yes
void handleInput(GameState *state);     //yes
void updateGame(GameState *state);      //yes
bool checkCollision(GameState *state);  //yes
void growSnake(GameState *state);       //yes 
void spawnFood(GameState *state);    //in progress

int main() {
    GameState state;
    initializeGame(&state);

    while(!state.gameOver) {
        handleInput(&state);
        updateGame(&state);
        drawBoard(&state);
        usleep(100000);
    }
   
 
    // Po zakończeniu gry
    mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "Game Over!");
    refresh();
    usleep(2000000); // Czekaj 2s
    endwin(); //exit ncurses
    return 0;
}

void initializeGame(GameState *state) {
    initscr();    //tryb ncurses
    noecho();
    curs_set(FALSE); //brak kursora
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); //nie czekaj na input    
    start_color();
    
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // kolor snake'a
    init_pair(2, COLOR_RED, COLOR_BLACK);    // kolor owocu, gameover
    init_pair(3, COLOR_BLUE, COLOR_BLACK);   // kolor scian
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);   // kolor wyniku 
    
    // score
    state->score = 0;
    state->gameOver = false;    
    
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
        state->board[i][HEIGHT-1] = '_';
    }

    // snake
    state->snake.length = 1;
    state->snake.direction = 'D';
    state->snake.segments[0].x = WIDTH / 2;
    state->snake.segments[0].y = HEIGHT / 2;

    // jedzenie 
    srand(time(NULL));
    spawnFood(state);
    
    clear();
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
    for (int i = 0; i < state->snake.length; i++)
        mvprintw(state->snake.segments[i].y, state->snake.segments[i].x, "O");
    attroff(COLOR_PAIR(1));
    
    // jedzenie
    attron(COLOR_PAIR(2));
    mvprintw(state->food.y, state->food.x, "@");  
    attroff(COLOR_PAIR(2));
    refresh();
    
    //wynik
    attron(COLOR_PAIR(4));
    mvprintw(HEIGHT, 0, "Score: %d", state->score);
    mvprintw(2, WIDTH+5, "Score: %d", state->score);
    attroff(COLOR_PAIR(4));
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
    }
}

void updateGame(GameState *state) {
    // ruch ciala weza
    for (int i = state->snake.length - 1; i > 0; i--) {
        state->snake.segments[i] = state->snake.segments[i - 1];
    }

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
    if (state->snake.segments[0].x == state->food.x && state->snake.segments[0].y == state->food.y) {
        state->score += 100;
        growSnake(state);
        spawnFood(state);    
    }
}

bool checkCollision(GameState *state) {
    // kolizja ściana
    if (state->snake.segments[0].x <= 0 || state->snake.segments[0].x >= WIDTH-1 ||
        state->snake.segments[0].y <= 0 || state->snake.segments[0].y >= HEIGHT-1) {
            return true;
    }
    
    // kolizja cialo weza
    if (state->snake.length <= 1)
        return false;
    for (int i = 1; i < state->snake.length; i++) {
        if (state->snake.segments[0].x == state->snake.segments[i].x && 
            state->snake.segments[0].y == state->snake.segments[i].y) {
            return true;
        }
    }
    
    return false;
}

void growSnake(GameState *state) {
    if (state->snake.length < MAX_LENGTH)
        state->snake.length++;
}

void spawnFood(GameState *state) {
    bool validPosition;
    
    do {
        validPosition = true;
        // Losowanie pozycji owoca
        state->food.x = 1 + rand() % (WIDTH - 2);
        state->food.y = 1 + rand() % (HEIGHT - 2);

        // czy nie jest na wezu
        for (int i = 0; i < state->snake.length; i++) {
            if (state->food.x == state->snake.segments[i].x &&
                state->food.y == state->snake.segments[i].y) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);
}
