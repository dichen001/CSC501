#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
	STATWORD ps; 
	disable(ps);
	// no need to consider whether it's private or not. It must be a NEW UNMAPPED one.
	if(npages<=0 || npages>NPGPBS || bs_id<0 || bs_id>15 )
	{
		if(GDB)
			kprintf("ERROR: npages=%d, bs_id=%d, bs.status=%d, bs.private=%d, bs.pid=%d, proc.bs.private=%d, proc.bs.status=%d\n",
				npages, bs_id, bsm_tab[bs_id].bs_status, bsm_tab[bs_id].private, bsm_tab[bs_id].bs_pid, proctab[currpid].bsmap[bs_id].private, proctab[currpid].bsmap[bs_id].bs_status);
		return SYSERR;
	}
	else if(bsm_tab[bs_id].bs_status == BSM_MAPPED)
	{
		return bsm_tab[bs_id].bs_npages;
	}
	if(bsm_tab[bs_id].bs_status == BSM_UNMAPPED){
		
		// no need to call map here, just assign the bs to it, and update related bs.
		// bsm_map(currpid, 0, bs_id, npages);
		curr_bs_num += 1;
		bsm_tab[bs_id].bs_status = BSM_MAPPED;
		bsm_tab[bs_id].bs_pid = currpid;
		bsm_tab[bs_id].bs_vpno = 0;
		bsm_tab[bs_id].bs_npages = npages;

		if(GDB)
			kprintf("Backing_Store[%d] got allocated to proc[%d], total bs in use is %d\n",bs_id, currpid, curr_bs_num);
		if(GDB)
			kprintf("get_bs(bs_id(%d), npages(%d)) completed\n",bs_id,npages);
		if(GDB)
			kprintf("REMEMBER the mapping between vp and bs hasn't been built up. Need xmmap()\n");
		restore(ps);
		return( npages );
	}

}


