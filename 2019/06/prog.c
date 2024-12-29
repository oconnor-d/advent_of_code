#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/map.c"
#include "../../utils/string.c"

void problem1(char* inputFilePath) {
    /*
    Get the number of objects orbiting other objects (plus indirect orbits through other orbiting objects).

    Store all the orbit connections in a map, and then for each object in orbit, backtrack the map until
    you reach an object that's not itself orbiting anything else.

    So, if AAA)BBB and BBB)CCC, there'ed by a mapping from CCC->BBB which counts as 1 orbit, then from
    BBB->AAA as the second orbit, and since AAA is orbiting nothing it all ends there.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    int parserEndIdx;

    // Orbiting Object -> Object it's Orbiting
    Map connections;
    initMap(&connections);
    char *object, *orbitingObject;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        object = parseUntil(line, 0, &parserEndIdx, ')');
        orbitingObject = parseUntil(line, parserEndIdx + 1, &parserEndIdx, ')');

        setMap(&connections, orbitingObject, object);
    }

    fclose(inputFile);

    // For each orbiting object, backtrack through ALL of it's orbits until the base object, keeping
    // count of the number of jumps taken.
    int totalOrbitCount = 0;
    int orbitCount;
    char* key;
    for (int idx = 0; idx < connections.capacity; idx += 1) {
        if (connections.entries[idx].key == NULL) continue;

        key = connections.entries[idx].key;
        orbitCount = 0;
        while (getMap(&connections, key, &key)) orbitCount += 1;

        totalOrbitCount += orbitCount;
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", totalOrbitCount, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Find the number of orbital jumps need for you (YOU) and Santa (SAN) to meet up at a common, reachable object.

    Like part 1, keep all the orbit connections in the map. From there, backtrack on both SAN and YOU, keeping
    track of the jump distance for each connected object in two different respective maps.

    Then, loop through the matching keys of both to get the minimum number of hops required to meet up.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    int parserEndIdx;

    // Orbiting Object -> Object it's Orbiting
    Map connections;
    initMap(&connections);
    char *object, *orbitingObject;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        object = parseUntil(line, 0, &parserEndIdx, ')');
        orbitingObject = parseUntil(line, parserEndIdx + 1, &parserEndIdx, ')');

        setMap(&connections, orbitingObject, object);
    }

    fclose(inputFile);

    // Store the distance both YOU and SAN are from all respective orbiting objects.
    LLongMap santaOrbits, yourOrbits;
    initLLongMap(&santaOrbits);
    initLLongMap(&yourOrbits);

    long long orbitDistance = 0;
    char* key = "SAN";
    while (getMap(&connections, key, &key)) {
        setLLongMap(&santaOrbits, key, orbitDistance);
        orbitDistance += 1;
    }

    orbitDistance = 0;
    key = "YOU";
    while (getMap(&connections, key, &key)) {
        setLLongMap(&yourOrbits, key, orbitDistance);
        orbitDistance += 1;
    }

    // Loop through matching keys in the two orbit distance maps, keeping track of the least distance.
    int minDistance = INT_MAX;
    int distance;
    for (int idx = 0; idx < yourOrbits.capacity; idx += 1) {
        if (yourOrbits.entries[idx].key == NULL) continue;

        for (int santaIdx = 0; santaIdx < santaOrbits.capacity; santaIdx += 1) {
            if (santaOrbits.entries[santaIdx].key == NULL) continue;
            if (strcmp(santaOrbits.entries[santaIdx].key, yourOrbits.entries[idx].key) != 0) continue;

            // Matching orbit, compute the distance and store it if it's the minimum yet found.
            distance = yourOrbits.entries[idx].value + santaOrbits.entries[santaIdx].value;
            if (distance < minDistance) minDistance = distance;
        }
    }

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", minDistance, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
