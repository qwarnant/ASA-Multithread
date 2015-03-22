#ifndef _KLOCK_H_
#define _KLOCK_H_

/*
* The klock system is used by the kernel to share kernel object
* between several cores.
* The values of klock are hold in specific registries CORE_LOCK
* and CORE_UNLOCK.
*/

/*
* This method tries to get the klock. If it is a success,
* the method decreases the value of the registry and continues
* its processing. If not, the current process is blocked.
*/
void klock();

/**
* This method tries to release the klock. If it is a success,
* the method increases the value of the registry and continues
* its processing.
*/
void kunkock();


#endif