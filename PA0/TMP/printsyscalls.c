#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

int flag = 1;
int freq[6][NPROC];
char sys[6][20] = {
        "getpid",
        "gettim",
        "kill",
        "signal",
        "sleep",
        "wait"
};


void initfreq(){
	int i,j;
	for(i = 0; i < 6; i++)
                for(j = 0; j < NPROC; j++)
                        freq[i][j] = 0;
}

void syscallsummary_start(){
	initfreq();
	flag = 1;
	kprintf("syscall_summary_start: \tflag:%d\n",flag);
}

void syscallsummary_stop(){
        flag = 0;
        kprintf("syscallsummary_stop: \tflag:%d\n",flag);
}

void show_freq(int pid){
	int i;
	kprintf("------ Summary of process \"%s\" (pid:%d) ------\n",proctab[pid].pname, pid);
	for(i = 0; i < 6; i++){
		kprintf("process (%s) has been called \t%10d times\n",sys[i],freq[i][pid]);
	}
}

void printsyscalls(){
        int i;
	struct  pentry *p;
        for(i = 0; i < NPROC; i++){
		p = &proctab[i];
                if(i >= 0 && i < NPROC && p->pstate != PRFREE){
			show_freq(i);
		}
	}
}
