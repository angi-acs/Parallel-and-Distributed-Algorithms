#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// #define NUM_THREADS (sysconf(_SC_NPROCESSORS_CONF))
#define NUM_THREADS 2

void *f(void *arg) {
  	long id = *(long*)arg;
	// for (int i = 0; i < 100; i++)
  	// 	printf("%d. Hello World din thread-ul %ld!\n", i, id);
  	printf("f - Hello World din thread-ul %ld!\n", id);
  	pthread_exit(NULL);
}

void *g(void *arg) {
	long id = *(long*)arg;
  	printf("g - Hello World din thread-ul %ld!\n", id);
  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t threads[NUM_THREADS];
  	int r, s;
  	long id;
  	void *status;
	long ids[NUM_THREADS];

  	for (id = 0; id < NUM_THREADS; id++) {
		ids[id] = id; 

		if (id == 0) {
			r = pthread_create(&threads[id], NULL, f, &ids[id]);
		} else {
			if (id == 1) {
				r = pthread_create(&threads[id], NULL, g, &ids[id]);
			}
		}

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

  	pthread_exit(NULL);
}
