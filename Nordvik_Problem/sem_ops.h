/**
 * @file sem_ops.h
 * @author your name (you@domain.com)
 * @brief modified version of the sem_ops.h which is the simplified 
 * semaphore operations in, 'Advanced UNIX Programming', M.J. Rochkind
 * 
 */

#include <sys/sem.h>


/**
 * @brief returns the semaphore id (system wide) of the
 * semaphore number you give. If no semaphore has been
 * established for this number, one is created
 * 
 * @param key semaphore id
 * @return int - semaphore id on success, -1 on failure
 */
int semtran(int key) 
{
  return semget((key_t)key, 1, 0666 | IPC_CREAT);;
}


/**
 * @brief applies the operation to the semaphore
 * 
 * @param sid 
 * @param op 
 * @return int 
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
 * @brief the semaphore signal operation. sid must be the system
 * wide semaphore number returned by semtran above
 * 
 * @param sid 
 * @return int 
 */
int P_WAIT(int sid) 
{
  return (semcall(sid, -1));
}


/**
 * @brief The semaphore release operation. sid must be the system
 * wide semaphore number returned by semtran above.
 * 
 * @param sid 
 * @return int 
 */
int V_POST(int sid) 
{
  return (semcall( sid, 1 ));
}


/**
 * @brief remove a semaphore from the system. sid must be the
 * system wide semaphore returned from semtran
 * 
 * @param sid 
 */
void rm_sem(int sid) 
{
  (void)semctl( sid, 0, IPC_RMID, 0 );
}
