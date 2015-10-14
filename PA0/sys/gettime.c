/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>
extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
    /* long	now; */

	/* FIXME -- no getutim */
    extern int flag;
    extern int freq[6][NPROC];
    if(flag){
    	freq[1][currpid]++;
	kprintf("########################################\n");
    	kprintf("System Call(1) 'gettime' being called by \"%s\" (pid=%d)\n", proctab[currpid].pname, currpid);
    	kprintf("########################################\n");
    }
    return OK;
}
