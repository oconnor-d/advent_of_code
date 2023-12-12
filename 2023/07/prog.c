#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "../../utils/array.c"
#include "../../utils/string.c"


typedef struct {
    int cards[5];
    int bid;
} HandBid;

int FIVE_OF_A_KIND  = 7;
int FOUR_OF_A_KIND  = 6;
int FULL_HOUSE      = 5;
int THREE_OF_A_KIND = 4;
int TWO_PAIR        = 3;
int ONE_PAIR        = 2;
int HIGH_CARD       = 1;

int JOKER = 0;

/*
- Five of a kind, where all five cards have the same label: AAAAA
- Four of a kind, where four cards have the same label and one card has a different label: AA8AA
- Full house, where three cards have the same label, and the remaining two cards share a different label: 23332
- Three of a kind, where three cards have the same label, and the remaining two cards are each different from any other card in the hand: TTT98
- Two pair, where two cards share one label, two other cards share a second label, and the remaining card has a third label: 23432
- One pair, where two cards share one label, and the other three cards have a different label from the pair and each other: A23A4
- High card, where all cards' labels are distinct: 23456
*/
int getHandType(int cards[5]) {
    int cardCounts[14];
    for (int idx = 0; idx < 14; idx += 1) {
        cardCounts[idx] = 0;
    }

    for (int idx = 0; idx < 5; idx += 1) {
        cardCounts[cards[idx] - 1] += 1;
    }

    // FIVE_OF_A_KIND
    for (int idx = 0; idx < 14; idx += 1) {
        if (cardCounts[idx] == 5) return FIVE_OF_A_KIND;
    }

    // FOUR_OF_A_KIND
    for (int idx = 0; idx < 14; idx += 1) {
        if (cardCounts[idx] == 4) return FOUR_OF_A_KIND;
    }

    // FULL_HOUSE
    bool threeOfAKind = false, pair = false;
    for (int idx = 0; idx < 14; idx += 1) {
        if (cardCounts[idx] == 3) threeOfAKind = true;
        if (cardCounts[idx] == 2) pair = true;

        if (threeOfAKind && pair) return FULL_HOUSE;
    }

    // THREE_OF_A_KIND
    for (int idx = 0; idx < 14; idx += 1) {
        if (cardCounts[idx] == 3) return THREE_OF_A_KIND;
    }

    // TWO_PAIR
    bool pair1 = false, pair2 = false;
    for (int idx = 0; idx < 14; idx += 1) {
        if (pair1 && cardCounts[idx] == 2) pair2 = true;
        if (cardCounts[idx] == 2) pair1 = true;

        if (pair1 && pair2) return TWO_PAIR;
    }

    // ONE_PAIR
    for (int idx = 0; idx < 14; idx += 1) {
        if (cardCounts[idx] == 2) return ONE_PAIR;
    }

    return HIGH_CARD;
}


/*
Like `getHandType` above, but J is now a Joker that can transform into any card to get the highest hand type.

- Five of a kind, where all five cards have the same label: AAAAA
- Four of a kind, where four cards have the same label and one card has a different label: AA8AA
- Full house, where three cards have the same label, and the remaining two cards share a different label: 23332
- Three of a kind, where three cards have the same label, and the remaining two cards are each different from any other card in the hand: TTT98
- Two pair, where two cards share one label, two other cards share a second label, and the remaining card has a third label: 23432
- One pair, where two cards share one label, and the other three cards have a different label from the pair and each other: A23A4
- High card, where all cards' labels are distinct: 23456
*/
int getHandTypeJoker(int cards[5]) {
    int cardCounts[14];
    for (int idx = 0; idx < 14; idx += 1) {
        cardCounts[idx] = 0;
    }

    for (int idx = 0; idx < 5; idx += 1) {
        cardCounts[cards[idx] - 1] += 1;
    }

    // FIVE_OF_A_KIND
    for (int idx = 0; idx < 14; idx += 1) {
        if (cardCounts[idx] == 5) return FIVE_OF_A_KIND;

        if (idx != JOKER && cardCounts[JOKER] == 5 - cardCounts[idx]) return FIVE_OF_A_KIND;
    }

    // FOUR_OF_A_KIND
    for (int idx = 0; idx < 14; idx += 1) {
        if (cardCounts[idx] == 4) return FOUR_OF_A_KIND;

        if (idx != JOKER && cardCounts[JOKER] == 4 - cardCounts[idx]) return FOUR_OF_A_KIND;
    }

    // FULL_HOUSE
    // The only way to use a Joker to make a full house is if you already have a hand with two pairs, like:
    // K K J Q Q -> K K K Q Q
    bool threeOfAKind = false, pairOne = false, pairTwo = false;
    bool singleJoker = cardCounts[JOKER] == 1;
    for (int idx = 0; idx < 14; idx += 1) {
        if (cardCounts[idx] == 3) threeOfAKind = true;
        if (cardCounts[idx] == 2 && !pairOne) {
            pairOne = true;
        } else if (cardCounts[idx] == 2 && pairOne) {
            pairTwo = true;
        }

        if ((threeOfAKind && pairOne) || (pairOne && pairTwo && singleJoker)) return FULL_HOUSE;
    }

    // THREE_OF_A_KIND
    for (int idx = 0; idx < 14; idx += 1) {
        if (cardCounts[idx] == 3) return THREE_OF_A_KIND;

        if (idx != JOKER && cardCounts[JOKER] == 3 - cardCounts[idx]) return THREE_OF_A_KIND;
    }

    // TWO_PAIR
    // It's never worth using a Joker to make a two pair, since you could make always make a three of a kind
    // instead, which is strictly better.
    // A A K Q J -> A A K Q A instead of A A K Q K
    bool pair1 = false, pair2 = false;
    for (int idx = 0; idx < 14; idx += 1) {
        if (pair1 && cardCounts[idx] == 2) pair2 = true;
        if (cardCounts[idx] == 2) pair1 = true;

        if (pair1 && pair2) return TWO_PAIR;
    }

    // ONE_PAIR
    for (int idx = 0; idx < 14; idx += 1) {
        if (cardCounts[idx] == 2) return ONE_PAIR;

        if (idx != JOKER && cardCounts[JOKER] == 2 - cardCounts[idx]) return ONE_PAIR;
    }

    return HIGH_CARD;
}


int compareHandBids(const void * elem1, const void * elem2)  {
    HandBid handBid1 = *((HandBid*)elem1);
    HandBid handBid2 = *((HandBid*)elem2);

    int handBid1HandType = getHandType(handBid1.cards);
    int handBid2HandType = getHandType(handBid2.cards);

    if (handBid1HandType < handBid2HandType) {
        return -1;
    }

    if (handBid1HandType > handBid2HandType) {
        return 1;
    }

    for (int idx = 0; idx < 5; idx += 1) {
        if (handBid1.cards[idx] > handBid2.cards[idx]) {
            return 1;
        }
        
        if (handBid1.cards[idx] < handBid2.cards[idx]) {
            return -1;
        }
    }

    return 0;
}


int compareHandBidsJoker(const void * elem1, const void * elem2)  {
    HandBid handBid1 = *((HandBid*)elem1);
    HandBid handBid2 = *((HandBid*)elem2);

    int handBid1HandType = getHandTypeJoker(handBid1.cards);
    int handBid2HandType = getHandTypeJoker(handBid2.cards);

    if (handBid1HandType < handBid2HandType) {
        return -1;
    }

    if (handBid1HandType > handBid2HandType) {
        return 1;
    }

    for (int idx = 0; idx < 5; idx += 1) {
        if (handBid1.cards[idx] > handBid2.cards[idx]) {
            return 1;
        }
        
        if (handBid1.cards[idx] < handBid2.cards[idx]) {
            return -1;
        }
    }

    return 0;
}


void problem1() {
    clock_t start = clock();

    FILE* inputFile = fopen("2023/07/input.txt", "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLength;

    int handBidsSize = 1000, handBidsItems = 0;
    HandBid* handBids  = malloc(handBidsSize * sizeof(HandBid));

    int startIdx, endIdx, bid;
    char* hand = malloc(sizeof(char) * 6);
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        substring(line, hand, 0, 6);
        bid = parseFirstNumber(line + 6, &startIdx, &endIdx);

        HandBid handBid;
        handBid.bid = bid;
        for (int idx = 0; idx < 5; idx += 1) {
            if (hand[idx] == 'A') {
                handBid.cards[idx] = 14;
            } else if (hand[idx] == 'K') {
                handBid.cards[idx] = 13; 
            } else if (hand[idx] == 'Q') {
                handBid.cards[idx] = 12; 
            } else if (hand[idx] == 'J') {
                handBid.cards[idx] = 11; 
            } else if (hand[idx] == 'T') {
                handBid.cards[idx] = 10; 
            } else {
                handBid.cards[idx] = hand[idx] - '0';
            }
        }

        if (handBidsItems >= handBidsSize) {
            // Grow the array.
            handBidsSize *= 2;
            handBids = realloc(handBids, handBidsSize * sizeof(HandBid));
        }
        handBids[handBidsItems] = handBid;
        handBidsItems += 1;
    }

    fclose(inputFile);

    // Sort the cards from lowest to highest rank.
    //
    // NOTE: qsort is pretty slow and takes up the vast majority of the time for this.
    qsort(handBids, handBidsItems, sizeof(HandBid), compareHandBids);

    int total = 0;
    for (int idx = handBidsItems; idx > 0; idx -= 1) {
        total += handBids[idx - 1].bid * idx;
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", total, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}


void problem2() {
    clock_t start = clock();

    FILE* inputFile = fopen("2023/07/input.txt", "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLength;

    int handBidsSize = 1000, handBidsItems = 0;
    HandBid* handBids  = malloc(handBidsSize * sizeof(HandBid));

    int startIdx, endIdx, bid;
    char* hand = malloc(sizeof(char) * 6);
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        substring(line, hand, 0, 6);
        bid = parseFirstNumber(line + 6, &startIdx, &endIdx);

        HandBid handBid;
        handBid.bid = bid;
        for (int idx = 0; idx < 5; idx += 1) {
            if (hand[idx] == 'A') {
                handBid.cards[idx] = 14;
            } else if (hand[idx] == 'K') {
                handBid.cards[idx] = 13; 
            } else if (hand[idx] == 'Q') {
                handBid.cards[idx] = 12; 
            } else if (hand[idx] == 'J') {
                // J is now a Joker
                handBid.cards[idx] = 1; 
            } else if (hand[idx] == 'T') {
                handBid.cards[idx] = 10; 
            } else {
                handBid.cards[idx] = hand[idx] - '0';
            }
        }

        if (handBidsItems >= handBidsSize) {
            // Grow the array.
            handBidsSize *= 2;
            handBids = realloc(handBids, handBidsSize * sizeof(HandBid));
        }
        handBids[handBidsItems] = handBid;
        handBidsItems += 1;
    }

    fclose(inputFile);

    // Sort the cards from lowest to highest rank.
    //
    // NOTE: qsort is pretty slow and takes up the vast majority of the time for this.
    qsort(handBids, handBidsItems, sizeof(HandBid), compareHandBidsJoker);

    int total = 0;
    for (int idx = handBidsItems; idx > 0; idx -= 1) {
        total += handBids[idx - 1].bid * idx;
    }

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", total, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}


int main() {
    problem1();
    problem2();

    return 0;
}