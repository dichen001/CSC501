/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>


fr_map_t frm_tab[NFRAMES];
/*-------------------------------------------------------------------------
 * init_frm_tab - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm_tab()
{
	kprintf("initialize frame \n");
	int i;
	for (i = 0; i < NFRAMES; ++i)
	{
		frm_tab[i].bs_next = NULL;
		frm_tab[i].fr_upper_t = -1;
		frm_tab[i].fr_id = i;

		frm_tab[i].fr_status = FRM_UNMAPPED;
		frm_tab[i].fr_pid = -1;
		frm_tab[i].fr_vpno = -1;
		frm_tab[i].fr_refcnt = 0;
		frm_tab[i].fr_type = -1;
		frm_tab[i].fr_dirty = 0;
		frm_tab[i].cookie = NULL;
		frm_tab[i].fr_loadtime = 0;
	}
	kprintf("frame initialized \n");
	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
int get_frm()
{	int i;
	for (i = 0; i < NFRAMES; ++i)
	{
		if (frm_tab[i].fr_status == FRM_UNMAPPED)
		{
			if(GDB)
				kprintf("Frame[%d] got!\n",i);
			return i;
		}
	}
	if(GDB)
		kprintf("!!!!!!!!!No Frame left!\n");
	return -1;
}


/*-------------------------------------------------------------------------
 * init_frm - initialize a frame after get_frm
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm(int i, int pid, int type){
	STATWORD ps;
	// Disable interrupts
    disable(ps);
	frm_tab[i].bs_next = NULL;
	frm_tab[i].fr_upper_t = -1;

	frm_tab[i].fr_status = FRM_MAPPED;	// ** will here be wrong? ** //
	frm_tab[i].fr_pid = pid;
	frm_tab[i].fr_refcnt = 1;
	frm_tab[i].fr_type = type;
	frm_tab[i].fr_dirty = 0; 
	if(GDB)
		kprintf("frame[%d].pid=%d .vpno=%d .refcnt=%d\n",i,pid,frid2vpno(i),frm_tab[i].fr_refcnt);
	if(GDB)
		kprintf("frame[%d] initialized\n",i);
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * reset_frm - reset a frame when free them
 *-------------------------------------------------------------------------
 */
SYSCALL reset_frm(int i){
	STATWORD ps;
	// Disable interrupts
    disable(ps);
	//frm_tab[i].bs_next = NULL;
	//frm_tab[i].fr_upper_t = -1;

	frm_tab[i].fr_status = FRM_UNMAPPED;	// ** will here be wrong? ** //
	//frm_tab[i].fr_pid = -1;
	//frm_tab[i].fr_vpno = -1;
	frm_tab[i].fr_refcnt = 0;
	//frm_tab[i].fr_type = -1;
	frm_tab[i].fr_dirty = 0; 
	//frm_tab[i].fr_cookie
	frm_tab[i].fr_loadtime = -1;
	if(GDB)
		kprintf("[%d] reset\n",i);
	restore(ps);
	return OK;
}


/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int id)
{	
	pd_t *pd;
	pt_t *pt, *p;
	int i, j, upper_id, store, pageth, dirty, fr_t_id;
	pd = proctab[currpid].pdbr;
	STATWORD ps;
	// Disable interrupts
    disable(ps);
	if(frm_tab[id].fr_type == FR_PAGE)
	{
		upper_id = frm_tab[id].fr_upper_t;
		p = frid2pa(id);
		//pt = frid2pa(upper_id);
		if(GDB)
			kprintf("pd@frame[%d], pt@frame[%d], p@frame[%d] frame.vpno=%d\n",pa2frid((unsigned long)pd), upper_id, id, frm_tab[id].fr_vpno);
		if( SYSERR == bsm_lookup(currpid, frm_tab[id].fr_vpno, &store, &pageth))
		{
			if(GDB)
				kprintf("ERROR: This virtual address hasn't been mapped!\n");
			kill(currpid);
    	}
    	if(GDB)
    		kprintf("writing frame[%d] back to bs[%d]@(%d)th page\n",id,store,pageth);
    	write_bs((char *)p, store, pageth);
    	init_pt(p);
    	reset_frm(id);

		frm_tab[upper_id].fr_refcnt--;
		if(GDB)
			kprintf("frame[%d].fr_refcnt=%d\n",upper_id,frm_tab[upper_id].fr_refcnt);
		if(frm_tab[upper_id].fr_refcnt <= 0)
		{	
			if(GDB)
				kprintf("its page_table @frame[%d].fr_refcnt = %d \n",upper_id,frm_tab[upper_id].fr_refcnt);
			if(GDB)
				kprintf("going to free Page Table @ frame[%d]\n",upper_id);
			free_frm(upper_id);
		}

		restore(ps);
		return OK;
	}
	else if(frm_tab[id].fr_type == FR_TBL)
	{
		upper_id = frm_tab[id].fr_upper_t;
		pt = frid2pa(id);
		if(GDB)
			kprintf("pd@frame[%d], pt@frame[%d] frame.vpno=%d\n",pa2frid((unsigned long)pd), id, frm_tab[id].fr_vpno);
		// no need to write back to bs for page_table_frame
    	init_pt(pt);
    	reset_frm(id);

		frm_tab[upper_id].fr_refcnt--;
		if(GDB)
			kprintf("its page_directory @frame[%d].fr_refcnt = %d \n",upper_id,frm_tab[upper_id].fr_refcnt);
		if(frm_tab[upper_id].fr_refcnt <= 0)
		{
			free_frm(upper_id);
		}

		restore(ps);
		return OK;
	}	
	else
	{
		if(GDB)
			kprintf("going to free all Local PDEs for Page Directory @ frame[%d].fr_refcnt= %d\n",id,frm_tab[id].fr_refcnt);  		pd = frid2pa(id);
  		init_pd(pd);
  		reset_frm(id);
  		/**
  		 * shouldn do below when the process is back 
  		 * and its previous frames are restored back from backing stores
  		 * 
  		 */
  		//proctab[currpid].bsmap[store].frames = NULL;
		restore(ps);
  		return OK;
	}
}

/*-------------------------------------------------------------------------
 * find_frm - find a frame using bs_id and bs_pageth
 *-------------------------------------------------------------------------
 */
SYSCALL find_frm(int pid, int vpno)
{

	int i;
	STATWORD ps;
	// Disable interrupts
    disable(ps);
	// Iterate over the frames
    for (i=0; i < NFRAMES; i++) {

        // Is this frame free, if so skip
        if (frm_tab[i].fr_status == FRM_UNMAPPED)
            continue;

        // Is this a backing store page. If not skip
        if (frm_tab[i].fr_type != FR_PAGE)
            continue;

        // Does the bsid/bsoffset match? If so.. bingo
        if (frm_tab[i].fr_pid == pid && frm_tab[i].fr_vpno == vpno) 
        {
            if(GDB)
            	kprintf("**********************Frame %d for {pid:%d vpno:%d} already allocated before.\n", 
                i,
                pid,
                vpno);
            restore(ps);
            return i;
        }

    }
    restore(ps);
    // If we are here then we did not find anything
    return -1;

}


/*-------------------------------------------------------------------------
 * dec_frm_refcnt - decrease the reference count for each frame allocated
 * to this process for one particular backing store.
 *-------------------------------------------------------------------------
 */
 SYSCALL dec_frm_refcnt(int pid, int s)
 {
 	STATWORD ps;
	// Disable interrupts
    disable(ps);
 	fr_map_t *curr,*prev;
 	curr = proctab[pid].bsmap[s].frames;
 	if(GDB)
 		kprintf("proctab[%d].bsmap[%d].frames = %x, ->vpno=%d, ->bs_next=%x\n",
 		pid, s, curr, curr->fr_vpno, curr->bs_next);
 	while(curr != NULL)
 	{
 		if(GDB)
 			kprintf("in dec_frm_refcnt: curr = frame[%d], ->fr_vpno=%d\n",
 			curr->fr_id, curr->fr_vpno);
 		curr->fr_refcnt--;
 		if(GDB)
 			kprintf("frame[%d].fr_refcnt = %d\n",curr->fr_id,curr->fr_refcnt);
 		if(curr->fr_refcnt <= 0)
 		{
 			prev = curr;
 			curr = curr->bs_next;
 			if(GDB)
 				kprintf("going to free frame[%d]\n",prev->fr_id);
 			free_frm(prev->fr_id);
 		}
 		else
 			curr = curr->bs_next;
 	}
 	restore(ps);
 	return OK;
 }

/*
 fr_map_t* reverse_list(fr_map_t *head){
 	if(head==NULL || head->bs_next==NULL)
 		return head;
 	kprintf("head.frame[%d]\n", head->fr_id);
 	fr_map_t* new_head = reverse_list(head->bs_next);
 	head->bs_next->bs_next = head;
 	head->bs_next = NULL;
 	return new_head;
 }
*/
 SYSCALL write_back()
 {
 	//kprintf("checking if writing back is needed for old_pid=%d (%s).\n",pid,proctab[pid].pname);
 	int i, upper_id, store, pageth;
 	pd_t *pd;	
	pt_t *pt, *p;
	unsigned int pd_offset,pt_offset,pg_offset;
	STATWORD ps;
	// Disable interrupts
    disable(ps);
	int local_counter=0;
	pd = proctab[old_pid].pdbr;
	if(GDB)
		kprintf("Hi! we are in write_back(), pid=%d (%s)\n",old_pid, proctab[old_pid].pname);
 	for(i=0; i<NFRAMES; i++)
 	{
 		if(frm_tab[i].fr_status == FRM_MAPPED && frm_tab[i].fr_type == FR_PAGE && frm_tab[i].fr_pid == old_pid)
 		{
 			if(GDB)
 				kprintf("it seems frame[%d] is the one that MAPPED, FR_PAGE, PID\n",i);
			p = frid2pa(i);
			upper_id = frm_tab[i].fr_upper_t;
			//pt = frid2pa(upper_id);
			if(GDB)
				kprintf("pd@frame[%d], pt@frame[%d], p@frame[%d] frame.vpno=%d\n",pa2frid((unsigned long)pd), upper_id, i, frm_tab[i].fr_vpno);
			if( SYSERR == bsm_lookup(old_pid, frm_tab[i].fr_vpno, &store, &pageth))
			{
				if(GDB)
					kprintf("ERROR: This virtual address hasn't been mapped!\n");
				kill(old_pid);
			}
			if(GDB)
				kprintf("writing frame[%d] back to bs[%d]@(%d)th page\n",i,store,pageth);
			
			//**********************************************************
			proc_frames[old_pid][local_counter++]=frm_tab[i].fr_vpno;
			if(GDB)
				kprintf("proc_frames[%d][%d] = %d =@%8x\n", old_pid, local_counter-1, frm_tab[i].fr_vpno, 4096*frm_tab[i].fr_vpno);
			write_bs((char *)p, store, pageth);
			if(GDB)
				kprintf("after writing back, i'd like to free frame[%d]\n",i);

	    	init_pt(p);
	    	reset_frm(i);
			frm_tab[upper_id].fr_refcnt--;
			if(GDB)
				kprintf("its page_table @frame[%d].fr_refcnt = %d \n",upper_id,frm_tab[upper_id].fr_refcnt);
			if(frm_tab[upper_id].fr_refcnt <= 0)
			{	
				if(GDB)
					kprintf("going to free Page Table @ frame[%d]\n",upper_id);

				//should clear the present bit of the its page directory.
	    		/*
	    		int vpno = frm_tab[upper_id].fr_vpno;
	    		virt_addr_t * vaddr = vpno*NBPG;
	    		pd_offset = vaddr->pd_offset;
			    pt_offset = vaddr->pt_offset;
			    pg_offset = vaddr->pg_offset;
			    kprintf("frame[%d].vpno=%d, Vaddr=%x, pd_offset=%d, pt_offset=%d, pg_offset=%d\n",upper_id, vpno, (unsigned long)vaddr, pd_offset, pt_offset, pg_offset);
				*/
				int id = upper_id;
				upper_id = frm_tab[upper_id].fr_upper_t;

				pt = frid2pa(id);
				init_pt(pt);
	    		reset_frm(id);
	    		
	    		frm_tab[upper_id].fr_refcnt--;
	    		//if(GDB)
	    			kprintf("its page_directory @frame[%d].fr_refcnt = %d \n",upper_id,frm_tab[upper_id].fr_refcnt);
	    		if(frm_tab[upper_id].fr_refcnt <= 0)
	    		{
	    			//if(GDB)
	    				kprintf("going to free all Local PDEs for Page Directory @ frame[%d]\n",upper_id);
	    			init_pd(pd);	//also clean up the page directory for all the local page table it got.
	    			/**
			  		 * shouldn do below when the process is back 
			  		 * and its previous frames are restored back from backing stores
			  		 */
	    			//proctab[old_pid].bsmap[store].frames = NULL;
	    		}
			}

 		}
 	}
	 /**
	 * Finally must invalidate TLB entries since page table contents 
	 * have changed. From intel vol III
	 * All of the (nonglobal) TLBs are automatically invalidated any
	 * time the CR3 register is loaded.
	 */
    //set_PDBR(currpid);
   	local_counter = 0;
 	restore(ps);
    return OK;
 }
//from bs to frames.
SYSCALL recover_back(pid){
	int i=0, test;
	int *addr; 
	//kprintf("in recover_back:\n");
	while(proc_frames[pid][i] != 0){
		//kprintf("i=%d, vpno=%d =@%8x\n",i, proc_frames[pid][i], (proc_frames[pid][i]*4096));
		test = *((int *) (proc_frames[pid][i]*4096));
		//kprintf("test = %d\n",test);
		i++;
	}
	return OK;
}	
/*
  SYSCALL recover_frame(int pid)
 {
 	int i, temp, store, pageth;
 	fr_map_t *curr,*test;
 	unsigned long p;

 	STATWORD ps;
	// Disable interrupts
    disable(ps);
    Enter_From_restore_frame = 1;
 	for(i=0; i<NBS; i++)
 	{
 		if(proctab[pid].bsmap[i].frames == NULL)
 			continue;
 		else
 		{
 			curr = proctab[pid].bsmap[i].frames;
		 	curr = reverse_list(curr);
		 	test = curr;
		 	while(test != NULL){
		 		kprintf("test.frame[%d]\n", test->fr_id);
		 		test=test->bs_next;
		 	}
		 	while(curr != NULL)
		 	{
		 		//if(GDB)
		 		kprintf("proctab[%d].bsmap[%d].frames = frame[%d], ->vpno=%d @%8x, ->bs_next=%x\n",
		 		pid, i, curr->fr_id, curr->fr_vpno, curr->fr_vpno*4096, curr->bs_next);
			 	if( SYSERR == bsm_lookup(pid, curr->fr_vpno, &store, &pageth))
				{
					if(GDB)
						kprintf("ERROR: This virtual address hasn't been mapped!\n");
					kill(pid);
				}
				kprintf("found vpno:%d vaddr:%8x on bs[%d].offest(%d)\n",
					curr->fr_vpno, (curr->fr_vpno)*4096, store, pageth );
		 		
		 		temp = *((int *) ((curr->fr_vpno)*4096));
		 		kprintf("*vaddr[%8x] = %d\n", (curr->fr_vpno)*4096, temp);
		 		//curr = curr->bs_next;
		 		kprintf("before run next curr->next = frame[%d] vaddr=%8x\n\n\n", curr->fr_id, (curr->fr_vpno)*4096);
		 		test = curr;
			 	while(test != NULL){
			 		kprintf("test.frame[%d]\n", test->fr_id);
			 		test=test->bs_next;
			 	}
		 	}
 		}
 	}
	 
	 * Finally must invalidate TLB entries since page table contents 
	 * have changed. From intel vol III
	 * All of the (nonglobal) TLBs are automatically invalidated any
	 * time the CR3 register is loaded.
	 
    set_PDBR(currpid);
    Enter_From_restore_frame = 0;
 	restore(ps);
    return OK;
 }
*/