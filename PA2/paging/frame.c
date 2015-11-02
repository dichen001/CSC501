/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

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
	frm_tab[i].fr_status = FRM_MAPPED;	// ** will here be wrong? ** //
	frm_tab[i].fr_pid = pid;
	frm_tab[i].fr_vpno = -1;
	frm_tab[i].fr_refcnt += 1;
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
SYSCALL free_frm(int i)
{
	frm_tab[i].fr_status = FRM_UNMAPPED;
	frm_tab[i].fr_pid = -1;
	frm_tab[i].fr_vpno = -1;
	frm_tab[i].fr_refcnt = 0;
	frm_tab[i].fr_type = -1;
	frm_tab[i].fr_dirty = 0;
	frm_tab[i].cookie = NULL;
	frm_tab[i].fr_loadtime = 0;

  kprintf("Frame[%d] freed\n",i);
  return OK;
}



