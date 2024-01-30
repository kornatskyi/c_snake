#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
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
Position* find_border_position(int width, int height, Position borderPosition, int borderSize);
int isColliding(Position* borderPositions, int borderSize, Position pos);
int arePositionsEqual(Position a, Position b);

Position* find_border_position(int width, int height, Position borderPosition, int borderSize) {
    Position* borderPositions = malloc(sizeof(Position) * borderSize);

    // First loop: Top  and Bottom borders
    for (int i = 0; i < width; i++) {
        borderPositions[i] = (Position){i, 0};
        borderPositions[i + width] = (Position){i, height - 1};
    }
    // Second loop: Left and Right borders
    for (int i = 0; i < height - 2; i++) {
        borderPositions[2 * width + i] = (Position){0, 1 + i};
        borderPositions[2 * width + height - 2 + i] = (Position){width - 1, 1 + i};
    }

    return borderPositions;
}

int arePositionsEqual(Position a, Position b) {
    return a.x == b.x && a.y == b.y;
}

int isColliding(Position* collisionTargets, int numberOfTargets, Position snakePosition) {
    for (int i = 0; i < numberOfTargets; i++) {
        if (arePositionsEqual(collisionTargets[i], snakePosition)) {
            return 1; // Position is on the border
        }
    }
    return 0; // Position is not on the border
}

void updateSnakePosition(Position* snakeBody, int snakeLength, enum Direction direction) {
    // Update body
    for (int i = snakeLength - 1; i > 0; i--) {
        snakeBody[i] = snakeBody[i - 1];
    }
    // Update head
    switch (direction) {
    case UP:
        snakeBody[0].y -= 1;
        break;
    case DOWN:
        snakeBody[0].y += 1;
        break;
    case LEFT:
        snakeBody[0].x -= 1;
        break;
    case RIGHT:
        snakeBody[0].x += 1;
        break;
    default:
        break;
    }
}

void renderSnake(WINDOW* gameWindow, Position* snakeBody, int snakeLength) {

    for (int i = 0; i < snakeLength; i++) {
        mvwprintw(gameWindow, snakeBody[i].y, snakeBody[i].x, "s");
    }
}

int main(int argc, char* argv[]) {
    initscr();             // Start curses mode
    cbreak();              // Line buffering disabled
    keypad(stdscr, TRUE);  // Enable function keys
    nodelay(stdscr, TRUE); // Make getch non-blocking
    noecho();              // Don't echo keypresses

    // GLOBAL
    int ch;
    int last_ch;
    int milisecondsToSleep = 300;

    // GAME
    enum Direction direction = RIGHT;
    int gameWindowHeight = 20;
    int gameWindowWidth = 60;
    Position gameWindowPosition = {3, 3};
    int borderLength = (gameWindowWidth * 2 + gameWindowHeight * 2) - 4;
    Position* borderPositions = find_border_position(gameWindowWidth, gameWindowHeight, gameWindowPosition, borderLength);

    WINDOW* gameWindow = create_newwin(gameWindowHeight, gameWindowWidth, gameWindowPosition.y, gameWindowPosition.x);

    int snakeLength = 8;
    Position snakeBody[10];
    snakeBody[0] = (Position){10, 10};
    snakeBody[1] = (Position){9, 10};
    snakeBody[2] = (Position){8, 10};
    snakeBody[3] = (Position){7, 10};
    snakeBody[4] = (Position){6, 10};
    snakeBody[5] = (Position){6, 9};
    snakeBody[6] = (Position){6, 8};
    snakeBody[7] = (Position){6, 7};

    // UI
    Position infoPosition = {1, 1};
    int uiWindowHeight = 5;
    int uiWindowWidth = 60;
    Position uiWindowPosition = {3, 23};
    WINDOW* uiWindow = create_newwin(uiWindowHeight, uiWindowWidth, uiWindowPosition.y, uiWindowPosition.x);

    Position foodPositions[1];

    foodPositions[0].x = 15;
    foodPositions[0].y = 15;

    // Game loop
    while (1) {
        // Read all available characters
        while ((ch = getch()) != ERR) {
            last_ch = ch; // Store the last character
        }

        if (last_ch != ERR) {
            // Key was pressed
            switch (last_ch) {
            case KEY_UP:
                if (direction != DOWN) {
                    direction = UP;
                }
                break;
            case KEY_DOWN:
                if (direction != UP) {
                    direction = DOWN;
                }
                break;
            case KEY_LEFT:
                if (direction != RIGHT) {
                    direction = LEFT;
                }
                break;
            case KEY_RIGHT:
                if (direction != LEFT) {
                    direction = RIGHT;
                }
                break;
            default:
                break;
            }
        }

        updateSnakePosition(snakeBody, snakeLength, direction);

        // Game rendering
        wclear(gameWindow);
        renderSnake(gameWindow, snakeBody, snakeLength);
        mvwprintw(gameWindow, foodPositions[0].y, foodPositions[0].x, "f");
        box(gameWindow, 0, 0);
        wrefresh(gameWindow); // Update game window

        // UI rendering
        wclear(uiWindow);

        mvwprintw(uiWindow, infoPosition.y, infoPosition.x, "Last key pressed: %d", last_ch);
        mvwprintw(uiWindow, infoPosition.y + 1, infoPosition.x, "Your position is x: %d, y: %d", snakeBody[0].x, snakeBody[0].y);
        if (isColliding(borderPositions, borderLength, snakeBody[0])) {
            mvwprintw(uiWindow, infoPosition.y + 2, infoPosition.x, "Don't cross the border!!!");
        }
        if (isColliding(foodPositions, 1, snakeBody[0])) {
            mvwprintw(uiWindow, infoPosition.y + 2, infoPosition.x, "Colliding with food!!!");
        }
        box(uiWindow, 0, 0);
        wrefresh(uiWindow); // Update game window

        usleep(1000 * milisecondsToSleep); // Wait for 1 second
    }

    free(borderPositions);
    borderPositions = NULL;
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
