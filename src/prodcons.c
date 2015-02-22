#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sem.h"
#include "prodcons.h"

/* Define the semaphore structures for the program */
struct sem_s mutex, empty, full;
/* Define the default sleep time of the consumer process */
int consumer_sleep_time = 0;
/* Define the default sleep time of the producer process */
int producer_sleep_time = 0;

int main(int argc, char* argv[]) {

	init_multicore();

	create_ctx(STACK_SIZE, producer, NULL);
	create_ctx(STACK_SIZE, consumer, NULL);

	/* Buffer access control */
	sem_init(&mutex, 1);
	/* At the beginning, the buffer is empty */
	sem_init(&empty, BUFFER_SIZE);
	sem_init(&full, 0);



	/* Start the context scheduler */
	printf("Start the scheduler ...\n");
	yield();
	printf("\nEND OF MAIN !");

	return EXIT_SUCCESS;
}

void producer(void *args) {

	while (1) {
		produce_object();
		sem_down(&empty);
		sem_down(&mutex);
		put_object();
		sem_up(&mutex);
		sem_up(&full);
		printf("Producer E:%d F:%d \n", empty.sem_cpt, full.sem_cpt);

	}
}
void consumer(void *args) {

	while (1) {
		sem_down(&full);
		sem_down(&mutex);
		remove_object();
		sem_up(&mutex);
		sem_up(&empty);
		use_object();
		printf("\tRetirer E:%d F:%d \n", empty.sem_cpt, full.sem_cpt);
	}
}

void produce_object() {
	sleep(producer_sleep_time);
	printf("Produce_object\n");
}

void put_object() {
	printf("Put_object\n");
}
void remove_object() {
	printf("\tRemove_object\n");
}
void use_object() {
	sleep(consumer_sleep_time);
	printf("\tUse_object\n");
}

