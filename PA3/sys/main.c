#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>


#define DEFAULT_LOCK_PRIO 20
extern  struct  pentry proctab[];
#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }
int mystrncmp(char* des,char* target,int n){
    int i;
    for (i=0;i<n;i++){
        if (target[i] == '.') continue;
        if (des[i] != target[i]) return 1;
    }
    return 0;

}
/*--------------------------------Test 1--------------------------------*/

int testval = 1;
void reader1 (char *msg, int lck)
{
    lock (lck, READ, DEFAULT_LOCK_PRIO);
    testval = testval*10;
    sleep (3);
    testval = testval - 5;
    releaseall (1, lck);
    kprintf("\n\n\n\n\n******************************************\n\n");
}

void test1 ()
{
    int lck;
    int pid1;
    int pid2;

    kprintf("\nTest 1: readers can share the rwlock.\n");
    testval = 1;
    lck  = lcreate ();
    assert (lck != SYSERR,"Test 1 FAILED\n");

    pid1 = create(reader1, 2000, 20, "reader a", 2, "reader a", lck);
    pid2 = create(reader1, 2000, 20, "reader b", 2, "reader b", lck);

    resume(pid1);
    sleep(1);
    resume(pid2);
    
    kprintf("\n\n\n\n\ngoing to sleep(5) in %s\n\n\n\n\n\n\n\n",proctab[currpid].pname);
    sleep (5);
    kprintf("\n\n\n\n\n\n\n\n\ngoing to ldelete lock[%d] in %s\n\n\n\n\n\n\n",lck,proctab[currpid].pname);
    ldelete (lck);
    kprintf("going to kill proc[%d] %s\n",pid1,proctab[pid1].pname);
    kill(pid1);
    kprintf("going to kill proc[%d] %s\n",pid2,proctab[pid2].pname);
    kill(pid2);
    assert (testval == 90,"Test 1 FAILED\n");
    kprintf ("Test 1 PASSED!\n");

    //kprintf ("testval: %d \n", testval);
}


/*----------------------------------Test 2---------------------------*/
void reader2 (char *msg, int lck)
{
        lock (lck, READ, DEFAULT_LOCK_PRIO);
        testval = testval*10;
        sleep (3);
        testval = testval-10;
        releaseall (1, lck);
}

void writer2 (char *msg, int lck)
{
    lock (lck, WRITE, DEFAULT_LOCK_PRIO);
    testval = testval*10;
    sleep (3);
    testval = testval-5;
    releaseall (1, lck);
}

void test2 ()
{
        int     lck;
        int     pid1;
        int     pid2;
        testval = 1;
        kprintf("\nTest 2: write lock is mutual exclusive\n");
        lck  = lcreate ();
        assert (lck != SYSERR,"Test 2 FAILED\n");

        pid1 = create(writer2, 2000, 20, "writer2", 2, "writer", lck);
        pid2 = create(reader2, 2000, 20, "reader2", 2, "reader", lck);

        resume(pid1);
        sleep (2);
        resume(pid2);

        sleep (8);
        ldelete (lck);
        kill(pid1);
        kill(pid2);

        assert (testval == 40,"Test 2 FAILED\n");
        kprintf ("Test 2 PASSED!\n");

        //kprintf ("testval: %d \n", testval);
}

/*-----------------------------------Test 3---------------------------*/
void test3 ()
{
        int     lck[NLOCKS];
        int last_lck;
        int     index;


        kprintf("\nTest 3: return SYSERR if no lock is available\n");
    
        for (index = 0; index < NLOCKS; index++) {
            lck[index] = lcreate ();
            assert (lck[index] != SYSERR,"Test 3 FAILED\n");
        }

        last_lck  = lcreate ();
        assert (last_lck == SYSERR,"Test 3 FAILED\n");

        for (index = 0; index < NLOCKS; index++) {
                ldelete (lck[index]);
        }        

        kprintf ("Test 3 PASSED!\n");
}


/*----------------------------------Test 4---------------------------*/
void reader4 (char *msg, int lck)
{
    int ret;

    ret = lock (lck, READ, DEFAULT_LOCK_PRIO);
    assert (ret == DELETED,"Test 4 FAILED\n");
    kprintf ("Test 4 PASSED!\n");
}

void writer4 (char *msg, int lck)
{
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        sleep (5);
        releaseall (1, lck);

}

void test4 ()
{
        int     lck;
        int     pid1;
        int     pid2;

        kprintf("\nTest 4: return DELETED if lock deleted while waiting\n");
        lck  = lcreate ();
        assert (lck != SYSERR,"Test 4 FAILED\n");

        pid1 = create(writer4, 2000, 20, "writer4", 2, "writer", lck);
        pid2 = create(reader4, 2000, 20, "reader4", 2, "reader", lck);

        resume(pid1);
        sleep (1);

        resume(pid2);
        sleep(1);
        ldelete (lck);

        sleep (5);
        kill(pid1);
        kill(pid2);
}

/*-----------------------------------Test 5---------------------------*/
void test5 ()
{
        int     lck[5];
        int     index;
        int ret;

        kprintf("\nTest 5: release multiple locks simultaneously\n");

        for (index = 0; index < 5; index++) {
                lck[index] = lcreate ();
                assert (lck[index] != SYSERR,"Test 5 FAILED\n");

                ret = lock (lck[index], READ, DEFAULT_LOCK_PRIO);
                assert (ret == OK,"Test 5 FAILED\n");
        }

        ret = releaseall (2, lck[4], lck[0]);
        assert (ret == OK,"Test 5 FAILED\n");

        ret = releaseall (3, lck[1], lck[3], lck[2]);
        assert (ret == OK,"Test 5 FAILED\n");

        for (index = 0; index < 5; index++) {
            kprintf("%d~~~~~~~~~~~~going to delete lock[%d]\n",index,lck[index]);
            ldelete (lck[index]);
        }

        kprintf ("Test 5 PASSED!\n");
}

/*----------------------------------Test 6---------------------------*/
char output7[10];
int count7;
void reader6 (char i, int lck, int lprio)
{
        int     ret;

        //kprintf ("  %c: to acquire lock\n", i);
        lock (lck, READ, lprio);
        output7[count7++]=i;
        //kprintf ("  %c: acquired lock, sleep 3s\n", i);
        sleep (3);
        //kprintf ("  %c: to release lock\n", i);
        output7[count7++]=i;
        releaseall (1, lck);
        
}

void writer6 (char i, int lck, int lprio)
{
        //kprintf ("  %c: to acquire lock\n", i);
        lock (lck, WRITE, lprio);
        output7[count7++]=i;
        //kprintf ("  %c: acquired lock, sleep 3s\n", i);
        sleep (3);
        //kprintf ("  %c: to release lock\n", i);
        output7[count7++]=i;
        releaseall (1, lck);
        
}

void test6 ()
{
        int     lck;
        int     rd1, rd2, rd3, rd4;
        int     wr1;

        count7 = 0;
        kprintf("\nTest 6: wait on locks with priority. Expected order of "
        "lock acquisition is: reader A, reader B, reader C, writer E, reader D\n");
        lck  = lcreate ();
        assert (lck != SYSERR,"Test 6 FAILED\n");

        rd1 = create(reader6, 2000, 20, "reader6", 3, 'A', lck, 20);
        rd2 = create(reader6, 2000, 20, "reader6", 3, 'B', lck, 30);
        rd3 = create(reader6, 2000, 20, "reader6", 3, 'C', lck, 40);
        rd4 = create(reader6, 2000, 20, "reader6", 3, 'D', lck, 20);
        wr1 = create(writer6, 2000, 20, "writer6", 3, 'E', lck, 25);
    
        //kprintf("-start reader A, then sleep 1s. lock granted to reader A\n");
        resume(rd1);
        sleep (1);

        //kprintf("-start writer C, then sleep 1s. writer waits for the lock\n");
        resume(wr1);
        sleep (1);

        //kprintf("-start reader B, D, E. reader B is granted lock.\n");
        resume (rd2);
        sleep10(1);
        resume (rd3);
        sleep10(1);
        resume (rd4);


        sleep (10);
        ldelete (lck);
        kill(rd1);kill(rd2);kill(rd3);kill(rd4);kill(wr1);
        kprintf("Output is %s\n",output7);

}


int main( )
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
}
