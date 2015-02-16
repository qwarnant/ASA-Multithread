#ifndef _PC_H_
#define _PC_H_

/* The size of the object buffer */
#define BUFFER_SIZE 5
/* The size of the context stack */
#define STACK_SIZE 16384

/*
	producer()
	This method performs the following tasks :
	- Produce a new object
	- Decrease the number of the free places in the buffer
	- Get the lock on the critical section
	- Put the new object in the buffer
	- Release the lock on the critical section
	- And finally increase the number of the busy place in the buffer
 */
void producer(void *);

/*
    consumer()
    This method performs the following tasks :
    - Decrease the number of the busy places in the buffer
    - Get the lock on the critical section
    - Remove an object from the buffer
    - Release the lock on the critical section
    - Increase the number of free places in the buffer
    - Use the previously acquired object
 */
void consumer(void *);

/*
    produce_object()
    This method is sleeping a moment which is introduce by the user
    of the program
 */
void produce_object();
/*
    remove_object()
    This method is sleeping a moment which is introduce by the user
    of the program
 */
void remove_object();

void put_object();
void use_object();

#endif
