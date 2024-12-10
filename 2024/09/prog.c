#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"

#define INPUT_MAX_SIZE 20000
#define DISK_MAP_STARTING_SIZE 100000
#define FREE_SPACE -1

void problem1(char* inputFilePath) {
    /*
    The problem is asking us to move file blocks in a fragmented drive as right as possible to de-fragment
    it, and then compute the checksum (sum of fileId * idx in disk-map) for the disk-map.

    First we convert the input ("<block-size-1><empty-space-1>...") into the disk-map, which looks something
    like: 3222421 -> "000..11..2222..3" for example

    This program doesn't actually move any of the file blocks around, but jumps around from the front of the
    map and the back of the map when computing the check sum.

    It computes the check sum as normal from the front until it hits a free space, at which point it starts
    reading the map from the back, and using the first file id found to multiply the forward-going idx
    by.

    NOTE:
    Idea for speeding up -> Instead of building the diskmap out, with each block and free space represented
    by it's own spot in the array, keep the original input as what we're processing.

    Going forward in the array, if we're at a file block compute the checkSum for the file id and the
    "idx" it's currently at, if file id 5 has 4 blocks, we'd do checkSum += (5 * idx) + (5 * idx + 1) ...
    and so on.

    When we're on an empty space, subtract the file blocks from the back of the array, basically moving
    them to the front, then add them to the checksum. Keep taking from the back file blocks until none
    are left, and then move on to the next file block at the end of the array.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char input[INPUT_MAX_SIZE];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_SIZE, inputFile);

    fclose(inputFile);

    IntArray diskMap;
    initIntArray(&diskMap, DISK_MAP_STARTING_SIZE);

    // Read the input into a diskmap, where each file block is represented by it's
    // file id, and an empty space is represented by FREE_SPACE.
    //
    // 12345 -> 0..111....22222
    int fileBlocks, freeSpace, fileIdx = 0;
    for (int idx = 0; idx < fileLen; idx += 2) {
        fileBlocks = input[idx] - '0';
        freeSpace = input[idx + 1] - '0';

        for (int blockIdx = 0; blockIdx < fileBlocks; blockIdx += 1) {
            insertIntArray(&diskMap, fileIdx);
        }

        for (int freeSpaceIdx = 0; freeSpaceIdx < freeSpace; freeSpaceIdx += 1) {
            insertIntArray(&diskMap, FREE_SPACE);
        }

        fileIdx += 1;
    }

    // Go through the disk map from both directions to compute the checksum. Use the forward idx
    // to compute the checksum (doing file id * forward idx) until you hit an empty space, then
    // searching for the first non-empty space going backwards. Once the non-empty file id is
    // found from the back of the disk map, use that for the checksum.
    //
    // All indices have been processed once the forwards idx > backwards idx, at which point
    // the checksum is complete.
    long long checkSum = 0;
    int forwardsIdx = 0, backwardsIdx = diskMap.numItems - 1;
    while (forwardsIdx <= backwardsIdx) {
        if (diskMap.data[forwardsIdx] == FREE_SPACE) {
            // Get the first non-empty backwards index from the back.
            while (diskMap.data[backwardsIdx] == FREE_SPACE) backwardsIdx -= 1;

            checkSum += forwardsIdx * diskMap.data[backwardsIdx];
            backwardsIdx -= 1;  // Move this index back one to point out the next space.
        } else {
            checkSum += forwardsIdx * diskMap.data[forwardsIdx];
        }

        forwardsIdx += 1;
    }

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", checkSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Instead of moving a file block-by-block, this time we only move files if ALL blocks can be moved
    into an earlier empty space all at once.

    This time we actually are moving the file blocks in the disk-map, so we can easily tell which free
    spaces have been filled. Besides that, the algorithm is somewhat the same, going from the back,
    get the size of the file you need to move, and then from the very front of the array try to
    find a run of free space big enough to fit the file. If enough free space is found, move the
    file ids representing the blocks to the free space it fills.

    Keep doing that until you go through the whole disk-map, backwards.

    NOTE:
    Similarly I think this could also be sped up by doing simpler math on the unexpanded disk-map.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char input[INPUT_MAX_SIZE];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_SIZE, inputFile);

    fclose(inputFile);

    IntArray diskMap;
    initIntArray(&diskMap, DISK_MAP_STARTING_SIZE);

    // Read the input into a diskmap, where each file block is represented by it's
    // file id, and an empty space is represented by FREE_SPACE.
    //
    // 12345 -> 0..111....22222
    int fileBlocks, freeSpace, fileIdx = 0;
    for (int idx = 0; idx < fileLen; idx += 2) {
        fileBlocks = input[idx] - '0';
        freeSpace = input[idx + 1] - '0';

        for (int blockIdx = 0; blockIdx < fileBlocks; blockIdx += 1) {
            insertIntArray(&diskMap, fileIdx);
        }

        for (int freeSpaceIdx = 0; freeSpaceIdx < freeSpace; freeSpaceIdx += 1) {
            insertIntArray(&diskMap, FREE_SPACE);
        }

        fileIdx += 1;
    }

    // Go through the disk map from both directions to compute the checksum. Use the forward idx
    // to compute the checksum (doing file id * forward idx) until you hit an empty space, then
    // searching for the first non-empty space going backwards. Once the non-empty file id is
    // found from the back of the disk map, use that for the checksum.
    //
    // All indices have been processed once the forwards idx > backwards idx, at which point
    // the checksum is complete.
    int forwardsIdx = 0, backwardsIdx = diskMap.numItems - 1, spaceToFill = 0, fileId, emptySpaces;
    while (backwardsIdx >= 0) {
        // Find the first block at the end of the map.
        if (diskMap.data[backwardsIdx] == FREE_SPACE) {
            backwardsIdx -= 1;
            continue;
        }

        // Once we found it, figure out how many blocks make up the file.
        forwardsIdx = 0;
        fileId = diskMap.data[backwardsIdx];

        spaceToFill = 0;
        while (diskMap.data[backwardsIdx] == fileId) {
            spaceToFill += 1;
            backwardsIdx -= 1;
        }

        // Now, going forward, find the first run of empty space that would fit the file.
        while (forwardsIdx < backwardsIdx + 1) {
            emptySpaces = 0;

            if (diskMap.data[forwardsIdx] != FREE_SPACE) {
                // Skip occupied spots.
                forwardsIdx += 1;
            } else {
                // Count the free space.
                while (diskMap.data[forwardsIdx] == FREE_SPACE) {
                    emptySpaces += 1;
                    forwardsIdx += 1;
                }

                // Too small, continue on to find the next run of empty space.
                if (emptySpaces < spaceToFill) continue;

                // We can fit, so move the blocks from the end to the start of the run of free space.
                for (int n = 0; n < spaceToFill; n += 1) {
                    diskMap.data[forwardsIdx - emptySpaces + n] = fileId;
                    diskMap.data[backwardsIdx + spaceToFill - n] = FREE_SPACE;
                }

                // We're done, find the next file to move.
                break;
            }
        }
    }

    // Compute the check sum, this time we can just loop through the disk-map, since we
    // already de-fragmented it.
    long long checkSum = 0;
    for (int idx = 0; idx < diskMap.numItems; idx += 1) {
        if (diskMap.data[idx] != FREE_SPACE) {
            checkSum += diskMap.data[idx] * idx;
        }
    }

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", checkSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
