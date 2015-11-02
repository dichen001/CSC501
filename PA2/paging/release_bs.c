#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {
	STATWORD ps;
    disable(ps);
  /* release the backing store with ID bs_id */
	if(bsm_tab[bs_id].bs_pid = currpid && bsm_tab[bs_id].mapping_num == 1){
		free_bsm(bs_id);
		restore(ps);
   		return OK;	
	}
	else{
		kprintf("only the bs owener [%s] call release me-->bs[%d]\n",proctab[ bsm_tab[bs_id].bs_pid ].pname,bs_id);
		restore(ps);
		return SYSERR;
	} 


}

