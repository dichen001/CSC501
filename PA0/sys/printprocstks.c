#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

static unsigned long *esp;

void printprocstks(int priority){
	struct  pentry *p;
	int i;
	for(i = 0; i < NPROC; i++){
		p = &proctab[i];
		if(i >= 0 && i < NPROC && p->pprio < priority && p->pstate != PRFREE){
			if(p->pstate == PRCURR){
        			asm("movl %esp, esp");
				kprintf("\n***** Currently Executing Process *****\n");
				kprintf("Current stack pointer:%t 0x%x\n", esp);
				kprintf("Run time stack size:\t %d\n",esp - p->pbase);
			}
			kprintf("Saved stack pointer:\t 0x%x\n", p->pesp);
			kprintf("Stack base:\t 0x%x\n", p->pbase);
			kprintf("Stack size:\t %d\n", p->pstklen);
			kprintf("Stack limit:\t 0x%x\n", p->plimit);
			kprintf("Process name:\t %s\n", p->pname);
			kprintf("Pid:\t %d\n", i);
			kprintf("Priority:\t %d\n", p->pprio);
			kprintf("-----------------------------------\n\n");
		}
	}
}
