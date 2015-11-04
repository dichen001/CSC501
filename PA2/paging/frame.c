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
			kprintf("Frame[%d] got!\n",i);
			return i;
		}
	}
	kprintf("No Frame left!\n");
	return -1;
}


/*-------------------------------------------------------------------------
 * init_frm - initialize a frame after get_frm
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm(int i, int pid, int type){
	frm_tab[i].bs_next = NULL;
	frm_tab[i].fr_upper_t = -1;

	frm_tab[i].fr_status = FRM_MAPPED;	// ** will here be wrong? ** //
	frm_tab[i].fr_pid = pid;
	frm_tab[i].fr_refcnt = 1;
	frm_tab[i].fr_type = type;
	frm_tab[i].fr_dirty = 0; 
	kprintf("frame[%d].pid=%d .vpno=%d .refcnt=%d\n",i,pid,frid2vpno(i),frm_tab[i].fr_refcnt);
	kprintf("frame[%d] initialized\n",i);
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
	if(frm_tab[id].fr_type == FR_PAGE)
	{
		upper_id = frm_tab[id].fr_upper_t;
		p = frid2pa(id);
		//pt = frid2pa(upper_id);
		kprintf("pd@frame[%d], pt@frame[%d], p@frame[%d] frame.vpno=%d\n",pa2frid((unsigned long)pd), upper_id, id, frm_tab[id].fr_vpno);
		if( SYSERR == bsm_lookup(currpid, frm_tab[id].fr_vpno, &store, &pageth))
		{
			kprintf("ERROR: This virtual address hasn't been mapped!\n");
			kill(currpid);
    	}
    	kprintf("writing frame[%d] back to bs[%d]@(%d)th page\n",id,store,pageth);
    	write_bs((char *)p, store, pageth);
    	for(i=0; i<1024; i++)
    	{
    		p[i].pt_pres = 0;
			p[i].pt_write = 1;
			p[i].pt_user = 0;
			p[i].pt_pwt = 0;
			p[i].pt_pcd = 0;
			p[i].pt_acc = 0;
			p[i].pt_dirty = 0;
			p[i].pt_mbz = 0;
			p[i].pt_global = 0;
			p[i].pt_avail = 0;
			p[i].pt_base = 0;
    	}
    	frm_tab[id].fr_status = FRM_UNMAPPED;
		frm_tab[id].fr_pid = -1;
		frm_tab[id].fr_vpno = -1;
		frm_tab[id].fr_refcnt = 0;
		frm_tab[id].fr_type = -1;
		frm_tab[id].fr_dirty = 0;
		//frm_tab[i].fr_cookie
		frm_tab[i].fr_loadtime = -1;

		frm_tab[upper_id].fr_refcnt--;
		if(frm_tab[upper_id].fr_refcnt <= 0)
		{	
			kprintf("its page_table @frame[%d].fr_refcnt = %d \n",upper_id,frm_tab[upper_id].fr_refcnt);
			kprintf("going to free Page Table @ frame[%d]\n",upper_id);
			free_frm(upper_id);
		}
		return OK;
	}
	else if(frm_tab[id].fr_type == FR_TBL)
	{
		upper_id = frm_tab[id].fr_upper_t;
		pt = frid2pa(id);
		kprintf("pd@frame[%d], pt@frame[%d] frame.vpno=%d\n",pa2frid((unsigned long)pd), id, frm_tab[id].fr_vpno);
		// no need to write back to bs for page_table_frame
    	for(i=0; i<1024; i++)
    	{
    		pt[i].pt_pres = 0;
			pt[i].pt_write = 1;
			pt[i].pt_user = 0;
			pt[i].pt_pwt = 0;
			pt[i].pt_pcd = 0;
			pt[i].pt_acc = 0;
			pt[i].pt_dirty = 0;
			pt[i].pt_mbz = 0;
			pt[i].pt_global = 0;
			pt[i].pt_avail = 0;
			pt[i].pt_base = 0;
    	}
    	frm_tab[id].fr_status = FRM_UNMAPPED;
		frm_tab[id].fr_pid = -1;
		frm_tab[id].fr_vpno = -1;
		frm_tab[id].fr_refcnt = 0;
		frm_tab[id].fr_type = -1;
		frm_tab[id].fr_dirty = 0;
		//frm_tab[i].fr_cookie
		frm_tab[i].fr_loadtime = -1;
		frm_tab[upper_id].fr_refcnt--;
		kprintf("its page_directory @frame[%d].fr_refcnt = %d \n",upper_id,frm_tab[upper_id].fr_refcnt);
		return OK;
	}	
	else
	{
		kprintf("????????????????\n",i);
  		return SYSERR;
	}
}

/*-------------------------------------------------------------------------
 * find_frm - find a frame using bs_id and bs_pageth
 *-------------------------------------------------------------------------
 */
SYSCALL find_frm(int pid, int vpno)
{

	int i;

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
            kprintf("Frame %d for {pid:%d vpno:%d} already allocated before.\n", 
                i,
                pid,
                vpno);
            return i;
        }

    }

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

 	fr_map_t *curr,*prev;
 	curr = proctab[pid].bsmap[s].frames;
 	kprintf("proctab[%d].bsmap[%d].frames = %x, ->vpno=%d, ->bs_next=%x\n",
 		pid, s, curr, curr->fr_vpno, curr->bs_next);
 	while(curr != NULL)
 	{
 		kprintf("in dec_frm_refcnt: curr = frame[%d], ->fr_vpno=%d\n",
 			curr->fr_id, curr->fr_vpno);
 		curr->fr_refcnt--;
 		kprintf("frame[%d].reference = %d\n",curr->fr_id,curr->fr_refcnt);
 		if(curr->fr_refcnt == 0)
 		{
 			prev = curr;
 			curr = curr->bs_next;
 			kprintf("going to free frame[%d]\n",prev->fr_id);
 			free_frm(prev->fr_id);
 		}
 		else
 			curr = curr->bs_next;
 	}
 	return OK;
 }
