/* sem.h - isbadsem */

#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef	NLOCKS
#define	NLOCKS		50	/* number of semaphores, if not defined	*/
#endif

#define	L_FREE 		0		/* this lock is free		*/
#define	L_USED 		1		/* this lock is used		*/

/* No Need To Define these */
#define	READ 		0		/* this lock is read		*/
#define	WRITE		1		/* this lock is write		*/
#define	DELETED 	2		/* this lock is deleted		*/

struct	lentry	{	/* lock table entry		*/
	int	lstatus;	/* SFREE or SUSED		*/
	int lstate;		/* READ WRITE or DELETED */
	int lprio;		/* the priority of a lock */
	int	ltime;		/* the time begin waiting for the lock		*/
	int head;		/* q index of head of list		*/
	int next;
};


extern	struct	lentry	locktab[];
extern	int	nextlock;
extern  int GDB;  // Global Debugger.
extern void deque(int pid, int ldes);  

#define	isbadsem(s)	(s<0 || s>=NSEM)

#endif
