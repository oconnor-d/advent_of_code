#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/intcode.c"

#define NETWORK_SIZE 50

void problem1(char* inputFilePath) {
    /*
    Pretty simple - initialize 50 pcs with the program and continuously run them in sequence until
    a packet to 255 is encountered.

    To run each pc, first forward any incoming packets to the pc, or -1 otherwise. Then run it, and push all
    the outgoing packets to the packet queue for the outgoing pcs to consume the next time they run.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Parse the input file, to initialize the network from.
    LLongArray input;
    initLLongArray(&input, 500);
    lineLen = getline(&line, &lineCap, inputFile);

    int parserEndIdx = 0;
    while (parserEndIdx < lineLen - 1) insertLLongArray(&input, parseNumber(line, parserEndIdx, &parserEndIdx));
    fclose(inputFile);

    // Initialize the network.
    IntCodeProgram network[NETWORK_SIZE];
    for (int networkAddress = 0; networkAddress < NETWORK_SIZE; networkAddress += 1) {
        initIntCodeProgramFromLLongArray(&network[networkAddress], &input);
        pushInput(&network[networkAddress], networkAddress);
    }
    IntCodeProgram* pc;

    // The packets (X1, Y1, X2, Y2, ...) for each pc in the network.
    LLongArray packetQueue[NETWORK_SIZE];
    for (int networkAddress = 0; networkAddress < NETWORK_SIZE; networkAddress += 1) {
        initLLongArray(&packetQueue[networkAddress], 100);
    }

    // Until we encounter an outgoing packet to 255, run the network, forwarding the output packets
    // of each pc.
    long long outgoingNetworkAddress, x, y;
    while (true) {
        for (int networkAddress = 0; networkAddress < NETWORK_SIZE; networkAddress += 1) {
            pc = &network[networkAddress];

            // If there are fresh incoming packets for the network, send them all, otherwise send -1.
            if (packetQueue[networkAddress].numItems > 0) {
                for (int idx = 0; idx < packetQueue[networkAddress].numItems; idx += 2) {
                    x = packetQueue[networkAddress].data[idx];
                    y = packetQueue[networkAddress].data[idx + 1];

                    pushInput(pc, x);
                    pushInput(pc, y);
                }

                // We've processed all the packets, clear the queue.
                packetQueue[networkAddress].numItems = 0;
            } else {
                pushInput(pc, -1);
            }

            // Run the program.
            intcodeRun(pc);

            // Store the output packet in the queue.
            for (int idx = 0; idx < pc->output.numItems; idx += 3) {
                outgoingNetworkAddress = pc->output.data[idx];
                x = pc->output.data[idx + 1];
                y = pc->output.data[idx + 2];

                // Found the first packet to 255, we're done.
                if (outgoingNetworkAddress == 255) break;

                // Queue the packet for the next time the outgoing pc is run.
                insertLLongArray(&packetQueue[outgoingNetworkAddress], x);
                insertLLongArray(&packetQueue[outgoingNetworkAddress], y);
            }

            // We've forwarded all the packets, clear the output.
            pc->output.numItems = 0;
        }

        // Found the first packet to 255, we're done.
        if (outgoingNetworkAddress == 255) break;
    }

    clock_t end = clock();
    printf("Problem 01: X=%lld, Y=%lld [%.2fms]\n", x, y, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Very similar to part 1. This time store outgoing packets to 255, and if no packets get sent or
    received, forward the stored 255 packet to pc 0 to re-kick-off the network.

    Stop when the same packet gets forwarded to pc 0 twice in a row.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Parse the input file, to initialize the network from.
    LLongArray input;
    initLLongArray(&input, 500);
    lineLen = getline(&line, &lineCap, inputFile);

    int parserEndIdx = 0;
    while (parserEndIdx < lineLen - 1) insertLLongArray(&input, parseNumber(line, parserEndIdx, &parserEndIdx));
    fclose(inputFile);

    // Initialize the network.
    IntCodeProgram network[NETWORK_SIZE];
    for (int networkAddress = 0; networkAddress < NETWORK_SIZE; networkAddress += 1) {
        initIntCodeProgramFromLLongArray(&network[networkAddress], &input);
        pushInput(&network[networkAddress], networkAddress);
    }

    IntCodeProgram* pc;

    // The packets (X1, Y1, X2, Y2, ...) for each pc in the network.
    LLongArray packetQueue[NETWORK_SIZE];
    for (int networkAddress = 0; networkAddress < NETWORK_SIZE; networkAddress += 1) {
        initLLongArray(&packetQueue[networkAddress], 100);
    }

    // Until the NAT pc sends the same output twice in a row to pc 0, send packets between the pcs. When the
    // network is idle (no incoming or outgoing packets), the NAT will kick start the network again with it's
    // stored packet (natX, natY).
    int sentPackets, numOutputs;
    long long prevNatX = -1, prevNatY = -1, natX, natY;
    long long outgoingNetworkAddress = 1, x = 2, y = 3;
    while (true) {
        sentPackets = 0;
        numOutputs = 0;

        // Process all the pcs in the network, sending and receiving packets.
        for (int networkAddress = 0; networkAddress < NETWORK_SIZE; networkAddress += 1) {
            pc = &network[networkAddress];

            // If there are fresh incoming packets for the network, send them all, otherwise send -1.
            if (packetQueue[networkAddress].numItems > 0) {
                for (int idx = 0; idx < packetQueue[networkAddress].numItems; idx += 2) {
                    sentPackets += 1;

                    x = packetQueue[networkAddress].data[idx];
                    y = packetQueue[networkAddress].data[idx + 1];

                    pushInput(pc, x);
                    pushInput(pc, y);
                }

                // We've processed all the packets, clear the queue.
                packetQueue[networkAddress].numItems = 0;
            } else {
                pushInput(pc, -1);
            }

            // Run the program.
            intcodeRun(pc);

            // Store the output packet in the queue.
            for (int idx = 0; idx < pc->output.numItems; idx += 3) {
                numOutputs += 1;

                outgoingNetworkAddress = pc->output.data[idx];
                x = pc->output.data[idx + 1];
                y = pc->output.data[idx + 2];

                // If the outgoing address is to the NAT pc, store the packet without worrying about
                // overriding previous packets (as per the project description).
                //
                // Otherwise, queue the packet as normal.
                if (outgoingNetworkAddress == 255) {
                    natX = x;
                    natY = y;
                } else {
                    insertLLongArray(&packetQueue[outgoingNetworkAddress], x);
                    insertLLongArray(&packetQueue[outgoingNetworkAddress], y);
                }
            }

            // We've forwarded all the packets, clear the output.
            pc->output.numItems = 0;
        }

        // If no packets were sent or received, send the NAT packet to pc 0 to kick off the network again.
        if (sentPackets == 0 && numOutputs == 0) {
            // If sending the same packet to pc 0 twice in a row, we're done.
            if (natX == prevNatX && natY == prevNatY) break;

            prevNatX = natX;
            prevNatY = natY;
            insertLLongArray(&packetQueue[0], natX);
            insertLLongArray(&packetQueue[0], natY);
        }
    }

    clock_t end = clock();
    printf("Problem 02: X=%lld, Y=%lld [%.2fms]\n", natX, natY, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
