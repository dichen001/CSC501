#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>
/**
 * initialize the 4 globle_page_table first.
 */
int gpd[4];		//global page table.
SYSCALL init_gpd(){
	kprintf("start initializing globle_page_table\n");
	int i;
	for (i = 0; i < 4; ++i)
	{
		int avail = get_frm();	//get the id of a new frame from frm_tab[];
		//initialize frame[avail], update the process_id and frame_type of this frame.
		init_frm(avail, NULLPROC, FR_TBL);	
		gpd[i] = avail;

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
			new_pt[j].pt_avail = avail; // indicates the frame id this page lies.
			new_pt[j].pt_base = i*1024 + j;	//map page 0-4095, i.e. the first 16M of physical memory.
		}
	}
	kprintf("globle_page_table initialized \n");
	return OK;
}

SYSCALL create_PD(int pid){
	kprintf("creating page directory for process: %s \n",proctab[pid].pname);
	int avail = get_frm();	//get the id of a new frame from frm_tab[];
	//initialize frame[avail], update the process_id and frame_type of this frame.
	init_frm(avail, pid, FR_DIR);
	pd_t *new_pd = frid2pa(avail);
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
		new_pd[i].pd_avail = avail; // indicates the frame id this page_table lies.
		new_pd[i].pd_base = -1;	//map page 0-4095, i.e. the first 16M of physical memory.
		if (i < 4)
		{
			new_pd[i].pd_pres = 1;
			new_pd[i].pd_global = 1;
			new_pd[i].pd_base = gdb[i];	//map the 4 global page tables.
		}
	}
}