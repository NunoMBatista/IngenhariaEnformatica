
/*
 * SEMLIB.H
 *
 * Interface definition of a simple semaphore library for using
 * in inherited programs (parent-child relationship).
 */

#ifndef _SEMLIB_H_
#define _SEMLIB_H_

/* Obtains a new array of semaphores from the system. The number
 * of semaphores in the array is <nsem>, and they all have the initial
 * value of <init_val>. The function returns the semaphore id (semid)
 * of the set, or -1 on error. The semaphores in the set range from 0
 * to nsem-1.
 */
extern int sem_get(int nsem, int init_val);

/* Releases a semaphore set that is no longer in use. <sem_id> identifies
 * the semaphore set.
 */
extern void sem_close(int sem_id);

/* Performs a wait on a certain semaphore in a semaphore set. The semaphore
 * set is <sem_id>, and <sem_num> represents the particular semaphore in
 * question.
 */
extern void sem_wait(int sem_id, int sem_num);

/* Performs a signal on a certain semaphore in a semaphore set. The semaphore
 * set is <sem_id>, and <sem_num> represents the particular semaphore in
 * question.
 */
extern void sem_post(int sem_id, int sem_num);

/*
 * Directly sets the value of a semaphore in a semaphore set to a
 * certain value. The semaphore set is identified by <sem_id>, and the
 * semaphore is <sem_num>. <value> represents the new value of the semaphore.
 * This routine should only be used for initializing semaphores.
 */
extern void sem_setvalue(int sem_id, int sem_num, int value);


#endif

