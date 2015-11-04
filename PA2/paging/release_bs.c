#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {
	STATWORD ps;
    disable(ps);
  /* release the backing store with ID bs_id */
	if(bsm_tab[bs_id].mapping_num == 0){
		free_bsm(bs_id);
		restore(ps);
   		return OK;	
	}
	else{
		kprintf("Cannot release, bs[%d].mapping_num= %d\n",bs_id, bsm_tab[bs_id].mapping_num);
		restore(ps);
		return SYSERR;
	} 


}

