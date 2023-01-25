#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
    int numtasks, rank, flag;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int recv_num = 0;

    // First process starts the circle.
    if (rank == 0) {
        // First process starts the circle.
        // Generate a random number.
        // Send the number to the next process.
        MPI_Status status;
        MPI_Request request;
        sleep(1);

        MPI_Isend(&recv_num, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (!flag) {
            MPI_Wait(&request, &status);            
        }

        sleep(1);

        MPI_Irecv(&recv_num, 1, MPI_INT, numtasks - 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (!flag) {
            MPI_Wait(&request, &status);            
        }
        printf("%d -> %d : %d \n", numtasks - 1, rank, recv_num);

    } else if (rank == numtasks - 1) {
        // Last process close the circle.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the first process.
        MPI_Status status;
        MPI_Request request;
        sleep(1);

        MPI_Irecv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (!flag) {
            MPI_Wait(&request, &status);            
        }
        printf("%d -> %d : %d \n", rank - 1, rank, recv_num);
        recv_num += 2;

        sleep(1);

        MPI_Isend(&recv_num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (!flag) {
            MPI_Wait(&request, &status);            
        }

    } else {
        // Middle process.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the next process.
        MPI_Status status;
        MPI_Request request;
        sleep(1);

        MPI_Irecv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (!flag) {
            MPI_Wait(&request, &status);            
        }
        printf("%d -> %d : %d \n", rank - 1, rank, recv_num);
        recv_num += 2;

        sleep(1);

        MPI_Isend(&recv_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (!flag) {
            MPI_Wait(&request, &status);            
        }
    }

    MPI_Finalize();

}

