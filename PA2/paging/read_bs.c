#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>
#include <proc.h>
#include <paging.h>

SYSCALL read_bs(char *dst, bsd_t bs_id, int page) {

  /* fetch page page from map map_id
     and write beginning at dst.
  */
 	//kprintf("in read_bs\n");
   void * phy_addr = BACKING_STORE_BASE + (bs_id<<19) + page*NBPG;
  // kprintf("phy_addr = %x\n",(unsigned int)phy_addr);
   bcopy(phy_addr, (void*)dst, NBPG);

   struct mblock * memblock = phy_addr;
  // kprintf("phy_addr->mnext@[%x] = %x, phy_addr->mlen@[%x] = %d\n",
  // 	&memblock->mnext, memblock->mnext, &memblock->mlen, memblock->mlen);

   memblock = dst;
  // kprintf("dst->mnext@[%x] = %x, dst->mlen@[%x] = %d\n",
  // 	&memblock->mnext, memblock->mnext, &memblock->mlen, memblock->mlen);


}


