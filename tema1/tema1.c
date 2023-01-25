#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "tema1.h"
#include "queue.h"

long fastExponentiation(long a, long n) {
    long ans = 1;
    while (n) {
        if (n & 1) {
            ans = ans * a;
        } 
        a = a * a;
        n >>= 1;
    }
    return ans;
}

int isPower(int n, int b) {
    if (n == 1)
        return 1;

    long lowa, mida, higha, ab;
    int bits, lgn;
    int var = n;

    for(bits = 0; var != 0; bits++) 
        var >>= 1;

    lgn = bits - 1;
    lowa = 1L;
    higha = 1L << (lgn / b + 1);
    while (lowa < higha - 1) {
        mida = (lowa + higha) >> 1;
        ab = fastExponentiation(mida, b); 
        if (ab > n) {
            higha = mida;
        } else {
            if (ab < n) {
                lowa = mida;
            } else {
                return 1;
            }
        }
    }
    return 0;
}

void checkPower(int value, int mapper, int reducers, int*** partialLists, int** indexes) {
    int idx;

    for (int r = 2; r <= reducers + 1; r++) {
        if (isPower(value, r)) {
            idx = indexes[mapper][r-2];
            partialLists[mapper][r-2][idx] = value;
            idx++;
            indexes[mapper][r-2] = idx;
        }
    }
}

void *mapF(void* arg) {
    // Initialize variables for one mapper
    MapArg mapArg = *(MapArg*) arg;
    Queue* queue = mapArg.queue;
    char fileName[LINELENGTH];
    int valuesNo, value;

    // Assign dynamically every file
    pthread_mutex_lock(mapArg.mutex);
    while (!isQueueEmpty(queue)) {
        strcpy(fileName, dequeue(queue));
        FILE* in = fopen(fileName, "r");
        pthread_mutex_unlock(mapArg.mutex);

        // Parse file and process values
        fscanf(in, "%d", &valuesNo);
        for (int i = 0; i < valuesNo; i++) {
            fscanf(in, "%d", &value);
            checkPower(value, mapArg.id, mapArg.reducers, mapArg.partialLists, mapArg.indexes);
        }
        fclose(in);
    }

    pthread_barrier_wait(mapArg.barrier);
    pthread_exit(NULL);
}

void merge(int reducer, int mappers, int*** partialLists, int** indexes, 
            int** mergedLists, int* mergedIndexes) {
    int j = 0;
    for (int m = 0; m < mappers; m++) {
        for (int i = 0; i < indexes[m][reducer]; i++) {
            mergedLists[reducer][j] = partialLists[m][reducer][i];
            j++;
        }
    }
    mergedIndexes[reducer] = j;
}

void *redF(void* arg) {
    RedArg redArg = *(RedArg*) arg;
    pthread_barrier_wait(redArg.barrier);

    int r = redArg.id;
    char outString[LINELENGTH];
    
    // Merge partial lists for the power the current reducer is responsible for
    merge(r, redArg.mappers, redArg.partialLists, redArg.indexes, 
            redArg.mergedLists, redArg.mergedIndexes);

    // Count unique elements in merged list
    int j;
    int count = 1;

    for (int i = 1; i < redArg.mergedIndexes[r]; i++) {
        j = 0;
        for (j = 0; j < i; j++) {
            if (redArg.mergedLists[r][i] == redArg.mergedLists[r][j]) {
                break;
            }
        }
        if (i == j) {
            count++;
        }
    }

    // Write result to file
    pthread_mutex_lock(redArg.mutex);
    snprintf(outString, LINELENGTH, "out%d.txt", r+2);
    FILE* out = fopen(outString, "w");
    pthread_mutex_unlock(redArg.mutex);
    fprintf(out, "%d", count);
    fclose(out);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    // Get arguments
    int mappers = atoi(argv[1]);
    int reducers = atoi(argv[2]);
    FILE* in = freopen(argv[3], "r", stdin);

    // Initialize variables
    pthread_t threads[mappers + reducers];
    MapArg mapArgs[mappers];
    RedArg redArgs[reducers];
    int i, j, r, filesNo;
    scanf("%d", &filesNo);
    File files[filesNo];
    Queue* queue = createQueue();
    
    // Initialize elements of synchronization
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, mappers + reducers);

    // Calloc arrays
    int ***partialLists = (int***) calloc(mappers, sizeof(int**));
    for (i = 0; i < mappers; i++) {
        partialLists[i] = (int**) calloc(reducers, sizeof(int*));
        for (j = 0; j < reducers; j++) {
            partialLists[i][j] = (int*) calloc(LIMIT, sizeof(int));
        }
    }

    int** indexes = (int**) calloc(mappers, sizeof(int*));
    for (i = 0; i < mappers; i++) {
        indexes[i] = (int*) calloc(reducers, sizeof(int));
    }

    int** mergedLists = (int**) calloc(reducers, sizeof(int*));
    for (i = 0; i < reducers; i++) {
        mergedLists[i] = (int*) calloc(LIMIT, sizeof(int));
    }

    int* mergedIndexes = (int*) calloc(reducers, sizeof(int));

    // Get files and sort them in descending order by size
    for (i = 0; i < filesNo; i++) {
        scanf("%s", files[i].name);
        files[i].size = getFileSize(files[i].name);
    }
    qsort(files, filesNo, sizeof(File), cmpF);

    // After sorting all files go into a queue
    for (i = 0; i < filesNo; i++) {
        enqueue(queue, files[i].name);
    }

    // Create threads
    for (i = 0; i < mappers; i++) {
        mapArgs[i] = initMapArg(i, mappers, reducers, queue, partialLists, indexes,
                                &mutex, &barrier);

        r = pthread_create(&threads[i], NULL, mapF, &mapArgs[i]);
        if (r) {
            printf("Error creating mapper %d\n", i);
            exit(-1);
        }
    }

    for (i = 0; i < reducers; i++) {
        redArgs[i] = initRedArg(i, mappers, partialLists, indexes, mergedLists, mergedIndexes,
                                &mutex, &barrier);

        r = pthread_create(&threads[i + mappers], NULL, redF, &redArgs[i]);
        if (r) {
            printf("Error creating reducer %d\n", i);
            exit(-1);
        }
    }

    // Join all threads at once
    for (i = 0; i < mappers + reducers; i++) {
        r = pthread_join(threads[i], NULL);
        if (r) {
            printf("Error waiting thread %d\n", i);
            exit(-1);
        }
  	}

    // Exit program
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);
    pthread_exit(NULL);
    fclose(in);
    return 0;
}