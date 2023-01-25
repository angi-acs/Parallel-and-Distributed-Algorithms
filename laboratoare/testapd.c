#include<mpi.h>
#include<stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int processesNo, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processesNo);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Status status;

    int R = rank * 2;
    int rRecv;

    for (int i = 0; i < processesNo; i++) {
        if (i % 2 == 0) {
            if (rank % 2 == 0) {
                if (rank != processesNo - 1) {
                    MPI_Sendrecv(&R, 1, MPI_INT, rank+1, 0,
                                &rRecv, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD, &status);
                    if (R < rRecv) {
                        R = rRecv;
                    }            
                }
            } else {
                if (rank != 0) {
                    MPI_Sendrecv(&R, 1, MPI_INT, rank-1, 0,
                                &rRecv, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, &status);
                    if (R > rRecv) {
                        R = rRecv;
                    }  
                }
            }
        } else {
             if (rank % 2 == 1) {
                if (rank != processesNo - 1) {
                    MPI_Sendrecv(&R, 1, MPI_INT, rank+1, 0,
                            &rRecv, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD, &status);
                    if (R < rRecv) {
                        R = rRecv;
                    }  
                }
            } else {
                if (rank != 0) {
                    MPI_Sendrecv(&R, 1, MPI_INT, rank-1, 0,
                        &rRecv, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, &status);
                    if (R > rRecv) {
                        R = rRecv;
                    }  
                }
            }
        }
    }

    printf("%d - %d\n", rank, R);

    MPI_Barrier(MPI_COMM_WORLD);

    int *v = malloc(sizeof(int) * processesNo);
    memset(v, -1, sizeof(int) * processesNo);

    v[rank] = R;

    if (rank != processesNo - 1) {
        MPI_Send(v, processesNo, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }

    if (rank != 0) {
        MPI_Recv(v, processesNo, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == processesNo - 1) {
        for (int i = 0; i < processesNo; i++) {
            printf("%d ", v[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
}