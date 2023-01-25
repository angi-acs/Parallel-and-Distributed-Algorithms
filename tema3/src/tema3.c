#include<mpi.h>
#include<stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define coords 4
#define min(a,b) (((a) < (b)) ? (a) : (b))

int workersNo;
int *workers;

void readClusters(int rank) {
    FILE *fp;
    char fileName[15];
    sprintf(fileName, "cluster%d.txt", rank);

    fp = fopen(fileName, "r");
	fscanf(fp, "%d", &workersNo);

	workers = malloc(sizeof(int) * workersNo);

	for (int i = 0; i < workersNo; i++)
		fscanf(fp, "%d", &workers[i]);
}

void updateV(int* v, int* vRecv, int processesNo) {
    for (int i = 0; i < processesNo; i++) {
        if (v[i] != vRecv[i] && vRecv[i] != -1) {
            v[i] = vRecv[i];
        }
    }
}

int* getTopology(int rank, int processesNo) {
    MPI_Status status;

    // The workers find out who their coordonator is
    int coordonator;
    if (rank < coords) {
        for (int i = 0; i < workersNo; i++) {
            MPI_Send(&rank, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, workers[i]);
        }
    } else {
        MPI_Recv(&coordonator, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    }

    // Topology arrays
    int *v = malloc(sizeof(int) * processesNo);
	int *vRecv = malloc(sizeof(int) * processesNo);

    memset(v, -1, sizeof(int) * processesNo);
	memset(vRecv, -1, sizeof(int) * processesNo);

    if (rank < coords) {
        for (int i = 0; i < workersNo; i++) {
            v[workers[i]] = rank;
        }
        v[rank] = workersNo;

        // First process starts the circle
        if (rank == 0) {
            MPI_Send(v, processesNo, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);

            MPI_Recv(vRecv, processesNo, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
            updateV(v, vRecv, processesNo);

        } else if (rank == 3) {
            MPI_Recv(vRecv, processesNo, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            updateV(v, vRecv, processesNo);
            MPI_Send(v, processesNo, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);

            MPI_Recv(vRecv, processesNo, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
            updateV(v, vRecv, processesNo);
            MPI_Send(v, processesNo, MPI_INT, 0, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 0);

        } else if (rank == 2) {
            MPI_Recv(vRecv, processesNo, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
            updateV(v, vRecv, processesNo);
            MPI_Send(v, processesNo, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);

            MPI_Recv(vRecv, processesNo, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
            updateV(v, vRecv, processesNo);
            MPI_Send(v, processesNo, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);

        } else if (rank == 1) {
            MPI_Recv(vRecv, processesNo, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
            updateV(v, vRecv, processesNo);

            MPI_Send(v, processesNo, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
        }

        // Send the topology to their respective workers
        for (int i = 0; i < workersNo; i++) {
            MPI_Send(v, processesNo, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, workers[i]);
        }
    }

    // Workers receive the topology
    if (rank >= coords) {
        MPI_Recv(v, processesNo, MPI_INT, coordonator, 0, MPI_COMM_WORLD, &status);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Log topolgy in terminal
    printf("%d ->", rank);
    for (int i = 0; i < coords; i++) {
        int idx = 1;
        printf(" %d:", i);
        for (int j = coords; j < processesNo; j++) {
            if (v[j] == i) {
                if (v[i] == idx) {
                    printf("%d", j);
                } else {
                    printf("%d,", j);
                    idx++;
                }
            }
        }
    }
    printf("\n");

    return v;
}

void updateA(int* a, int* aRecv, int worker, int N, int processesNo) {
    int id = worker - coords;
    int P = processesNo - coords;
    int start = id * N / P;
    int end = min((id + 1) * N / P, N);

    for (int i = start; i < end; i++) {
        a[i] = aRecv[i];
    }
}

void computeArray(int* v, int rank, int processesNo, char *argv[]) {
    MPI_Status status;
    int N;

    // Send N to the coordonators
    if (rank == 0) {
        N = atoi(argv[1]);
        
        MPI_Send(&N, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 3);
    } else if (rank == 3) {
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Send(&N, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 2);
    } else if (rank == 2) {
        MPI_Recv(&N, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
        MPI_Send(&N, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 1);
    } else if (rank == 1) {
        MPI_Recv(&N, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
    }

    // Send N to workers
    if (rank < coords) {
        for (int i = 0; i < workersNo; i++) {
            MPI_Send(&N, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, workers[i]);
        }
    } else {
        MPI_Recv(&N, 1, MPI_INT, v[rank], 0, MPI_COMM_WORLD, &status);
    }

    // Send the array to the coordonators
    int *a = malloc(sizeof(int) * N);
	int *aRecv = malloc(sizeof(int) * N);

    memset(a, -1, sizeof(int) * N);
	memset(aRecv, -1, sizeof(int) * N);

    if (rank == 0) {
        for (int k = 0; k < N; k++) {
            a[k] = N - k - 1;
        }
        MPI_Send(a, N, MPI_INT, 3, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 3);

    } else if (rank == 3) {
        MPI_Recv(a, N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Send(a, N, MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 2);

    } else if (rank == 2) {
        MPI_Recv(a, N, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
        MPI_Send(a, N, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 1);

    } else if (rank == 1) {
        MPI_Recv(a, N, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
    }

    // Send the array to workers
    if (rank < coords) {
        for (int i = 0; i < workersNo; i++) {
            MPI_Send(a, N, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, workers[i]);
        }
    } else {
        MPI_Recv(a, N, MPI_INT, v[rank], 0, MPI_COMM_WORLD, &status);
    }

    // Workers do their part and send the array back to their coordonator
    if (rank >= coords) {
        int id = rank - coords;
        int P = processesNo - coords;
        int start = id * N / P;
        int end = min((id + 1) * N / P, N);

        for (int i = start; i < end; i++) {
            a[i] *= 5;
        }

        MPI_Send(a, N, MPI_INT, v[rank], 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, v[rank]);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // The coordonators receive the array from their respective workers
    if (rank < coords) {
        for (int i = 0; i < workersNo; i++) {
            MPI_Recv(aRecv, N, MPI_INT, workers[i], 0, MPI_COMM_WORLD, &status);
            updateA(a, aRecv, workers[i], N, processesNo);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Every coordonator updates the array until it reaches 0
    if (rank == 1) {
        MPI_Send(a, N, MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 2);
    } else if (rank == 2) {
        MPI_Recv(aRecv, N, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        for (int i = coords; i < processesNo; i++) {
            if (v[i] == 1) {
                updateA(a, aRecv, i, N, processesNo);
            }
        }
        MPI_Send(a, N, MPI_INT, 3, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 3);
    } else if (rank == 3) {
        MPI_Recv(aRecv, N, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
        for (int i = coords; i < processesNo; i++) {
            if (v[i] == 1 || v[i] == 2) {
                updateA(a, aRecv, i, N, processesNo);
            }
        }
        MPI_Send(a, N, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 0);
    } else if (rank == 0) {
        MPI_Recv(aRecv, N, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
        for (int i = coords; i < processesNo; i++) {
            if (v[i] == 1 || v[i] == 2 || v[i] == 3) {
                updateA(a, aRecv, i, N, processesNo);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Coordonator 0 logs the result
    if (rank == 0) {
        printf("Rezultat:");
        for (int i = 0; i < N; i++) {
            printf(" %d", a[i]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int processesNo, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processesNo);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank < coords) {
        readClusters(rank);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    int* v;
    v = getTopology(rank, processesNo);

    MPI_Barrier(MPI_COMM_WORLD);

    computeArray(v, rank, processesNo, argv);

    MPI_Finalize();

    return 0;
}