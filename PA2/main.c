/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>

void halt();

/*------------------------------------------------------------------------
 *  main  --  user main program
 *-----------------------------------
 *-------------------------------------
 */

void proc1_test2() {
	kprintf("\nRuning proc1_test2() \n\n");
	int i,temp;
	i = 0;
	kprintf("&i = %8x, i = %d\n",&i,i);
	
	int *y;
	struct	mblock	*x;
	
	kprintf("************************************\n");
	x = vgetmem(1000);
	//x= (struct	mblock *)x;
	kprintf("x=%8x \t x->mnext=%8x \t x->mlen=%d\n",x,x->mnext,x->mlen);
	y=x;
	kprintf("&y=%x y=%x *y=%d\n",&y,y,*y);
	*y = 100; 
	kprintf("&y=%x y=%x *y=%d\n",&y,y,*y);
    y++; 
    kprintf("&y=%x y=%x *y=%d\n",&y,y,*y);
    *y = 200; 
    kprintf("&y=%x y=%x *y=%d\n",&y,y,*y);
    temp = *y; 
    kprintf("temp=%d\n",temp);
	kprintf("####################################\n");
	vfreemem(--y,1000);
	kprintf("\n vfreemem(x,1000); \n\n");
	
	/*
	kprintf("************************************\n");
	x = vgetmem(1000);
	kprintf("####################################\n");
	vfreemem(x,500);
	kprintf("\n freemem(x,500); \n\n");
	
	kprintf("************************************\n");
	x = vgetmem(1000);
	kprintf("####################################\n");
	vfreemem(x+500,500);
	kprintf("\n freemem(x+500,500); \n\n");
	*/

	kprintf("************************************\n");
	i = get_bs(4, 100); 
	kprintf("i=%d\n",i);
	kprintf("####################################\n");
	
	xmmap(7000,4,100);
	char *addr =7000*4096;
	kprintf("&addr=%x addr=%x *addr=%c\n",&addr,addr,*addr);
	*addr = 'Y';
	kprintf("&addr=%x addr=%x *addr=%c\n",&addr,addr,*addr);
	char tem = *addr;
	kprintf("tem=%c\n",tem);

	xmunmap(7000);
	release_bs(4);
	kprintf("************************************\n");

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
