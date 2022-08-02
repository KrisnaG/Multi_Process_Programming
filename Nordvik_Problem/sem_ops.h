/**
 * @file sem_ops.h
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief 
 * 
 * This file is a modified version of the sem_ops.h which is the simplified 
 * semaphore operations in, 'Advanced UNIX Programming', M.J. Rochkind.
 * 
 * It has modified to return any errors produced.
 * 
 */

#include <sys/sem.h>


/**
 * @brief Returns the semaphore id (system wide) of the
 * semaphore number you give. If no semaphore has been
 * established for this number, one is created
 * 
 * @param key semaphore id
 * @return int - semaphore id on success, -1 on failure
 */
int semtran(int key) 
{
  return semget((key_t)key, 1, 0666 | IPC_CREAT);
}


/**
 * @brief Applies the operation to the semaphore
 * 
 * @param sid semaphore id
 * @param op operation to conduct
 * @return int - 0 on success, -1 on failure 
 */
static int semcall(int sid, int op)
{
  struct sembuf sb;

  sb.sem_num = 0;
  sb.sem_op = op;
  sb.sem_flg = 0;

  return semop(sid, &sb, 1);
}


/**
 * @brief The semaphore signal operation. sid must be the system
 * wide semaphore number returned by semtran above
 * 
 * @param sid semaphore id to signal
 * @return int - 0 on success, -1 on failure 
 */
int sem_wait(int sid) 
{
  return (semcall(sid, -1));
}


/**
 * @brief The semaphore release operation. sid must be the system
 * wide semaphore number returned by semtran above.
 * 
 * @param sid semaphore id to release
 * @return int - 0 on success, -1 on failure
 */
int sem_release(int sid) 
{
  return (semcall( sid, 1 ));
}


/**
 * @brief  Removes a semaphore from the system. sid must be the
 * system wide semaphore returned from semtran
 * 
 * @param sid semaphore id to remove
 * @return int 
 */
int rm_sem(int sid) 
{
  return semctl( sid, 0, IPC_RMID, 0 );
}
