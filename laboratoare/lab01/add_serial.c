#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
    schelet pentru exercitiul 5
*/

int* arr;
int array_size;
int NUM_THREADS;
#define min(a,b) (((a) < (b)) ? (a) : (b))

void *f(void *arg) {
  	long ID = *(long*)arg;
    int N = array_size;
    int P = NUM_THREADS;
	  int start = ID * (double)N / P;
    int end = min((ID + 1) * (double)N / P, N);
    for (int i = start; i < end; i++) {
        arr[i] += 100;
    }

  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Specificati dimensiunea array-ului\n");
        exit(-1);
    }

    array_size = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);

    arr = malloc(array_size * sizeof(int));
    for (int i = 0; i < array_size; i++) {
        arr[i] = i;
    }

    // for (int i = 0; i < array_size; i++) {
    //     printf("%d", arr[i]);
    //     if (i != array_size - 1) {
    //         printf(" ");
    //     } else {
    //         printf("\n");
    //     }
    // }

    // TODO: aceasta operatie va fi paralelizata
  	// for (int i = 0; i < array_size; i++) {
    //     arr[i] += 100;
    // }

    pthread_t threads[NUM_THREADS];
  	int r, s;
  	long id;
  	void *status;
	long ids[NUM_THREADS];

  	for (id = 0; id < NUM_THREADS; id++) {
		ids[id] = id; 
		r = pthread_create(&threads[id], NULL, f, &ids[id]);
		
		if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	for (id = 0; id < NUM_THREADS; id++) {
		r = pthread_join(threads[id], &status);

		if (r) {
	  		printf("Eroare la asteptarea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}


    // for (int i = 0; i < array_size; i++) {
    //     printf("%d", arr[i]);
    //     if (i != array_size - 1) {
    //         printf(" ");
    //     } else {
    //         printf("\n");
    //     }
    // }

  	pthread_exit(NULL);
}
