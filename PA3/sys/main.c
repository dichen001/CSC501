/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

void halt();

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{
	kprintf("\n\nHello World, Xinu lives\n\n");
	return 0;
}


#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>


/* test1.c
 * This test program creates three processes, two readers and a writer at
 * priority 20.  The main process also has priority 20.
 * All readers and writers have the same priority.
 */

int reader(char, int, int);
int writer(char, int, int);

int reader1, reader2;
int writer1;
int lck1;

int main()
{
	kprintf("\n\nHello World, Xinu lives\n\n");


    int i;
    /*create lock*/
    lck1 = lcreate();


    kprintf("\n\nTEST1:\n");
    resume(reader1 = create(reader,2000,20,"reader1",3,'A', lck1, 30));
    resume(reader2 = create(reader,2000,20,"reader2",3,'B', lck1, 30));
    resume(writer1 = create(writer,2000,20,"writer1",3,'C', lck1, 30));

    while (1) {
            kprintf("%c", 'D');
            for (i=0; i< 10000; i++);
    }

	return 0;
}


int reader(char c, int lck, int prio){
   int i;

   while(1) {
     lock(lck, READ, prio);
     sleep(5);
     releaseall(1, lck);

   }
}

int writer(char c, int lck, int prio){
  int i;

   while(1) {
     lock(lck, WRITE, prio);
     sleep(5);
     releaseall(1, lck);

   }
}
