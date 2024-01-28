#include <ncurses.h>
#include <stdio.h>
#include <unistd.h> // for sleep

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

typedef struct {
    int x;
    int y;
} Position;

WINDOW* create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW* local_win);

int main(int argc, char* argv[]) {
    initscr();             // Start curses mode
    cbreak();              // Line buffering disabled
    keypad(stdscr, TRUE);  // Enable function keys
    nodelay(stdscr, TRUE); // Make getch non-blocking
    noecho();              // Don't echo keypresses

    // Game loop
    int ch;
    int last_ch;
    int milisecondsToSleep = 300;
    Position infoPosition = {1, 1};
    Position snakePosition = {3, 3};
    enum Direction direction = RIGHT;

    int gameWindowHeight = 20;
    int gameWindowWidth = 60;
    Position gameWindowPosition = {3, 3};

    int uiWindowHeight = 5;
    int uiWindowWidth = 60;
    Position uiWindowPosition = {3, 23};

    WINDOW* gameWindow = create_newwin(gameWindowHeight, gameWindowWidth, gameWindowPosition.y, gameWindowPosition.x);
    WINDOW* uiWindow = create_newwin(uiWindowHeight, uiWindowWidth, uiWindowPosition.y, uiWindowPosition.x);

    while (1) {
        // Read all available characters
        while ((ch = getch()) != ERR) {
            last_ch = ch; // Store the last character
        }

        if (last_ch != ERR) {
            // Key was pressed
            switch (last_ch) {
            case KEY_UP:
                direction = UP;
                break;
            case KEY_DOWN:
                direction = DOWN;
                break;
            case KEY_LEFT:
                direction = LEFT;
                break;
            case KEY_RIGHT:
                direction = RIGHT;
                break;
            default:
                break;
            }
        }
        switch (direction) {
        case UP:
            snakePosition.y -= 1;
            break;
        case DOWN:
            snakePosition.y += 1;
            break;
        case LEFT:
            snakePosition.x -= 1;
            break;
        case RIGHT:
            snakePosition.x += 1;
            break;

        default:
            break;
        }

        // Game rendering
        wclear(gameWindow);
        mvwprintw(gameWindow, snakePosition.y, snakePosition.x, "snake");
        box(gameWindow, 0, 0);
        wrefresh(gameWindow); // Update game window

        // UI rendering
        wclear(uiWindow);
        mvwprintw(uiWindow, infoPosition.y, infoPosition.x, "Last key pressed: %d", last_ch);
        box(uiWindow, 0, 0);
        wrefresh(uiWindow); // Update game window

        usleep(1000 * milisecondsToSleep); // Wait for 1 second
    }

    endwin(); // End curses mode
    return 0;
}

WINDOW* create_newwin(int height, int width, int starty, int startx) {
    WINDOW* local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0); /* 0, 0 gives default characters
                           * for the vertical and horizontal
                           * lines			*/
    wrefresh(local_win);  /* Show that box 		*/

    return local_win;
}
void destroy_win(WINDOW* local_win) {
    /* box(local_win, ' ', ' '); : This won't produce the desired
     * result of erasing the window. It will leave it's four corners
     * and so an ugly remnant of window.
     */
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    /* The parameters taken are
     * 1. win: the window on which to operate
     * 2. ls: character to be used for the left side of the window
     * 3. rs: character to be used for the right side of the window
     * 4. ts: character to be used for the top side of the window
     * 5. bs: character to be used for the bottom side of the window
     * 6. tl: character to be used for the top left corner of the window
     * 7. tr: character to be used for the top right corner of the window
     * 8. bl: character to be used for the bottom left corner of the window
     * 9. br: character to be used for the bottom right corner of the window
     */
    wrefresh(local_win);
    delwin(local_win);
}
