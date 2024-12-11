#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

float TABLE_MAX_LOAD = .75;

/**
 * Implementation heavily inspired by / copied from the Crafting Interpreters book.
 */

typedef struct {
    char* key;
    char* value;
} KeyValuePair;

typedef struct {
    KeyValuePair* entries;

    int numKeys;
    int capacity;
} Map;

/**
 * Returns the has of the given key using the FNV-1a hashing algorithm.
 *
 * `length` is the length of the key being hashed.
 *
 * Code taken from: https://craftinginterpreters.com/hash-tables.html#hashing-strings
 */
static uint32_t hashString(const char* key, int length) {
    uint32_t hash = 216613626lu;

    for (int idx = 0; idx < length; idx += 1) {
        hash ^= (uint8_t)key[idx];
        hash *= 16777619;
    }

    return hash;
}

void initMap(Map* map) {
    map->entries = NULL;

    map->numKeys = 0;
    map->capacity = 0;
}

void freeMap(Map* map) {
    free(map->entries);
    initMap(map);
}

static KeyValuePair* findEntry(KeyValuePair* entries, int capacity, char* key) {
    uint32_t index = hashString(key, strlen(key)) % capacity;

    while (true) {
        KeyValuePair* entry = &entries[index];
        // NOTE: strcmp is probably the bottleneck of this whole implementation, it's pretty slow.
        if (entry->key == NULL || strcmp(entry->key, key) == 0) return entry;

        index = (index + 1) % capacity;
    }
}

static void growMap(Map* map) {
    int newCapacity = map->capacity < 8 ? 8 : map->capacity * 2;

    KeyValuePair* newEntries = malloc(newCapacity * sizeof(KeyValuePair));

    for (int idx = 0; idx < newCapacity; idx += 1) {
        newEntries[idx].key = NULL;
        newEntries[idx].value = NULL;
    }

    // Copy over the old values
    for (int idx = 0; idx < map->capacity; idx += 1) {
        KeyValuePair* source = &map->entries[idx];
        if (source->key == NULL) continue;

        KeyValuePair* dest = findEntry(newEntries, newCapacity, source->key);
        dest->key = source->key;
        dest->value = source->value;
    }

    free(map->entries);

    map->entries = newEntries;
    map->capacity = newCapacity;
}

bool setMap(Map* map, char* key, char* value) {
    // Grow the map if need be.
    if (map->numKeys + 1 > map->capacity * TABLE_MAX_LOAD) {
        growMap(map);
    }

    KeyValuePair* entry = findEntry(map->entries, map->capacity, key);
    bool isNewKey = entry->key == NULL;
    if (isNewKey) map->numKeys += 1;

    entry->key = key;
    entry->value = value;

    return isNewKey;
}

bool getMap(Map* map, char* key, char** value) {
    if (map->numKeys == 0) return false;

    KeyValuePair* entry = findEntry(map->entries, map->capacity, key);
    if (entry->key == NULL) return false;

    *value = entry->value;
    return true;
}


/*
LLongMap
*/

typedef struct {
    char* key;
    long long value;
} LLongKeyValuePair;

typedef struct {
    LLongKeyValuePair* entries;

    int numKeys;
    int capacity;
} LLongMap;

void initLLongMap(LLongMap* map) {
    map->entries = NULL;

    map->numKeys = 0;
    map->capacity = 0;
}

void freeLLongMap(LLongMap* map) {
    free(map->entries);
    initLLongMap(map);
}

static LLongKeyValuePair* findLLongEntry(LLongKeyValuePair* entries, int capacity, char* key) {
    uint32_t index = hashString(key, strlen(key)) % capacity;

    while (true) {
        LLongKeyValuePair* entry = &entries[index];
        // NOTE: strcmp is probably the bottleneck of this whole implementation, it's pretty slow.
        if (entry->key == NULL || strcmp(entry->key, key) == 0) return entry;

        index = (index + 1) % capacity;
    }
}

static void growLLongMap(LLongMap* map) {
    int newCapacity = map->capacity < 8 ? 8 : map->capacity * 2;

    LLongKeyValuePair* newEntries = malloc(newCapacity * sizeof(LLongKeyValuePair));

    for (int idx = 0; idx < newCapacity; idx += 1) {
        newEntries[idx].key = NULL;
    }

    // Copy over the old values
    for (int idx = 0; idx < map->capacity; idx += 1) {
        LLongKeyValuePair* source = &map->entries[idx];
        if (source->key == NULL) continue;

        LLongKeyValuePair* dest = findLLongEntry(newEntries, newCapacity, source->key);
        dest->key = source->key;
        dest->value = source->value;
    }

    free(map->entries);

    map->entries = newEntries;
    map->capacity = newCapacity;
}


bool setLLongMap(LLongMap* map, char* key, long long value) {
    // Grow the map if need be.
    if (map->numKeys + 1 > map->capacity * TABLE_MAX_LOAD) {
        growLLongMap(map);
    }

    LLongKeyValuePair* entry = findLLongEntry(map->entries, map->capacity, key);
    bool isNewKey = entry->key == NULL;
    if (isNewKey) map->numKeys += 1;

    entry->key = key;
    entry->value = value;

    return isNewKey;
}

bool getLLongMap(LLongMap* map, char* key, long long* value) {
    if (map->numKeys == 0) return false;

    LLongKeyValuePair* entry = findLLongEntry(map->entries, map->capacity, key);
    if (entry->key == NULL) return false;

    *value = entry->value;
    return true;
}