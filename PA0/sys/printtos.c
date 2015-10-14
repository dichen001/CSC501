#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

unsigned long *x1, *x2;

void printtos(){
	asm("movl %esp, x1");
	asm("movl %ebp, x2");
	kprintf("ebp:\t 0x%x \nesp_before_entry:\t 0x%x \n",x2,x2+2);
	kprintf("esp:\t 0x%x \n",x1);
	int i;
	for(i=0; i<6;i++)
		kprintf("esp+%d:\t 0x%x [esp+%d]:\t 0x%x\n",i,x1+i,i,*(x1+i));
	kprintf("--------------------------------\n");
}
