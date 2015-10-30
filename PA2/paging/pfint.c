/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{

	unsigned long cr2;
	virt_addr_t * vaddr; 
	STATWORD ps;
	// Disable interrupts
    disable(ps);
  	kprintf("**pfint** is starting up!\n");
  	// Get the faulted address. The processor loads the CR2 register
    // with the 32-bit address that generated the exception.
    
    //vaddr = (virt_addr_t) read_cr2();
    cr2 = read_cr2();
    vaddr = (virt_addr_t *)(&cr2); 
    kprintf();
  return OK;
}


