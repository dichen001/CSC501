#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>
/**
 * initialize the 4 globle_page_table first.
 */
int gpt[4];		//global page table.
SYSCALL init_gpt(){
	kprintf("\n\nstart initializing Globle Page Table\n");
	int i;
	for (i = 0; i < 4; ++i)
	{
		int avail = get_frm();	//get the frame id of a new frame from frm_tab[];
		//initialize frame[avail], update the process_id and frame_type of this frame.
		init_frm(avail, NULLPROC, FR_TBL);	
		gpt[i] = frid2vpno(avail);
		pt_t *new_pt = frid2pa(avail);
		int j;
		for (j = 0; j < 1024; ++j)
		{
			new_pt[j].pt_pres = 1;
			new_pt[j].pt_write = 1;
			new_pt[j].pt_user = 0; // not sure about the usage;
			new_pt[j].pt_pwt = 0;
			new_pt[j].pt_pcd = 0;
			new_pt[j].pt_acc = 0;
			new_pt[j].pt_dirty = 0;
			new_pt[j].pt_mbz = 0;
			new_pt[j].pt_global = 1;
			new_pt[j].pt_avail = 0; // not in use right now.
			new_pt[j].pt_base = i*1024 + j;	//map page 0-4095, i.e. the first 16M of physical memory.
		}
		kprintf("PT Base Address: %8x   PT_Value: new_pt[0] = %8x\n",new_pt,new_pt[0]);
	}
	kprintf("Globle Page Table initialized \n");
	return OK;
}

SYSCALL init_pd(pd_t *new_pd){
	int i;
	for (i = 0; i < 1024; ++i)
	{
		new_pd[i].pd_pres = 0;
		new_pd[i].pd_write = 1;
		new_pd[i].pd_user = 0; // not sure about the usage;
		new_pd[i].pd_pwt = 0;
		new_pd[i].pd_pcd = 0;
		new_pd[i].pd_acc = 0;
		new_pd[i].pd_mbz = 0;
		new_pd[i].pd_fmb = 0;
		new_pd[i].pd_global = 0;
		new_pd[i].pd_avail = 0; // not in use right now.
		new_pd[i].pd_base = 0;	
	}
	kprintf("pd: %x initialized\n",new_pd);
	return OK;
}


SYSCALL init_pt(pt_t *new_pt){
	int i;
	for (i = 0; i < 1024; ++i)
	{
		new_pt[i].pt_pres = 0;
		new_pt[i].pt_write = 1;
		new_pt[i].pt_user = 0; // not sure about the usage;
		new_pt[i].pt_pwt = 0;
		new_pt[i].pt_pcd = 0;
		new_pt[i].pt_acc = 0;
		new_pt[i].pt_dirty = 0;
		new_pt[i].pt_mbz = 0;
		new_pt[i].pt_global = 0;
		new_pt[i].pt_avail = 0; // not in use right now.
		new_pt[i].pt_base = 0;	
	}
	kprintf("pt: %x initialized\n",new_pt);
	return OK;
}

SYSCALL create_PD(int pid){
	kprintf("\n\ncreating Page Directory for process: %s \n",proctab[pid].pname);
	int avail = get_frm();	//get the id of a new frame from frm_tab[];
	//initialize frame[avail], update the process_id and frame_type of this frame.
	init_frm(avail, pid, FR_DIR);
	pd_t *new_pd = frid2pa(avail);
	init_pd(new_pd);
	/* map page 0-4095, i.e. the first 16M of physical memory. */
	
	int i;
	for(i = 0; i < 4; i++){
		new_pd[i].pd_pres = 1;
		new_pd[i].pd_global = 1;
		new_pd[i].pd_base = gpt[i];	// record the location of the 4 global page table 
		kprintf("PD_base_Address: %8x  Value(PDE): new_pd[%d] = %8x\n",new_pd,i,new_pd[i]);
	}
	
	// initialize the demand-paging related value for this newly created process. 
	proctab[pid].pdbr = (unsigned long) new_pd; //record the page_directory_base_register for this process.
	proctab[pid].vmemlist = NULL;
	
	init_bsm(proctab[pid].bsmap);
	
	kprintf("new Page Directory for process: %s created\n",proctab[pid].pname);
	return OK;
}


