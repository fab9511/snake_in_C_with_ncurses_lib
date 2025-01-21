#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>

#define WIDTH 45 
#define HEIGHT 45
#define MAX_LENGTH 100
#define MAX_SCORES 10
    
typedef struct {
    char name[50];
    int score;
} HighScore;

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
    HighScore scores[MAX_SCORES];
    int scoreCount;
    bool gameOver;             // Czy koniec gry??
} GameState;

void displayMenu(GameState *state);     //yes
void displayMoves();                    //yes
int loadHighScores(HighScore scores[]); //yes
void displayHighscores(GameState *state, int count); //yes
void sortHighScores(HighScore scores[], int count);  //yes
void saveHighScores(HighScore scores[], int count); //yes
void addHighScore(HighScore scores[], int *count, char *name, int score); //yes
int chooseDifficulty();                 //no no
void initializeGame(GameState *state);  //yes
void drawBoard(GameState *state);       //yes
void handleInput(GameState *state);     //yes
void updateGame(GameState *state);      //yes
bool checkCollision(GameState *state);  //yes
void growSnake(GameState *state);       //yes 
void spawnFood(GameState *state);       //yes

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
        usleep(100000);
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
    endwin(); //exit ncurses

    return 0;
}

int loadHighScores(HighScore scores[]) {
    FILE *file = fopen("highscores.txt", "r");
    if (file == NULL) 
        return 0; // Plik nie istnieje, brak wyników

    int count = 0;
    while (count < MAX_SCORES && fscanf(file, "%s %d", scores[count].name, &scores[count].score) == 2) {
        count++;
    }

    fclose(file);
    return count; // ile wyników
}

void displayMenu(GameState *state) {
    initscr();    //tryb ncurses
    noecho();
    curs_set(FALSE); //brak kursora
    clear();

    int choice = 0;

    while (true) {
        mvprintw(5, WIDTH / 2 - 5, "=== SNAKE GAME ===");
        mvprintw(7, WIDTH / 2 - 5, "1. PLAY");
        mvprintw(8, WIDTH / 2 - 5, "2. CHOOSE DIFICULTY");
        mvprintw(9, WIDTH / 2 - 5, "3. HIGHSCORE");
        mvprintw(10, WIDTH / 2 - 5, "4. HOW TO PLAY");
        mvprintw(11, WIDTH / 2 - 5, "5. EXIT");
        mvprintw(13, WIDTH / 2 - 5, "Choose option: ");
        refresh();
        
        choice = getch(); // Odczyt wyboru gracza
        
        switch (choice) {
            case '1':
                clear();
                return; // Rozpocznij grę
            case '2':
                //chooseDifficulty();
                return;
            case '3':
                displayHighscores(state, state->scoreCount);
                break;
            case '4':
                displayMoves();
                break;
            case '5':
                endwin();
                exit(0); // Wyjście z gry
           // default:
             //   mvprintw(13, WIDTH / 2 - 10, "");
               // refresh();
              //  usleep(1000000);
        }
    }
}

void displayMoves() {
    clear();
    mvprintw(3, WIDTH / 2 - 8, "=== HOW TO PLAY ===");
    mvprintw(5, WIDTH / 2 - 10, "Snake's moves:");
    mvprintw(6, WIDTH / 2 - 10, "W or key UP - to go up");
    mvprintw(7, WIDTH / 2 - 10, "S or key DOWN - to go down");
    mvprintw(8, WIDTH / 2 - 10, "A or key LEFT - to go left");
    mvprintw(9, WIDTH / 2 - 10, "D or key RIGHT - to go right");
    mvprintw(11, WIDTH / 2 - 10, "Others:");
    mvprintw(12, WIDTH / 2 - 10, "P - to pause a game");
    mvprintw(13, WIDTH / 2 - 10, "X - to exit during a game");
    mvprintw(15, WIDTH / 2 - 10, "Press any key to go back.");
    refresh();
    nodelay(stdscr, FALSE);
    getch();
    nodelay(stdscr, TRUE);
    clear();
}
void displayHighscores(GameState *state, int count) {
    clear();
    mvprintw(3, WIDTH / 2 - 8, "=== HIGHSCORE ===");

    if (count == 0) {
        mvprintw(10, WIDTH / 2 - 5, "No highscore yet.");
    } else {
        for (int i = 0; i < count; i++) {
            mvprintw(5 + i, WIDTH / 2 - 8, "%d. %s - %d", i + 1, state->scores[i].name, state->scores[i].score);
        }
    }

    mvprintw(5 + count + 2, WIDTH / 2 - 8, "Press any key to go back.");
    refresh();
    nodelay(stdscr, FALSE);
    getch();
    nodelay(stdscr, TRUE);

    clear();
}

void sortHighScores(HighScore scores[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scores[j].score < scores[j + 1].score) {
                HighScore temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }
}

void saveHighScores(HighScore scores[], int count) {
    FILE *file = fopen("highscores.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < count && i < MAX_SCORES; i++) {
            fprintf(file, "%s %d\n", scores[i].name, scores[i].score);
        }
        fclose(file);
    }
}

void addHighScore(HighScore scores[], int *count, char *name, int score) {
    if (*count < MAX_SCORES) {
        strcpy(scores[*count].name, name);
        scores[*count].score = score;
        (*count)++;
    } 
    else if (score > scores[*count - 1].score) { 
        strcpy(scores[*count - 1].name, name);
        scores[*count - 1].score = score;
    }

    sortHighScores(scores, *count);
}
 
void initializeGame(GameState *state) {
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
        state->board[i][HEIGHT-1] = 175;
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
    // highscores
    attron(COLOR_PAIR(4));
    mvprintw(8 , WIDTH + 5 , "Highscores:");
    for (int i = 0; i < state->scoreCount; i++) {
        mvprintw(10 + 2*i, WIDTH + 5, "%d. %s - %d", i + 1, state->scores[i].name, state->scores[i].score);
    }
    attroff(COLOR_PAIR(4));
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
    attron(COLOR_PAIR(2));
    mvprintw(state->food.y, state->food.x, "@");  
    attroff(COLOR_PAIR(2));
    refresh();
    
    //wynik
    attron(COLOR_PAIR(4));
    mvprintw(3, WIDTH + 5, "Score: %d", state->score);
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
