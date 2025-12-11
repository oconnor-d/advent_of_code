#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

#define MAX_NODES 700

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int idLookup[26][26][26];
    for (int i = 0; i < 26; i += 1)
        for (int j = 0; j < 26; j += 1)
            for (int k = 0; k < 26; k += 1)
                idLookup[i][j][k] = -1;

    bool graph[MAX_NODES][MAX_NODES] = {false};

    int maxId = 0;
    int sourceId, destId;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        sourceId = idLookup[line[0] - 'a'][line[1] - 'a'][line[2] - 'a'];
        if (sourceId == -1) {
            idLookup[line[0] - 'a'][line[1] - 'a'][line[2] - 'a'] = maxId;
            sourceId = maxId;

            maxId += 1;
        }

        int idx = 5;
        while (idx < lineLen - 1) {
            destId = idLookup[line[idx] - 'a'][line[idx + 1] - 'a'][line[idx + 2] - 'a'];

            if (destId == -1) {
                idLookup[line[idx] - 'a'][line[idx + 1] - 'a'][line[idx + 2] - 'a'] = maxId;
                destId = maxId;

                maxId += 1;
            }

            graph[sourceId][destId] = true;

            idx += 4;
        }
    }

    // DFS from 'you' to all 'out's.
    int outId = idLookup['o' - 'a']['u' - 'a']['t' - 'a'];
    int paths = 0;
    IntArray visitStack;
    initIntArray(&visitStack, 333);
    insertIntArray(&visitStack, idLookup['y' - 'a']['o' - 'a']['u' - 'a']);
    while (visitStack.numItems > 0) {
        sourceId = popIntArray(&visitStack);

        for (int id = 0; id <= maxId; id += 1) {
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

long long** cache;
bool graph[MAX_NODES][MAX_NODES] = {false};

long long getPaths(int startId, int outId, int maxId) {
    if (startId == outId) {
        return 1;
    }
    if (cache[startId][outId] != -1) {
        return cache[startId][outId];
    }

    long long paths = 0;
    for (int idx = 0; idx <= maxId; idx += 1) {
        if (!graph[startId][idx]) continue;

        paths += getPaths(idx, outId, maxId);
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

    int idLookup[26][26][26];
    for (int i = 0; i < 26; i += 1)
        for (int j = 0; j < 26; j += 1)
            for (int k = 0; k < 26; k += 1)
                idLookup[i][j][k] = -1;

    int maxId = 0;
    int sourceId, destId;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        sourceId = idLookup[line[0] - 'a'][line[1] - 'a'][line[2] - 'a'];
        if (sourceId == -1) {
            idLookup[line[0] - 'a'][line[1] - 'a'][line[2] - 'a'] = maxId;
            sourceId = maxId;

            maxId += 1;
        }

        int idx = 5;
        while (idx < lineLen - 1) {
            destId = idLookup[line[idx] - 'a'][line[idx + 1] - 'a'][line[idx + 2] - 'a'];

            if (destId == -1) {
                idLookup[line[idx] - 'a'][line[idx + 1] - 'a'][line[idx + 2] - 'a'] = maxId;
                destId = maxId;

                maxId += 1;
            }

            graph[sourceId][destId] = true;

            idx += 4;
        }
    }

    cache = malloc((maxId + 1) * sizeof(long long));
    for (int i = 0; i <= maxId; i += 1) {
        cache[i] = malloc((maxId + 1) * sizeof(long long));
        for (int j = 0; j <= maxId; j += 1) {
            cache[i][j] = -1;
        }
    }

    // DFS from 'svr' to all 'out's, keeping track if we've hit 'fft' and 'dac' before counting it as a valid path.
    int outId = idLookup['o' - 'a']['u' - 'a']['t' - 'a'];
    int fftId = idLookup['f' - 'a']['f' - 'a']['t' - 'a'];
    int dacId = idLookup['d' - 'a']['a' - 'a']['c' - 'a'];
    int svrId = idLookup['s' - 'a']['v' - 'a']['r' - 'a'];

    long long paths = getPaths(svrId, fftId, maxId) * getPaths(fftId, dacId, maxId) * getPaths(dacId, outId, maxId);

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", paths, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
