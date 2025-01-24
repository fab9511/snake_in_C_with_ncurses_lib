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

//rodzaje owocow
typedef enum {
    NORMAL, 
    SPECIAL,
    POISON
} FoodType;

typedef struct {
    int x, y;  // Pozycja jedzenia
    FoodType type;
    bool active;
    time_t lastSpawnTime; 
} Food;

//poziom trudnosci
typedef enum {
    EASY,
    MEDIUM,
    HARD
} Difficulty;

typedef struct {
    char board[HEIGHT][WIDTH]; // Plansza gry
    Snake snake;               // gracz
    Food food[3];                 // Jedzenie  0-normal 1-special 2-poison  
    int score;                 // Wynik
    HighScore scores[MAX_SCORES];
    Difficulty difficulty; // Poziom trudnosci
    int scoreCount;     //wynik gracza
    int speed;          //predkosc gry
    int obstacles;      
    time_t startTime;       
    bool gameOver;             // Czy koniec gry??
} GameState;

void displayMenu(GameState *state);     //yes
void chooseDifficulty(GameState *state); //yes
void applyDifficultySettings(GameState *state); //yes 
void displayMoves();                    //yes
int loadHighScores(HighScore scores[]); //yes
void displayHighscores(GameState *state, int count); //yes
void sortHighScores(HighScore scores[], int count);  //yes
void saveHighScores(HighScore scores[], int count); //yes
void addHighScore(HighScore scores[], int *count, char *name, int score); //yes
void initializeGame(GameState *state);  //yes
void spawnObstacles(GameState *state);   //in progress
void drawBoard(GameState *state);       //yes
void handleInput(GameState *state);     //yes
void updateGame(GameState *state);      //yes
bool checkCollision(GameState *state);  //yes
void growSnake(GameState *state);       //yes 
void spawnFood(GameState *state, FoodType type); //yes
void drawTimer(GameState *state);        //yes 
void drawFood(GameState *state);        //yes
void checkEatingFruit(GameState *state); //yes
void changePositionFood(GameState *state);  //yes

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
                chooseDifficulty(state);
                break;
            case '3':
                displayHighscores(state, state->scoreCount);
                break;
            case '4':
                displayMoves();
                break;
            case '5':
                endwin(); //exit ncurses
                exit(0); // Wyjście z gry
           // default:
             //   mvprintw(13, WIDTH / 2 - 10, "");
               // refresh();
              //  usleep(1000000);
        }
    }
}

void chooseDifficulty(GameState *state) {
    clear();
    int choice = 0; 

    mvprintw(5, WIDTH / 2 - 5, "=== DIFICULTY ===");
    mvprintw(7, WIDTH / 2 - 5, "1. EASY");
    mvprintw(8, WIDTH / 2 - 5, "2. MEDIUM");
    mvprintw(9, WIDTH / 2 - 5, "3. HARDCORE");
    mvprintw(11, WIDTH / 2 - 5, "Choose option: ");
    refresh();
    
    nodelay(stdscr, FALSE);   
    choice = getch(); // Odczyt wyboru gracza

    switch (choice) {
        case '1':
            state->difficulty = EASY;
            break;
        case '2':
            state->difficulty = MEDIUM;
            break;
        case '3':
            state->difficulty = HARD;
            break;
        default:
            state->difficulty = EASY;
 
   }
    
    clear();
    mvprintw(5, WIDTH / 2 - 7, "Difficulty set to: ");
    switch (state->difficulty) {
        case EASY:
            mvprintw(7, WIDTH / 2 - 7, "EASY");
            break;
        case MEDIUM:
            mvprintw(7, WIDTH / 2 - 7, "MEDIUM");
            break;
        case HARD:
            mvprintw(7, WIDTH / 2 - 7, "HARD");
            break;
    }
    refresh();
    usleep(1600000);
    clear();
}

void displayMoves() {
    clear();
    mvprintw(3, WIDTH / 2 - 8, "=== HOW TO PLAY ===");
    mvprintw(5, WIDTH / 2 - 10, "Snake's moves:");
    mvprintw(6, WIDTH / 2 - 10, "W or key UP - to go up");
    mvprintw(7, WIDTH / 2 - 10, "S or key DOWN - to go down");
    mvprintw(8, WIDTH / 2 - 10, "A or key LEFT - to go left");
    mvprintw(9, WIDTH / 2 - 10, "D or key RIGHT - to go right");
    mvprintw(11, WIDTH / 2 - 10, "Game controls:");
    mvprintw(12, WIDTH / 2 - 10, "P - to pause a game");
    mvprintw(13, WIDTH / 2 - 10, "X - to exit during a game");
    mvprintw(15, WIDTH / 2 - 10, "=== Obstacles ===");
    mvprintw(17, WIDTH / 2 - 10, "Avoid hitting obstacles (walls or special barriers)");
    mvprintw(18, WIDTH / 2 - 10, "If you hit one, the game will end");
    mvprintw(20, WIDTH / 2 - 10, "=== How the snake grows ===");
    mvprintw(22, WIDTH / 2 - 10, "The snake grows by one segment after eating a fruit");
    mvprintw(25, WIDTH / 2 - 10, "=== Fruits ===");
    mvprintw(27, WIDTH / 2 - 10, "There are three types of fruits:");
    mvprintw(29, WIDTH / 2 - 10, "@ Regular Fruit (Normal):");
    mvprintw(30, WIDTH / 2 - 5, "    Adds point to your score and grows the snake");
    mvprintw(31, WIDTH / 2 - 10, "* Special Fruit:");
    mvprintw(32, WIDTH / 2 - 5, "    Adds more points than regular fruit (depends on the difficulty)");
    mvprintw(33, WIDTH / 2 - 10, "X Poison Fruit:");
    mvprintw(34, WIDTH / 2 - 5, "    Causes the snake to shrink by one segment");
    mvprintw(35, WIDTH / 2 - 5, "    Reduces the player's score by a small amount");    

    mvprintw(37, WIDTH / 2 - 10, "Press any key to go back.");

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
        state->board[i][HEIGHT-1] = 175;
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

void drawTimer(GameState *state) {
    time_t currentTime = time(NULL);
    int elapsedTime = (int)(currentTime - state->startTime);

    mvprintw(2, WIDTH + 5, "Time: %02d:%02d", elapsedTime / 60, elapsedTime % 60);
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

void growSnake(GameState *state) {
    if (state->snake.length < MAX_LENGTH)
        state->snake.length++;
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
