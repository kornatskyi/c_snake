#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
Position* get_border_position(int width, int height, Position borderPosition, int borderSize);
int isColliding(Position* borderPositions, int borderSize, Position pos);
int arePositionsEqual(Position a, Position b);

Position* get_border_position(int width, int height, Position borderPosition, int borderSize) {
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
        mvwprintw(gameWindow, snakeBody[i].y, snakeBody[i].x, "o");
    }
}

Position get_random_position(int gameWindowWidth, int gameWindowHeight) {
    int randX = (rand() % (gameWindowWidth - 2)) + 1; // +1 because we don't want food to appear on the border
    int randY = (rand() % (gameWindowHeight - 2)) + 1;
    return (Position){randX, randY};
}

int main(int argc, char* argv[]) {
    initscr();             // Start curses mode
    cbreak();              // Line buffering disabled
    keypad(stdscr, TRUE);  // Enable function keys
    nodelay(stdscr, TRUE); // Make getch non-blocking
    noecho();              // Don't echo keypresses

    srand(time(NULL)); // Initialization, should only be called once.

    // Global
    int ch;
    int last_ch;
    int milisecondsToSleep = 200;

    // Game
    enum Direction direction = RIGHT;
    int gameWindowHeight = 20;
    int gameWindowWidth = 20;
    Position gameWindowPosition = {3, 3};
    int snakeLength = 1; // init snake length
    int maxSnakeLength = (gameWindowHeight - 1) * (gameWindowWidth - 1);
    Position snakeBody[maxSnakeLength];
    snakeBody[0] = (Position){10, 10}; // init head position

    Position foodPositions[1];
    foodPositions[0] = get_random_position(gameWindowWidth, gameWindowHeight);

    // Game window
    int borderLength = (gameWindowWidth * 2 + gameWindowHeight * 2) - 4;
    Position* borderPositions = get_border_position(gameWindowWidth, gameWindowHeight, gameWindowPosition, borderLength);
    WINDOW* gameWindow = create_newwin(gameWindowHeight, gameWindowWidth, gameWindowPosition.y, gameWindowPosition.x);

    // UI
    Position infoPosition = {1, 1};
    int uiWindowHeight = 7;
    int uiWindowWidth = 60;
    Position uiWindowPosition = {3, 23};
    WINDOW* uiWindow = create_newwin(uiWindowHeight, uiWindowWidth, uiWindowPosition.y, uiWindowPosition.x);

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

        // check if tail collides with food
        if (isColliding(foodPositions, 1, snakeBody[snakeLength - 1])) {
            // check which direction tail moves
            enum Direction tailDirection;
            Position oldTail = snakeBody[snakeLength - 1];

            updateSnakePosition(snakeBody, snakeLength, direction);

            snakeLength++;
            snakeBody[snakeLength - 1] = oldTail;

            foodPositions[0] = get_random_position(gameWindowWidth, gameWindowHeight); // set new random position for food

        } else {
            updateSnakePosition(snakeBody, snakeLength, direction);
        }

        // Game rendering
        wclear(gameWindow);
        renderSnake(gameWindow, snakeBody, snakeLength);
        mvwprintw(gameWindow, foodPositions[0].y, foodPositions[0].x, "O");
        box(gameWindow, 0, 0);
        wrefresh(gameWindow); // Update game window

        // UI rendering
        wclear(uiWindow);

        mvwprintw(uiWindow, infoPosition.y, infoPosition.x, "Last key pressed: %d", last_ch);
        mvwprintw(uiWindow, infoPosition.y + 1, infoPosition.x, "Your position is x: %d, y: %d", snakeBody[0].x, snakeBody[0].y);

        mvwprintw(uiWindow, infoPosition.y + 2, infoPosition.x, "Food position is x: %d, y: %d", foodPositions[0].x, foodPositions[0].y);
        if (isColliding(borderPositions, borderLength, snakeBody[0]) || (snakeLength > 1 && isColliding(&snakeBody[1], snakeLength - 1, snakeBody[0]))) {
            wclear(uiWindow);
            nodelay(stdscr, FALSE);
            mvwprintw(uiWindow, infoPosition.y + 3, infoPosition.x, "You lost. Press 'q' to quit or 'r' to restart the game!");
            wrefresh(uiWindow);

            int ch = getch();
            while (ch != 'q' && ch != 'Q' && ch != 'r' && ch != 'R') {
                ch = getch();
            }

            if (ch == 'q' || ch == 'Q') {
                break;
            } else if (ch == 'r' || ch == 'R') {
                // Reset the game sate
                nodelay(stdscr, TRUE);
                snakeBody[0] = (Position){10, 10};
                snakeLength = 1;
                foodPositions[0] = get_random_position(gameWindowWidth, gameWindowHeight);

                // Refresh windows
                wclear(gameWindow);
                wclear(uiWindow);
                box(gameWindow, 0, 0);
                box(uiWindow, 0, 0);
                wrefresh(gameWindow);
                wrefresh(uiWindow);

                continue;
            }
        }

        if (isColliding(borderPositions, borderLength, snakeBody[0])) {
            mvwprintw(uiWindow, infoPosition.y + 3, infoPosition.x, "Don't cross the border!!!");
        }
        if (isColliding(foodPositions, 1, snakeBody[0])) {
            mvwprintw(uiWindow, infoPosition.y + 3, infoPosition.x, "Colliding with food!!!");
        }
        if (snakeLength > 1 && isColliding(&snakeBody[1], snakeLength - 1, snakeBody[0])) {
            mvwprintw(uiWindow, infoPosition.y + 3, infoPosition.x, "Colliding with itself");
        }
        box(uiWindow, 0, 0);
        wrefresh(uiWindow); // Update game window

        usleep(1000 * milisecondsToSleep);
    }

    free(borderPositions);
    borderPositions = NULL;
    destroy_win(gameWindow);
    destroy_win(uiWindow);
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
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(local_win);
    delwin(local_win);
}
