#ifndef TEMA1_H_
#define TEMA1_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef char* Item;
#include "queue.h"

#define LINELENGTH 20
#define LIMIT 500

typedef struct File {
    char name[LINELENGTH];
    long int size;
} File;

typedef struct MapArg {
    int id;
    int mappers;
    int reducers;
    Queue* queue;
    int*** partialLists;
    int** indexes;
    pthread_mutex_t* mutex;
    pthread_barrier_t* barrier;
} MapArg;

typedef struct RedArg {
    int id;
    int mappers;
    int*** partialLists;
    int** indexes;
    int** mergedLists;
    int* mergedIndexes;
    pthread_mutex_t* mutex;
    pthread_barrier_t* barrier;
} RedArg;

MapArg initMapArg(int i, int mappers, int reducers, Queue* queue, int*** partialLists, int** indexes,
                    pthread_mutex_t* mutex, pthread_barrier_t* barrier) {
    MapArg mapArg;
    mapArg.id = i;
    mapArg.mappers = mappers;
    mapArg.reducers = reducers;
    mapArg.queue = queue;
    mapArg.partialLists = partialLists;
    mapArg.indexes = indexes;
    mapArg.mutex = mutex;
    mapArg.barrier = barrier;
    return mapArg;
}

RedArg initRedArg(int i, int mappers, int*** partialLists, int** indexes, int** mergedLists, 
                    int* mergedIndexes, pthread_mutex_t* mutex, pthread_barrier_t* barrier) {
    RedArg redArg;
    redArg.id = i;
    redArg.mappers = mappers;
    redArg.partialLists = partialLists;
    redArg.indexes = indexes;
    redArg.mergedLists = mergedLists;
    redArg.mergedIndexes = mergedIndexes;
    redArg.mutex = mutex;
    redArg.barrier = barrier;
    return redArg;                    
}

long int getFileSize(const char fileName[]) {
    FILE* in = fopen(fileName, "r");

    fseek(in, 0L, SEEK_END);
    long int res = ftell(in);

    fclose(in);
    return res;
}

// Function for qsort
int cmpF(const void* a, const void* b) {
    File file1 = *(File*) a;
    File file2 = *(File*) b;

    return file2.size - file1.size;
}

#endif