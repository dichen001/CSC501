/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * pfint - paging fault handlerISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
    int GBD = 1;
  	unsigned long cr2,physical_addr;
  	virt_addr_t * vaddr;
    int vp,s,o,avail,*store,*pageth;
  	unsigned int p,q,pt;
    pd_t *pd;
    pt_t *new_pt;
    STATWORD ps;
	// Disable interrupts
    disable(ps);
    if(GBD)
      kprintf("\n*************pfint is running!************\n");
  // Get the faulted address. The processor loads the CR2 register
  // with the 32-bit address that generated the exception.
  /* 1. Get the faulted address. */
    cr2 = read_cr2();
    vaddr = (virt_addr_t *)(&cr2); 
    if(GBD)
      kprintf("&cr2=%x, cr2=%x, &vaddr=%x, vaddr=%x\n",&cr2,cr2,&vaddr,vaddr);
  /* 2. Let 'vp' be the virtual page number of the page containing of the faulted address */
    vp = a2pno(cr2);
    if(GBD)
      kprintf("vp=%d,\n",vp);
  /* 3. Let pd point to the current page directory. */
    pd = proctab[currpid].pdbr;
    if(GBD)
      kprintf("pd=%x,\n",pd);
  /* 4. Check that a is a legal address (i.e., it has been mapped). 
     If it is not, print an error message and kill the process. */  
    pageth = getmem( sizeof(int *) );
    store = getmem( sizeof(int *) );
    if( SYSERR == bsm_lookup(currpid, vp, store, pageth)){
      kprintf("ERROR: This virtual address hasn't been mapped!\n");
      kill(currpid);
    }
  /* 5. Let p be the upper ten bits of a. [p represents page dirctory offset] */
  /* 6. Let q be the bits [21:12] of a. [p represents page table offset.]
  /* 7.1 Let pt point to the pth page table.*/
    p = vaddr->pd_offset;
    q = vaddr->pt_offset;
    pt = vaddr->pg_offset;
    if(GBD)
      kprintf("p=%x,q=%x,pt=%x\n",p,q,pt);
  /* 7.2  If the pth page table does not exist obtain a frame for it and initialize it. */
    if(pd[p].pd_pres != 1){
      if(GBD)
        kprintf("**obtain a frame for the new page table. \n");
      avail = get_frm();  //get the id of a new frame from frm_tab[];
      if (avail == -1) {
            if(GBD)
              kprintf("Could not create page table!\n");
            restore(ps);
            return SYSERR;
      }
      //initialize frame[avail], update the process_id and frame_type of this frame.
      init_frm(avail, currpid, FR_TBL);
      frm_tab[avail].fr_upper_t = pa2frid((unsigned long) pd);
      kprintf("upper page table @frame[%d]  pd=%x, a2pno(pd)=%d\n",frm_tab[avail].fr_upper_t, pd, a2pno((unsigned long) pd));
      new_pt = frid2pa(avail);
      init_pt(new_pt);
      //update this page_table_entry in the page_directory.
      pd[p].pd_pres = 1;
      pd[p].pd_write = 1;
      pd[p].pd_user = 0; // not sure about the usage;
      pd[p].pd_pwt = 0;
      pd[p].pd_pcd = 0;
      pd[p].pd_acc = 0;
      pd[p].pd_mbz = 0;
      pd[p].pd_fmb = 0;
      pd[p].pd_global = 0;
      pd[p].pd_avail = 0; // not in use right now.
      pd[p].pd_base = a2pno((unsigned long) new_pt);  /* location of page table */
      if(GBD)
        kprintf("New page_table(%x)@frame[%d] updated in page_directory[%d]@(frame[%d])\n",
        new_pt, avail, p, frm_tab[avail].fr_upper_t);
      kprintf("q=%d, new_pt[q]=%x, new_pt=%x, pd[p].pd_base=%x\n",q, new_pt[q], new_pt, pd[p].pd_base);
    }
    else
    {
      int avail = pd[p].pd_base -1024;
      frm_tab[avail].fr_refcnt++;
      if(GBD)
        kprintf("frm_tab[%d].fr_refcnt = %d, frame_type: %d\n",avail, frm_tab[avail].fr_refcnt, frm_tab[avail].fr_type);
    }
/* 8.1 Using the backing store map, find the store s and page offset o which correspond to vp. */
    //already saved in 'store' and 'pageth'
    s = *store;
    o = *pageth;
/* 8.2 In the inverted page table increment the reference count of the frame which holds pt. 
   This indicates that one more of pt's entries is marked "present." */
    avail = find_frm(currpid,vp);
    if (avail == -1)
    {
      if(GBD)
        kprintf("allocating a page for the page fault\n");

      avail = get_frm();
      if(avail == -1)
      {
        if(GBD)
          kprintf("ATTENTION! Frames full. ###Replacement NEEDED!###\n");
        kill(currpid);
        restore(ps);
        return SYSERR;
      }
      else
      {
        init_frm(avail, currpid, FR_PAGE);
        frm_tab[avail].fr_upper_t = pd[p].pd_base-FRAME0;
        kprintf("upper page table @frame[%d]\n",frm_tab[avail].fr_upper_t);
        frm_tab[avail].fr_vpno = vp;
        // Add this frame to head of the frame list within the bs of this process
        //(frm_tab[avail].bs_next)->fr_vpno
        //, proctab[currpid].bsmap[s].frames->bs_next
        kprintf("&frm_tab[avail].bs_next = %x\n",frm_tab[avail].bs_next, &frm_tab[avail].bs_next);
        kprintf("proctab[%d].bsmap[%d].frames = %x, ->vpno=%d, ->bs_next=%x\n",currpid, s, proctab[currpid].bsmap[s].frames, proctab[currpid].bsmap[s].frames->fr_vpno, proctab[currpid].bsmap[s].frames->bs_next);
        frm_tab[avail].bs_next = proctab[currpid].bsmap[s].frames;
        proctab[currpid].bsmap[s].frames = &frm_tab[avail];
        fr_map_t *frame = proctab[currpid].bsmap[s].frames;
        int i = frame->fr_vpno;
        kprintf("i = %d\n",i);
        kprintf("~~~frame[%d] linked to the head of the list 'frames'\n",avail);
        kprintf("frm_tab[avail].bs_next = %x, &**=%x\n",frm_tab[avail].bs_next, &frm_tab[avail].bs_next);
        kprintf("proctab[%d].bsmap[%d].frames = %x, ->vpno=%d, ->bs_next=%x\n",currpid, s, proctab[currpid].bsmap[s].frames, proctab[currpid].bsmap[s].frames->fr_vpno, proctab[currpid].bsmap[s].frames->bs_next);

        if(GBD)
          kprintf("Mapping frame[%d](ppno[%d]) to {pid[%d], vpno[%d]} -> {bs[%d],offset:%d}\n",
          avail,frid2vpno(avail),currpid,vp,s,o);
        
        physical_addr = frid2pa(avail);
        read_bs(physical_addr,s,o);
        if(GBD)
          kprintf("copied from bs[%d]:offset[%d] to vp[%d]@(%x)\n",s,o,vp,vp2pa(vp));
      }
    }

    else
    {
      frm_tab[avail].fr_refcnt++;
      if(GBD)
        kprintf("frm_tab[%d].fr_refcnt = %d, frame_type: %d\n",avail, frm_tab[avail].fr_refcnt, frm_tab[avail].fr_type);
    }

/* Update the page table entry for this newly got page*/
    new_pt = vp2pa(pd[p].pd_base);
    new_pt[q].pt_pres  = 1;
    new_pt[q].pt_write = 1;
    new_pt[q].pt_base  = frid2vpno(avail);
    if(GBD)
      kprintf("q=%d, new_pt[q]=%x, new_pt=%x, pd[p].pd_base=%x,\n",q, new_pt[q], new_pt, pd[p].pd_base);
      kprintf("New page (%x)@frame[%d] updated in page_table[%d]@(frame[%d])\n",
        new_pt[q], avail, q, frm_tab[avail].fr_upper_t);

    if(GBD)
      kprintf("*************pfint completed!*************\n");
/**
 * Finally must invalidate TLB entries since page table contents 
 * have changed. From intel vol III
 * All of the (nonglobal) TLBs are automatically invalidated any
 * time the CR3 register is loaded.
 */
    set_PDBR(currpid);

    restore(ps);
    return OK;
}


