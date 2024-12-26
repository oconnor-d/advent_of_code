#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/set.c"

int pcCharKey(char char0, char char1) {
    /*
    Gets the unique int key of the pc by it's characters. (char0 and char1 must be between 'a' and 'z', lowercase).

    The minimum key is 0 ('aa'), and the maximum is 675 ('zz'), and all pcs map to all ints between those two.
    */
    return (char0 - 'a') * 26 + (char1 - 'a');
}

void problem1(char* inputFilePath) {
    /*
    Gets the count of 3-sized cliques in the LAN network where one of the computers starts with 'T'.

    This is a naive approach, we loop through all possible 3-level connections using an adjacency matrix
    and only count the ones with at least one computer starting with 'T'.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Build the adjacency matrix, each pc name is converted to an int key.
    bool network[700][700] = {0};
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        network[pcCharKey(line[0], line[1])][pcCharKey(line[3], line[4])] = true;
        network[pcCharKey(line[3], line[4])][pcCharKey(line[0], line[1])] = true;
    }
    fclose(inputFile);

    // Tally up the connections for the computers starting with 't'.
    int cliques = 0;

    // Every possible pc name is represented as an int, by (char1 * 26 + char26)
    // Loop through all pc combos and count the ones that are all connected, with at least one computer starting with 'T'
    for (int pc1 = 0; pc1 < 676; pc1 += 1) {
        for (int pc2 = pc1 + 1; pc2 < 676; pc2 += 1) {
            for (int pc3 = pc2 + 1; pc3 < 676; pc3 += 1) {
                // The pc key / 26 gets the int value of the first character (relative to the starting 'a'). The int value of t is 19.
                if (network[pc1][pc2] && network[pc2][pc3] && network[pc3][pc1] && ((pc1 / 26 == 19) || (pc2 / 26 == 19) || (pc3 / 26 == 19))) cliques += 1;
            }
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", cliques, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

// ======================================== Part 2 ========================================

// LAN-party adjacency matrix.
bool connections[676][676];

int compare(const void* a, const void* b) {
    int int_a = *((int*)a);
    int int_b = *((int*)b);

    if (int_a == int_b)
        return 0;
    else if (int_a < int_b)
        return -1;
    else
        return 1;
}

void printClique(IntArraySet* clique) {
    /*
    Prints the clique using the pc's string names, sorted alphabetically in
    ascending order.
    */
    int buffer[clique->numTrue];

    int bufferIdx = 0;
    for (int idx = 0; idx < clique->size; idx += 1) {
        if (!clique->data[idx]) continue;

        buffer[bufferIdx] = idx;
        bufferIdx += 1;
    }

    qsort(buffer, clique->numTrue, sizeof(int), compare);

    for (int idx = 0; idx < clique->numTrue; idx += 1) {
        printf("%c%c", (buffer[idx] / 26) + 'a', (buffer[idx] % 26) + 'a');
        if (idx != clique->numTrue - 1) printf(",");
    }
}

IntArraySet biggestClique;
void BronKerbosch(IntArraySet* R, IntArraySet* P, IntArraySet* X) {
    /*
    The Bron-Kerbosch algorithm for finding maximal cliques: https://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm.

    The largest of the maximal cliques is stored in `biggestClique`. This is pretty slow, but I think most of the slowness
    is due to my naive set implementation.
    */
    if (P->numTrue == 0 && X->numTrue == 0) {
        // We've found a maximal clique, store it if it's the biggest one yet.
        if (R->numTrue > biggestClique.numTrue) biggestClique = *R;
        return;
    }

    for (int v = 0; v < P->size; v += 1) {
        if (!containsIntArraySet(P, v)) continue;

        IntArraySet newR, newP, newX, vNeighbors;
        initIntArraySet(&newR, 676);
        initIntArraySet(&vNeighbors, 676);

        // Copy R into newR.
        for (int rValue = 0; rValue < R->size; rValue += 1) {
            if (containsIntArraySet(R, rValue)) addIntArraySet(&newR, rValue);
        }

        // N(v)
        for (int neighborValue = 0; neighborValue < 676; neighborValue += 1) {
            if (connections[v][neighborValue]) addIntArraySet(&vNeighbors, neighborValue);
        }

        // R ⋃ {v}
        addIntArraySet(&newR, v);
        // P ⋂ N(v)
        intersectionIntArraySet(P, &vNeighbors, &newP);
        // X ⋂ N(v)
        intersectionIntArraySet(X, &vNeighbors, &newX);

        // BronKerbosch1(R ⋃ {v}, P ⋂ N(v), X ⋂ N(v))
        BronKerbosch(&newR, &newP, &newX);

        // P := P \ {v}
        removeIntArraySet(P, v);
        // X := X ⋃ {v}
        addIntArraySet(X, v);
    }
}

void problem2(char* inputFilePath) {
    /*
    Find the biggest clique in the network (not necessarily containing a 't' computer).

    Uses Bron-Kerbosch to find the maximal cliques, storing the largest as the problem solution.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Build the adjacency matrix.
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        connections[pcCharKey(line[0], line[1])][pcCharKey(line[3], line[4])] = true;
        connections[pcCharKey(line[3], line[4])][pcCharKey(line[0], line[1])] = true;
    }
    fclose(inputFile);

    // Initialize the sets for Bron-Kerbosch.
    IntArraySet R, P, X;
    initIntArraySet(&R, 676);
    initIntArraySet(&P, 676);
    initIntArraySet(&X, 676);
    for (int pc = 0; pc < 676; pc += 1) addIntArraySet(&P, pc);

    // Perform the maximal clique search, the largest result would be stored in `biggestClique`.
    BronKerbosch(&R, &P, &X);

    clock_t end = clock();
    printf("Problem 02: ");
    printClique(&biggestClique);
    printf(" [%.2fms]\n", (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
