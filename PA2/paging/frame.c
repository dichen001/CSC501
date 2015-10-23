/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

fr_map_t frm_tab[NFRAMES];
/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
	kprintf("initialize frame \n");
	for (int i = 0; i < NFRAMES; ++i)
	{
		frm_tab[i].fr_status = UNMAPPED;
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
SYSCALL get_frm(int* avail)
{
	for (int i = 0; i < NFRAMES; ++i)
	{
		if (frm_tab[i].fr_status == UNMAPPED)
		{
			kprintf("Frame[%d] got!\n",i);
			return OK;
		}
		// replacement wait for coding
	}
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	frm_tab[i].fr_status = UNMAPPED;
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



