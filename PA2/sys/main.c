/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>

void halt();

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{

	kprintf("\nRuning main() \n\n");
	int i = 0;
	kprintf("&i = %8x, i = %d\n",&i,i);
	return 0;
}
