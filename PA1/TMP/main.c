#include <conf.h>
#include <kernel.h>
#include <sched.h>
#include <stdio.h>
#include <proc.h>
#include <q.h>

//#include "lab1.h"

#define LOOP 50

int prA, prB, prC, prD;
int proc_a(), proc_b(), proc_c(), proc_d();
int procsleep(char c);
int procsleepmore(char c);
int proclong(char c);
int proccreate(char c);
int procchprio(char c);
int proc(char c);
int proc_basic(char c);
volatile int a_cnt = 0;
volatile int b_cnt = 0;
volatile int c_cnt = 0;
volatile int d_cnt = 0;

int main() {
    srand(1);
    int i;
    int count = 0;
    char buf[8];
    double total_cnt;
    double a_percent, b_percent, c_percent, d_percent;

    kprintf("\n\n########## Test Case1, linux-like scheduling basic(2 processes):\n");
    setschedclass(LINUXSCHED);
    resume(prC = create(proc, 2000, 90, "proc C", 1, 'C'));
    while (count++ < LOOP) {
        kprintf("M");

        for (i = 0; i < 10000000; i++);
    }
    kill(prC);

    kprintf("\n\n########## Test Case2, linux-like scheduling basic(4 processes):\n");
    count=0;
    resume(prA = create(proc, 2000, 5, "proc A", 1, 'A'));
    resume(prB = create(proc, 2000, 50, "proc B", 1, 'B'));
    resume(prC = create(proc, 2000, 100, "proc C", 1, 'C'));
    while (count++ < LOOP) {
        kprintf("M");
        for (i = 0; i < 10000000; i++);
    }
    kill(prA);
    kill(prB);
    kill(prC);

    kprintf("\n\n########## Test Case3, linux-like scheduling with priority 15 and 25 (3 processes):\n");
    count = 0;
    setschedclass(LINUXSCHED);
    resume(prA = create(proc, 2000, 15, "proc A", 1, 'A'));
    resume(prB = create(proc, 2000, 25, "proc B", 1, 'B'));
    while (count++ < LOOP) {
        kprintf("M");
    for (i = 0; i < 10000000; i++);
    }
    kill(prA);
    kill(prB);

    kprintf("\n\n########## Test Case4, linux-like scheduling with quantum 8 (3 processes):\n");
    count = 0;
    setschedclass(LINUXSCHED);
    resume(prA = create(proc, 2000, 8, "proc A", 1, 'A'));
    resume(prB = create(proc, 2000, 8, "proc B", 1, 'B'));
    while (count++ < LOOP) {
    kprintf("M");

        for (i = 0; i < 10000000; i++);
    }
    kill(prA);
    kill(prB);

    kprintf("\n\n########## Test Case5, linux-like scheduling with quantum 100 (3 processes):\n");
    count = 0;
    setschedclass(LINUXSCHED);
    resume(prA = create(proc, 2000, 100, "proc A", 1, 'A'));
    resume(prB = create(proc, 2000, 100, "proc B", 1, 'B'));
    while (count++ < LOOP) {
    kprintf("M");
        
        for (i = 0; i < 10000000; i++);
    }
    kill(prA);
    kill(prB);

    kprintf("\n\n########## Test Case6, linux-like scheduling with quantum 1000 (3 processes):\n");
    count = 0;
    setschedclass(LINUXSCHED);
    resume(prA = create(proc, 2000, 1000, "proc A", 1, 'A'));
    resume(prB = create(proc, 2000, 1000, "proc B", 1, 'B'));
    while (count++ < LOOP) {
    kprintf("M");
        
        for (i = 0; i < 10000000; i++);
    }
    kill(prA);
    kill(prB);

    count=0;
    kprintf("\n\n########## Test Case7, linux-like scheduling with big quantum(3 processes):\n");
    setschedclass(LINUXSCHED);
    resume(prA = create(proc, 2000, 1000000000, "proc A", 1, 'A'));
    resume(prB = create(procsleep, 2000, 1000000000, "proc B", 1, 'B'));
    while (count++ < LOOP) {
        kprintf("M");

        for (i = 0; i < 10000000; i++);
    }
    kill(prA);
    kill(prB);

    count=0;
    kprintf("\n\n########## Test Case8, linux-like scheduling with chprio(3 processes):\n");
    setschedclass(LINUXSCHED);
    resume(prA = create(proc, 2000, 100, "proc A", 1, 'A'));
    resume(prB = create(procchprio, 2000, 100, "proc B", 1, 'B'));
    while (count++ < LOOP) {
        kprintf("M");

        for (i = 0; i < 10000000; i++);
    }
    kill(prA);
    kill(prB);

    count=0;
    kprintf("\n\n########## Test Case9, linux-like scheduling with create(3 processes):\n");
    setschedclass(LINUXSCHED);
    resume(prA = create(proc, 2000, 200, "proc A", 1, 'A'));
    resume(prB = create(proccreate, 2000, 200, "proc B", 1, 'B'));
    while (count++ < LOOP) {
        kprintf("M");

        for (i = 0; i < 10000000; i++);
    }
    sleep(5);
    kill(prA);
    kill(prB);
    
    count=0;
    kprintf("\n\n########## Test Case10, linux-like scheduling with sleep across epoch(3 processes):\n");
    setschedclass(LINUXSCHED);
    resume(prA = create(proclong, 2000, 200, "proc A", 1, 'A'));
    resume(prB = create(procsleepmore, 2000, 200, "proc B", 1, 'B'));
    while (count++ < LOOP*2) {
        kprintf("M");

        for (i = 0; i < 10000000; i++);
    }
    sleep(5);
    kill(prA);
    kill(prB);

    /*
    kprintf("\n\n########## Test Case11, multi-q scheduling(3 processes):\n");
    setschedclass(MULTIQSCHED);
    total_cnt=0;
    a_percent=0;
    b_percent=0;
    prA = create(proc_a, 2000, 100, "proc A", 1, 'A');
    prB = createReal(proc_b, 2000, 10, "proc B", 1, 'B');
    resume(prA);
    resume(prB);
    sleep(5);
    kill(prA);
    kill(prB);
    total_cnt = a_cnt + b_cnt ;
    a_percent = (double) a_cnt / total_cnt * 100;
    b_percent = (double) b_cnt / total_cnt * 100;
    kprintf("Test RESULT: A = %d, B = %d (%d : %d)\n", a_cnt,
            b_cnt, (int) a_percent, (int) b_percent);


    kprintf("\n\n########## Test Case12, multi-q scheduling(4 processes):\n");
    total_cnt=0;
    a_percent=0;
    b_percent=0;
    c_percent=0;
    a_cnt=b_cnt=c_cnt=0;
    prA = create(proc_a, 2000, 100, "proc A", 1, 'A');
    prB = createReal(proc_b, 2000, 10, "proc B", 1, 'B');
    prC = createReal(proc_c, 2000, 10, "proc C", 1, 'C');
    resume(prA);
    resume(prB);
    resume(prC);
    sleep(10);
    kill(prA);
    kill(prB);
    kill(prC);
    total_cnt = a_cnt + b_cnt + c_cnt;
    a_percent = (double) a_cnt / total_cnt * 100;
    b_percent = (double) b_cnt / total_cnt * 100;
    c_percent = (double) c_cnt / total_cnt * 100;
    kprintf("Test RESULT: A = %d, B = %d, C = %d (%d : %d : %d)\n", a_cnt,
            b_cnt, c_cnt, (int) a_percent, (int) b_percent, (int) c_percent);

    kprintf("\n\n########## Test Case13, multi-q scheduling without real proc(3 processes):\n");
    setschedclass(MULTIQSCHED);
    total_cnt=0;
    a_percent=0;
    b_percent=0;
    a_cnt=0;
    b_cnt=0;
    prA = create(proc_a, 2000, 100, "proc A", 1, 'A');
    prB = create(proc_b, 2000, 100, "proc B", 1, 'B');
    resume(prA);
    resume(prB);
    sleep(5);
    kill(prA);
    kill(prB);
    total_cnt = a_cnt + b_cnt ;
    a_percent = (double) a_cnt / total_cnt * 100;
    b_percent = (double) b_cnt / total_cnt * 100;
    kprintf("Test RESULT: A = %d, B = %d (%d : %d)\n", a_cnt,
            b_cnt, (int) a_percent, (int) b_percent);
    */
}

procchprio(char c) {
    int i;
    int count = 0;
    while (count++ < LOOP/6) {
        kprintf("%c", c);
        for (i = 0; i < 10000000; i++);
    }
    chprio(prB,200);
    kprintf("X");
    while (count++ < LOOP) {
        kprintf("%c", c);
        for (i = 0; i < 10000000; i++);
    }
}

procsleep(char c) {
    int i;
    int count = 0;
    while (count++ < LOOP/3) {
        kprintf("%c", c);
        for (i = 0; i < 10000000; i++);
    }
    kprintf("X");
    sleep10(1);
    kprintf("X");
    while (count++ < LOOP) {
        kprintf("%c", c);
        for (i = 0; i < 10000000; i++);
    }
}

procsleepmore(char c) {
    int i;
    int count = 0;
    while (count++ < LOOP*2/6) {
        kprintf("%c", c);
        for (i = 0; i < 10000000; i++);
    }
    kprintf("X");
    sleep10(2);
    kprintf("X");
    while (count++ < LOOP*2) {
        kprintf("%c", c);
        for (i = 0; i < 10000000; i++);
    }
}

proccreate(char c) {
    int i;
    int count = 0;

    while (count++ < LOOP/3) {
        kprintf("B");
        for (i = 0; i < 10000000; i++);
    }
    resume(prC = create(proc, 2000, 300, "proc C", 1, 'C'));
    kprintf("X");
    while (count++ < LOOP) {
        kprintf("B");
        for (i = 0; i < 10000000; i++);
    }
    kill(prC);
}
proc_basic(char c) {
    int count = 0;

    while (count++ < 1000) {
        kprintf("%c", c);
    }
}

proc_a(c)
    char c; {
    int i;

    while (1) {
        for (i = 0; i < 10000; i++)
            ;
        a_cnt++;
    }
}

proc_b(c)
    char c; {
    int i;

    while (1) {
        for (i = 0; i < 10000; i++)
            ;
        b_cnt++;
    }
}

proc_c(c)
    char c; {
    int i;

    while (1) {
        for (i = 0; i < 10000; i++)
            ;
        c_cnt++;
    }
}

proc_d(c)
    char c; {
    int i;

    while (1) {
        for (i = 0; i < 10000; i++)
            ;
        d_cnt++;
    }
}
proc(char c) {
    int i;
    int count = 0;

    while (count++ < LOOP) {
        kprintf("%c", c);
        for (i = 0; i < 10000000; i++);
    }
}
proclong(char c) {
    int i;
    int count = 0;

    while (count++ < LOOP*2) {
        kprintf("%c", c);
        for (i = 0; i < 10000000; i++);
    }
}
