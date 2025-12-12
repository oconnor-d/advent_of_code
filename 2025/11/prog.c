#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

#define MAX_NODES 700

int get(int idLookup[26][26][26], char c1, char c2, char c3) {
    return idLookup[c1 - 'a'][c2 - 'a'][c3 - 'a'];
}

void set(int idLookup[26][26][26], char c1, char c2, char c3, int id) {
    idLookup[c1 - 'a'][c2 - 'a'][c3 - 'a'] = id;
}

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Lookup table to map 'xyz' -> an id for use in the adjacency matrix below.
    int idLookup[26][26][26] = {0};
    // Adjacency matrix.
    bool graph[MAX_NODES][MAX_NODES] = {false};

    // Set up the adjacency matrix.
    int maxId = 1;
    int sourceId, destId;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        sourceId = get(idLookup, line[0], line[1], line[2]);
        if (sourceId == 0) {
            set(idLookup, line[0], line[1], line[2], maxId);
            sourceId = maxId;
            maxId += 1;
        }

        int idx = 5;
        while (idx < lineLen - 1) {
            destId = get(idLookup, line[idx], line[idx + 1], line[idx + 2]);

            if (destId == 0) {
                set(idLookup, line[idx], line[idx + 1], line[idx + 2], maxId);
                destId = maxId;
                maxId += 1;
            }

            graph[sourceId][destId] = true;
            idx += 4;
        }
    }

    // DFS from 'you' to all 'out's.
    int outId = get(idLookup, 'o', 'u', 't');
    int paths = 0;
    IntArray visitStack;
    initIntArray(&visitStack, 333);
    insertIntArray(&visitStack, get(idLookup, 'y', 'o', 'u'));
    while (visitStack.numItems > 0) {
        sourceId = popIntArray(&visitStack);

        for (int id = 1; id <= maxId; id += 1) {
            if (!graph[sourceId][id]) continue;

            if (id == outId)
                paths += 1;
            else
                insertIntArray(&visitStack, id);
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", paths, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

// Recursive dfs search using the cache for already traversed paths to speed things up.
long getPaths(bool graph[MAX_NODES][MAX_NODES], int startId, int outId, int maxId, long cache[MAX_NODES][MAX_NODES]) {
    if (startId == outId) return 1;
    if (cache[startId][outId] != -1) return cache[startId][outId];

    long paths = 0;
    for (int idx = 1; idx <= maxId; idx += 1) {
        if (!graph[startId][idx]) continue;

        paths += getPaths(graph, idx, outId, maxId, cache);
    }

    cache[startId][outId] = paths;

    return paths;
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int idLookup[26][26][26] = {0};
    bool graph[MAX_NODES][MAX_NODES] = {false};

    // Caches the number of paths from id1 to id2.
    long cache[MAX_NODES][MAX_NODES] = {0};

    int maxId = 1;
    int sourceId, destId;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        sourceId = get(idLookup, line[0], line[1], line[2]);
        if (sourceId == 0) {
            set(idLookup, line[0], line[1], line[2], maxId);
            sourceId = maxId;
            maxId += 1;
        }

        int idx = 5;
        while (idx < lineLen - 1) {
            destId = get(idLookup, line[idx], line[idx + 1], line[idx + 2]);

            if (destId == 0) {
                set(idLookup, line[idx], line[idx + 1], line[idx + 2], maxId);
                destId = maxId;
                maxId += 1;
            }

            graph[sourceId][destId] = true;
            idx += 4;
        }
    }

    // Initialize the cache with misses.
    for (int i = 1; i <= maxId; i += 1)
        for (int j = 1; j <= maxId; j += 1)
            cache[i][j] = -1;

    // Get all paths from svr to fft, fft to dac, and dac to out, and multiply them.
    int svrId = get(idLookup, 's', 'v', 'r');
    int dacId = get(idLookup, 'd', 'a', 'c');
    int fftId = get(idLookup, 'f', 'f', 't');
    int outId = get(idLookup, 'o', 'u', 't');

    long paths = (getPaths(graph, svrId, fftId, maxId, cache) *
                  getPaths(graph, fftId, dacId, maxId, cache) *
                  getPaths(graph, dacId, outId, maxId, cache));

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %ld [%.2fms]\n", paths, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
