/* sched.c - setschedclass, getschedclass */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
int sched_class = 0;
extern int epoch_id;
extern int epoch;
void setschedclass(int sched) {
	sched_class = sched;
	epoch_id = 0;
	epoch = 0;
}
int getschedclass(){
	return sched_class;
}


