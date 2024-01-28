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

int main(int argc, char* argv[]) {
    initscr();             // Start curses mode
    cbreak();              // Line buffering disabled
    keypad(stdscr, TRUE);  // Enable function keys
    nodelay(stdscr, TRUE); // Make getch non-blocking
    noecho();              // Don't echo keypresses

    if (has_colors() == TRUE) {
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
    }
    // Game loop
    int i = 0;
    int ch;
    int last_ch;
    Position infoPosition = {3, 20};

    Position currentPosition = {3, 20};
    int milisecondsToSleep = 300;

    enum Direction direction = RIGHT;
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
            currentPosition.y -= 1;
            break;
        case DOWN:
            currentPosition.y += 1;
            break;
        case LEFT:
            currentPosition.x -= 1;
            break;
        case RIGHT:
            currentPosition.x += 1;
            break;

        default:
            break;
        }

        if (has_colors() == TRUE) {
            attron(COLOR_PAIR(1));
        }

        clear(); // Clear the screen
        mvprintw(infoPosition.y, infoPosition.x, "Last key pressed: %d", last_ch);

        mvprintw(currentPosition.y, currentPosition.x, "snake");

        if (has_colors() == TRUE) {
            attroff(COLOR_PAIR(1));
        }

        refresh(); // Update the screen
        i++;
        usleep(1000 * milisecondsToSleep); // Wait for 1 second
    }

    endwin(); // End curses mode
    return 0;
}
