#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/string.c"

void problem1(char* inputFilePath) {
    /*
    Given image data (a series of pixels) broken up into layers (each of a given height and width), get the number
    of 1s and 2s in the layer with the least amount of 0s.

    I modified the AOC input to include the image dimensions in the input, as X,Y at the top of the file.

    Knowing the layer's width and height, we can get the number of pixels in the image by X * Y. A tally is kept of
    every 0, 1, and 2 in each layer in respective arrays. Every X * Y pixels, we hop to the next layer.

    Once we have the tallies, loop through the layers and keep the num 1s * num 2s of the layer with the least
    zeros found so far.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    int parserEndIdx;

    // Image dimensions: (X,Y)
    int width, height;
    getline(&line, &lineCap, inputFile);
    width = parseNumber(line, 0, &parserEndIdx);
    height = parseNumber(line, parserEndIdx, &parserEndIdx);

    // Image data
    lineLen = getline(&line, &lineCap, inputFile);
    fclose(inputFile);

    int pixelsInLayer = width * height;
    int numLayers = lineLen / pixelsInLayer;

    // Tallies of 0, 1, and 2s in each layer.
    int zerosInLayer[numLayers];
    int onesInLayer[numLayers];
    int twosInLayer[numLayers];

    // Go through the image data, tallying the 0, 1, and 2s, and jumping to the next layer when X * Y pixels
    // have been processed.
    int layer = 0;
    zerosInLayer[0] = 0;
    onesInLayer[0] = 0;
    twosInLayer[0] = 0;
    for (int idx = 0; idx < lineLen; idx += 1) {
        if (line[idx] == '0') zerosInLayer[layer] += 1;
        if (line[idx] == '1') onesInLayer[layer] += 1;
        if (line[idx] == '2') twosInLayer[layer] += 1;

        if (idx != lineLen - 1 && idx % pixelsInLayer == pixelsInLayer - 1) {
            layer += 1;
            zerosInLayer[layer] = 0;
            onesInLayer[layer] = 0;
            twosInLayer[layer] = 0;
        }
    }

    // Find the num 1s * num 2s of the layer with the least number of zeros.
    int result;
    int leastZeros = INT_MAX;
    for (layer = 0; layer < numLayers; layer += 1) {
        if (zerosInLayer[layer] < leastZeros) {
            leastZeros = zerosInLayer[layer];
            result = onesInLayer[layer] * twosInLayer[layer];
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", result, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    0s are black pixels, 1s are white, and 2s are transparent. With the first layer being topmost, find the message
    in the composite layers.

    Break up the image into layers like in part 1, this time grouping all pixels in a layer together.

    Then, to get the composite image, for each pixel, starting from the topmost layer, store the first non-transparent
    pixel found in the composite layer. If a transparent pixel is found, look at the next layer for it's pixel, and so
    on.

    After that, print it nicely and manually look for the message with your eyeballs.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    int parserEndIdx;

    // Image dimensions: (X,Y)
    int width, height;
    getline(&line, &lineCap, inputFile);
    width = parseNumber(line, 0, &parserEndIdx);
    height = parseNumber(line, parserEndIdx, &parserEndIdx);

    // Image data
    lineLen = getline(&line, &lineCap, inputFile);
    fclose(inputFile);

    int pixelsInLayer = width * height;
    int numLayers = lineLen / pixelsInLayer;

    // The pixels in each layer.
    int layers[numLayers][pixelsInLayer];

    // Store the pixels in each layer.
    int layer = 0, pixelIndex = 0;
    for (int idx = 0; idx < lineLen; idx += 1) {
        layers[layer][pixelIndex] = line[idx] - '0';
        pixelIndex += 1;

        if (idx != lineLen - 1 && idx % pixelsInLayer == pixelsInLayer - 1) {
            layer += 1;
            pixelIndex = 0;
        }
    }

    // Build the composite layer, storing the first non-transparent pixel from the topmost layer down.
    int compositeLayer[pixelsInLayer];
    for (int idx = 0; idx < pixelsInLayer; idx += 1) {
        layer = 0;

        // Get the first non-transparent pixel in a layer.
        while (layers[layer][idx] == 2) layer += 1;

        compositeLayer[idx] = layers[layer][idx];
    }

    clock_t end = clock();
    printf("Problem 02:\n\n");

    // Print the layer nicely.
    for (int idx = 0; idx < pixelsInLayer; idx += 1) {
        if (compositeLayer[idx] == 1) {
            printf("%d", compositeLayer[idx]);
        } else {
            printf(" ");
        }

        if (idx % width == width - 1) printf("\n");
    }

    printf("\n[% .2fms]\n", (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
