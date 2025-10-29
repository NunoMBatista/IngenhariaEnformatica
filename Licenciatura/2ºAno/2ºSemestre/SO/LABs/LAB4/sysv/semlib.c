
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <assert.h>
#include <stdio.h>
#include "semlib.h"

/*****************/


#if (defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)) || defined(__APPLE__) || defined(__OpenBSD__)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
  int val;                    /* value for SETVAL */
  struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
  unsigned short int *array;  /* array for GETALL, SETALL */
  struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif

/*****************/


int sem_get(int nsem, int init_val)
{
  int id;
  int i;

  if ( (id=semget(IPC_PRIVATE, nsem, IPC_CREAT|0777)) == -1 )
  {
    perror("Could not get the semafore set!");
    return -1;
  }

  for (i=0; i<nsem; i++)
    sem_setvalue(id, i, init_val);

  return id;
}

void sem_close(int sem_id)
{

  semctl(sem_id, 0, IPC_RMID, 0);
}

void sem_wait(int sem_id, int sem_num)
{
  struct sembuf op;

  op.sem_num = sem_num;
  op.sem_op  = -1;
  op.sem_flg = 0;

  if ( semop(sem_id, &op, 1) == -1 )
  {
    perror("Could not do the wait on the semafore");
  }
}

void sem_post(int sem_id, int sem_num)
{
  struct sembuf op;

  op.sem_num = sem_num;
  op.sem_op  = +1;
  op.sem_flg = 0;

  if ( semop(sem_id, &op, 1) == -1)
  {
      perror("Could not do the signal on the semafore");
  }
}

void sem_setvalue(int sem_id, int sem_num, int value)
{
  union semun val;
  val.val = value;

  if ( semctl(sem_id, sem_num, SETVAL, val) == -1 )
  {
      perror("Could not set the value on the semafore");
  }
}
