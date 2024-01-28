#include <ncurses.h>
#include <stdio.h>
#include <unistd.h> // for sleep

int main(int argc, char* argv[]) {
    initscr();            // Start curses mode
    cbreak();             // Line buffering disabled
    keypad(stdscr, TRUE); // Enable function keys
    noecho();             // Don't echo keypresses

    if (has_colors() == TRUE) {
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
    }
    // Game loop
    int i = 0;
    while (1) {
        if (has_colors() == TRUE) {
            attron(COLOR_PAIR(1));
        }

        clear(); // Clear the screen

        mvprintw(3, i, "s");

        if (has_colors() == TRUE) {
            attroff(COLOR_PAIR(1));
        }

        refresh(); // Update the screen
        i++;
        sleep(1); // Wait for 1 second
    }

    endwin(); // End curses mode
    return 0;
}
