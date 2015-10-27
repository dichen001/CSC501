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

void proc1_test2() {
	kprintf("\nRuning proc1_test2() \n\n");
	int i = 0;
	kprintf("&i = %8x, i = %d\n",&i,i);
	i = vgetmem(1000);
/*
  int *x;

  kprintf("ready to allocate heap space\n");
  x = vgetmem(1024);
  kprintf("heap allocated at %x\n", x);
  *x = 100;
  *(x + 1) = 200;

  kprintf("heap variable: %d %d\n", *x, *(x + 1));
  vfreemem(x, 1024);
*/
}

int main()
{

	kprintf("\nRuning main() \n\n");
	int i = 0;
	kprintf("&i = %8x, i = %d\n",&i,i);

	int pid1;
	pid1 = vcreate(proc1_test2, 2000, 100, 20, "proc1_test2", 0, NULL);
	resume(pid1);
	return 0;
}
