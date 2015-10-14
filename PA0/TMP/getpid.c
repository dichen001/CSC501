/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
extern int flag;
extern int freq[6][NPROC];

SYSCALL getpid(){
	if(flag){
		freq[0][currpid]++;
        	kprintf("########################################\n");
		kprintf("System Call(0) 'getpid' being called by \"%s\" (pid=%d) counter=%d\n", proctab[currpid].pname, currpid,freq[0][currpid]);
        	kprintf("########################################\n");
	}
	return(currpid);
}
