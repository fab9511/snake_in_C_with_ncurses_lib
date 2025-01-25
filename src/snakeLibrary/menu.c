#include "snakeLib.h"

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
    usleep(1100000);
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
