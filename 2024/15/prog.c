#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"

#define MAP_MAX_ROWS 50
#define MAP_MAX_COLS 50
#define MAX_MOVE_COMMANDS 21000

#define WALL '#'
#define BOX 'O'
#define ROBOT '@'
#define EMPTY_SPACE '.'
#define LEFT_BOX '['
#define RIGHT_BOX ']'

#define NORTH '^'
#define EAST '>'
#define SOUTH 'v'
#define WEST '<'

void problem1(char* inputFilePath) {
    /*
    The problem is Robot Sokoban, we're given a map of boxes to push and walls we cannot with a robot
    placed somewhere on it, and a list of directional move commands to try and take. If we move into
    a box, move the robot and the box (plus any boxes the box is touching in the direction we're going),
    unless we'd hit a wall, in which case we don't move.

    This attempts each move, if there's a WALL in front of the bot the movement fails, if there's an
    empty space in front of the bot it moves, and if there's one or more boxes in the front of the bot
    with an empty space at the end, we move the bot and the line of boxes.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    char map[MAP_MAX_ROWS][MAP_MAX_COLS];
    int row = 0, col = 0, robotRow, robotCol;
    // Read until hitting the newline separating the map from the movement commands.
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 1) {
        col = 0;
        for (; col < lineLen - 1; col += 1) {
            if (line[col] == ROBOT) robotRow = row, robotCol = col;
            map[row][col] = line[col];
        }
        row += 1;
    }

    int numRows = row, numCols = col;

    char moves[MAX_MOVE_COMMANDS];
    int moveIdx = 0;
    // Read the move commands.
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        for (int idx = 0; idx < lineLen; idx += 1) {
            if (line[idx] == '\n') continue;
            moves[moveIdx] = line[idx];
            moveIdx += 1;
        }
    }
    int numMoves = moveIdx;

    fclose(inputFile);

    // Attempt each move, moving any boxes in the way.
    char move;
    int vRow, vCol, endRow, endCol;
    for (int idx = 0; idx < numMoves; idx += 1) {
        move = moves[idx];

        // Get the velocity of movement (only ever moving horizontally or vertically, never both).
        if (move == NORTH) vRow = -1, vCol = 0;
        if (move == EAST) vRow = 0, vCol = 1;
        if (move == SOUTH) vRow = 1, vCol = 0;
        if (move == WEST) vRow = 0, vCol = -1;

        // NOTE: We don't have to out-of-bounds check since the input is enclosed in a wall at the borders.

        // Keep moving in the same direction until you've hit a non-box space.
        endRow = robotRow + vRow, endCol = robotCol + vCol;
        while (map[endRow][endCol] == BOX) endRow += vRow, endCol += vCol;

        // If the space ended on is a wall, no movement can happen.
        if (map[endRow][endCol] == WALL) continue;

        // Otherwise, move all the boxes up one direction.
        while (endRow != robotRow || endCol != robotCol) {
            map[endRow][endCol] = BOX;
            endRow -= vRow, endCol -= vCol;
        }

        // Then, move the robot up one direction, and record it's new position.
        map[robotRow + vRow][robotCol + vCol] = ROBOT;
        map[robotRow][robotCol] = EMPTY_SPACE;
        robotRow += vRow, robotCol += vCol;
    }

    // Tally up the score.
    int score = 0;
    for (int row = 0; row < numRows; row += 1)
        for (int col = 0; col < numCols; col += 1)
            if (map[row][col] == BOX) score += (100 * row) + col;

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", score, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void moveBoxes(char** map, int numRows, int numCols, int startRow, int startCol, int direction) {
    /*
    Attempts to move the box at the given startRow and startCol, and all connected boxes, in the
    given direction (+1 for NORTH, -1 for SOUTH).

    A DFS is performed to get the list of boxes to move. As we perform the search we add the boxes
    we hit to a list of boxes to move, with the boxes being in order of distance (thanks to DFS).

    If a wall is hit during the DFS, no boxes can be moved. Otherwise, all the boxes can be moved.

    To move the boxes, move the boxes from furthest to nearest the robot one by one.
    */

    // Places to visit as a list of 3 coords, like: [row1, leftCol1, rightCol1, ...]
    IntArray visitStack;
    initIntArray(&visitStack, numRows * numCols * 2);

    // The boxes to move as a list of 3 coords, like: [row1, leftCol1, rightCol1, ...]
    IntArray boxesToMove;
    initIntArray(&boxesToMove, numRows * numCols * 2);

    // Initialize the stack with the startRow, and the left and right columns of the box.
    insertIntArray(&visitStack, startRow);
    if (map[startRow][startCol] == LEFT_BOX) {
        insertIntArray(&visitStack, startCol);
        insertIntArray(&visitStack, startCol + 1);
    } else {
        // RIGHT_BOX
        insertIntArray(&visitStack, startCol - 1);
        insertIntArray(&visitStack, startCol);
    }

    // Perform a DFS, storing the found boxes in boxesToMove.
    int row, leftCol, rightCol;
    while (visitStack.numItems > 0) {
        rightCol = popIntArray(&visitStack);
        leftCol = popIntArray(&visitStack);
        row = popIntArray(&visitStack);

        // Store the visited box.
        insertIntArray(&boxesToMove, row);
        insertIntArray(&boxesToMove, leftCol);
        insertIntArray(&boxesToMove, rightCol);

        // Can't move any blocks if even a single one is blocked by a wall.
        if (map[row + direction][leftCol] == WALL || map[row + direction][rightCol] == WALL) return;

        // Three box configurations are possible:

        /*
            []
            []
        */
        if (map[row + direction][leftCol] == LEFT_BOX) {
            insertIntArray(&visitStack, row + direction);
            insertIntArray(&visitStack, leftCol);
            insertIntArray(&visitStack, rightCol);
        }

        /*
            []
             []
        */
        if (map[row + direction][leftCol] == RIGHT_BOX) {
            insertIntArray(&visitStack, row + direction);
            insertIntArray(&visitStack, leftCol - 1);
            insertIntArray(&visitStack, leftCol);
        }

        /*
             []
            []
        */
        if (map[row + direction][rightCol] == LEFT_BOX) {
            insertIntArray(&visitStack, row + direction);
            insertIntArray(&visitStack, rightCol);
            insertIntArray(&visitStack, rightCol + 1);
        }
    }

    // Now, go through the boxes BACKWARDS, moving them if possible.
    while (boxesToMove.numItems > 0) {
        rightCol = popIntArray(&boxesToMove);
        leftCol = popIntArray(&boxesToMove);
        row = popIntArray(&boxesToMove);

        if (map[row + direction][leftCol] == EMPTY_SPACE && map[row + direction][rightCol] == EMPTY_SPACE) {
            map[row + direction][leftCol] = LEFT_BOX;
            map[row + direction][rightCol] = RIGHT_BOX;
            map[row][leftCol] = EMPTY_SPACE;
            map[row][rightCol] = EMPTY_SPACE;
        }
    }
}

void problem2(char* inputFilePath) {
    /*
    Like part 1, this is Robot Sokoban, but instead of 1x1 boxes there are 2x1 boxes, where the
    boxes are 2 spaces wide.

    We expand the input map with the following rules to make these new boxes:

        If the tile is #, the new map contains ## instead.
        If the tile is O, the new map contains [] instead.
        If the tile is ., the new map contains .. instead.
        If the tile is @, the new map contains @. instead.

    Now that boxes are 2 wide, a single box can touch and thus push two boxes at the same time,
    so walking a straight line of boxes like in part 1 doesn't work.

    Instead of the straight line, a DFS is performed to get all the boxes to move from a single movement.
    If ALL the boxes found can be moved (i.e. all box paths end in a free space, and not a wall), start
    moving the boxes from the furthest away to the nearest.

    This updated movement is only done when a box is being pushed vertically, if a box is being pushed
    horizontally the part 1 logic is used, since it can still only affect a straight line of boxes.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // The expanded map is twice as wide. The StringArray struct is being used only to easily
    // allocate the memory needed for the 2D array, since it needs to be passed to the
    // `moveBoxes` function. The actual data structure isn't really used.
    StringArray allocMap;
    initStringArray(&allocMap, MAP_MAX_ROWS, MAP_MAX_COLS * 3);
    char** map = allocMap.data;

    int row = 0, col = 0, mapCol = 0, robotRow, robotCol;
    // Read until hitting the newline separating the map from the commands.
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 1) {
        /*
        If the tile is #, the new map contains ## instead.
        If the tile is O, the new map contains [] instead.
        If the tile is ., the new map contains .. instead.
        If the tile is @, the new map contains @. instead.
        */
        col = 0, mapCol = 0;
        for (; col < lineLen; col += 1) {
            if (line[col] == WALL) {
                map[row][mapCol] = WALL;
                map[row][mapCol + 1] = WALL;
            } else if (line[col] == BOX) {
                map[row][mapCol] = LEFT_BOX;
                map[row][mapCol + 1] = RIGHT_BOX;
            } else if (line[col] == EMPTY_SPACE) {
                map[row][mapCol] = EMPTY_SPACE;
                map[row][mapCol + 1] = EMPTY_SPACE;
            } else if (line[col] == ROBOT) {
                robotRow = row, robotCol = mapCol;

                map[row][mapCol] = ROBOT;
                map[row][mapCol + 1] = EMPTY_SPACE;
            }

            mapCol += 2;
        }
        row += 1;
    }

    int numRows = row, numCols = mapCol;

    char moves[MAX_MOVE_COMMANDS];
    int moveIdx = 0;
    // Read the move commands.
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        for (int idx = 0; idx < lineLen; idx += 1) {
            if (line[idx] == '\n') continue;
            moves[moveIdx] = line[idx];
            moveIdx += 1;
        }
    }
    int numMoves = moveIdx;

    fclose(inputFile);

    // Attempt each movement.
    char move, toPlace;
    int vRow, vCol, endRow, endCol;
    for (int idx = 0; idx < numMoves; idx += 1) {
        move = moves[idx];

        // NOTE: We don't have to bounds check, since the map has a border of walls.

        // Handle the special case of a box being hit vertically.

        // If there's a box to the NORTH/SOUTH, attempt to move all the boxes NORTH/SOUTH, and if that succeeds and the space in
        // front of the robot is now empty, move the robot.
        //
        // NORTH/SOUTH movement uses the DFS approach to get all the boxes to move, while the EAST/WEST movement is
        // the same as in part 1 (with some slight modification to place the right box characters based on what
        // side of the box is being moved at the moment).
        if (move == NORTH && (map[robotRow - 1][robotCol] == LEFT_BOX || map[robotRow - 1][robotCol] == RIGHT_BOX)) {
            moveBoxes(map, numRows, numCols, robotRow - 1, robotCol, -1);
            // After moving boxes, attempt to move the bot.
            if (map[robotRow - 1][robotCol] == EMPTY_SPACE) {
                map[robotRow][robotCol] = EMPTY_SPACE;
                map[robotRow - 1][robotCol] = ROBOT;
                robotRow -= 1;
            }
            continue;
        }

        if (move == SOUTH && (map[robotRow + 1][robotCol] == LEFT_BOX || map[robotRow + 1][robotCol] == RIGHT_BOX)) {
            moveBoxes(map, numRows, numCols, robotRow + 1, robotCol, 1);
            // After moving boxes, attempt to move the bot.
            if (map[robotRow + 1][robotCol] == EMPTY_SPACE) {
                map[robotRow][robotCol] = EMPTY_SPACE;
                map[robotRow + 1][robotCol] = ROBOT;
                robotRow += 1;
            }
            continue;
        }

        // Handle the general cases (horizontal movement, or vertical movement into an empty space).

        // Get the direction to move.
        if (move == NORTH) vRow = -1, vCol = 0;
        // If moving boxes EAST, we'd move the right edge of the last box first: @[]<-
        if (move == EAST) vRow = 0, vCol = 1, toPlace = RIGHT_BOX;
        if (move == SOUTH) vRow = 1, vCol = 0;
        // If moving boxes WEST, we'd move the left edge of the last box first: ->[]@
        if (move == WEST) vRow = 0, vCol = -1, toPlace = LEFT_BOX;

        // Keep moving in the same direction until you've hit a non-box space.
        endRow = robotRow + vRow, endCol = robotCol + vCol;
        while (map[endRow][endCol] == LEFT_BOX || map[endRow][endCol] == RIGHT_BOX) endRow += vRow, endCol += vCol;

        // If the space ended on is a wall, no movement can happen.
        if (map[endRow][endCol] == WALL) continue;

        // Otherwise, move all the boxes left/right one direction.
        while (endRow != robotRow || endCol != robotCol) {
            map[endRow][endCol] = toPlace;
            endRow -= vRow, endCol -= vCol;

            if (toPlace == LEFT_BOX)
                toPlace = RIGHT_BOX;
            else
                toPlace = LEFT_BOX;
        }

        // Then, move the robot up one direction, and record it's new position.
        map[robotRow + vRow][robotCol + vCol] = ROBOT;
        map[robotRow][robotCol] = EMPTY_SPACE;
        robotRow += vRow, robotCol += vCol;
    }

    // Tally up the score.
    int score = 0;
    for (int row = 0; row < numRows; row += 1)
        for (int col = 0; col < numCols; col += 1)
            if (map[row][col] == LEFT_BOX) score += (100 * row) + col;

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", score, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
