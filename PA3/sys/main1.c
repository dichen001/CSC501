/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>

int lck1, lck2;
int rdA, rdC, rdE;
int wrB, wrD, wrF;
void procA()
{
  kprintf ("rdA try to acquire lock 1\n");
  int rc = lock(lck1, READ, 20);
  if(rc == OK) kprintf ("rdA acquires lock 1, sleep 2s\n");
  else kprintf("unable to get lock.\n");
  sleep (2);
  kprintf ("rdA releases lock 1\n");
  releaseall (1, lck1);
}

void procB()
{
  kprintf ("wrB try to acquire lock 2\n");
  int rc = lock(lck2, WRITE, 20);
  if(rc == OK) kprintf ("wrB acquires lock 2, sleep 2s\n");
  else kprintf("unable to get lock.\n");
  sleep (2);
  kprintf ("wrB releases lock 2\n");
  releaseall (1, lck2);
}

void procC()
{
  kprintf ("rdC try to acquire lock 1\n");
  int rc = lock(lck1, READ, 10);
  if(rc == OK) kprintf ("rdC acquires lock 1\n");
  else kprintf("unable to get lock.\n");
  kprintf ("rdC try to acquire lock 2\n");
  rc = lock(lck2, READ, 5);
  if(rc == OK) kprintf ("rdC acquires lock 2, sleep 2s\n");
  else kprintf("unable to get lock.\n");
  sleep (2);
  kprintf ("rdC releases lock 1&2\n");
  releaseall (2, lck1, lck2);
}

void procD()
{
  kprintf ("procD kills procA & procC.\n");
  kill(rdA);
  kill(rdC);
  kprintf ("wrD try to acquire lock 1\n");
  int rc = lock(lck1, WRITE, 10);
  if(rc == OK) kprintf ("wrD acquires lock 1\n");
  else kprintf("unable to get lock.\n");
  kprintf ("wrD try to acquire lock 2\n");
  rc = lock(lck2, WRITE, 10);
  if(rc == OK) kprintf ("wrD acquires lock 2, sleep 2s\n");
  else kprintf("unable to get lock.\n");
  sleep (2);
  kprintf ("wrD releases lock 1&2\n");
  releaseall (2, lck1, lck2);
}

void procE()
{
  kprintf ("rdE try to acquire lock 1\n");
  int rc = lock(lck1, READ, 5);
  if(rc == OK) kprintf ("rdE acquires lock 1, sleep 2s\n");
  else kprintf("unable to get lock.\n");
  sleep (2);
  kprintf ("rdE releases lock 1\n");
  releaseall (1, lck1);
}

void procF()
{
  kprintf ("wrF try to acquire lock 2\n");
  int rc = lock(lck2, WRITE, 20);
  if(rc == OK) kprintf ("wrF acquires lock 2, sleep 2s\n");
  else kprintf("unable to get lock.\n");
  sleep (2);
  kprintf ("wrF releases lock 2\n");
  releaseall (1, lck2);
}

int main() 
{
        kprintf("expected output:\n rdA try to acquire lock 1\n rdA acquires lock 1, sleep 2s\n wrB try to acquire lock 2\n wrB acquires lock 2, sleep 2s\n rdC try to acquire lock 1\n rdC acquires lock 1\n rdC try to acquire lock 2\n procD kills procA & procC\n wrD try to acquire lock 1\n wrD acquires lock 1\n wrD try to acquire lock 2\n rdE try to acquire lock 1\n wrF try to acquire lock 2\n wrB releases lock 2\n wrF acquires lock 2, sleep 2s\n wrF releases lock 2\n wrD acquires lock 2, sleep 2s\n wrD releases lock 1&2\n rdE acquires lock 1, sleep 2s\n rdE releases lock 1\n\n\n real output: \n");

      lck1  = lcreate();
      lck2  = lcreate();

      rdA = create(procA, 2000, 20, "rdA", 1, 1);
      rdC = create(procC, 2000, 20, "rdC", 1, 1);
      rdE = create(procE, 2000, 20, "rdE", 1, 1);
      wrB = create(procB, 2000, 20, "wrB", 1, 1);
      wrD = create(procD, 2000, 20, "wrD", 1, 1);
      wrF = create(procF, 2000, 20, "wrF", 1, 1);

      resume(rdA);
      resume(wrB);
      resume(rdC);
      resume(wrD);
      resume(rdE);
      resume(wrF);
}

/* test1.c
 * This test program creates three processes, two readers and a writer at
 * priority 20.  The main process also has priority 20.
 * All readers and writers have the same priority.
 */


/*
void halt();

int reader(char, int, int);
int writer(char, int, int);

int reader1, reader2;
int writer1;
int lck1;

int main()
{
	kprintf("\n\nHello World, Xinu lives\n\n");


    int i;
    lck1 = lcreate();


    kprintf("\n\nTEST1:\n");
    resume(reader1 = create(reader,2000,20,"reader1",3,'A', lck1, 30));
    resume(reader2 = create(reader,2000,20,"reader2",3,'B', lck1, 30));
    kprintf("going to resume write1\n");
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
    kprintf("in %s, going to lcok\n",proctab[currpid].pname);
     lock(lck, READ, prio);
     kprintf("in %s, going to sleep\n",proctab[currpid].pname);
     sleep(5);
     kprintf("**in %s, going to releaseall\n",proctab[currpid].pname);
     releaseall(1, lck);

   }
}

int writer(char c, int lck, int prio){
  int i;

   while(1) {
     kprintf("in %s, writer, going to lock\n",proctab[currpid].pname);
     lock(lck, WRITE, prio);
     kprintf("in %s, writer, going to sleep\n",proctab[currpid].pname);
     sleep(5);
     kprintf("in %s, going to releaseall\n",proctab[currpid].pname);
     releaseall(1, lck);

   }
}
*/
