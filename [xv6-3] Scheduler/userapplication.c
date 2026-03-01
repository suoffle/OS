#include "types.h"
#include "stat.h"
#include "user.h"
/*
//starvation 관측
void child_process(int priority){
    int pid = getpid();
    set_proc_priority(pid, priority);
    printf(1, "pid: %d, priority: %d started.\n", pid, priority);

//busy-wait 수행
    for(volatile int i = 0; i < 10000000; i++);

    printf(1,"pid: %d, priority: %d finished.\n", pid, priority);
    exit();
}
*/

//starvation 해결 관측
void child_process(int priority){
    int pid = getpid();
    set_proc_priority(pid, priority);
    printf(1, "pid: %d, priority: %d started, age: %d\n", pid, priority, get_proc_age(pid));

//busy-wait 수행
    for(volatile int i = 0; i < 10000000; i++);

    printf(1,"pid: %d, priority: %d finished, age: %d\n", pid, priority, get_proc_age(pid));
    exit();
}


int main(){
    int priorities[12]={10,5,1,2,9,10,2,3,4,1,1,5};
    int pid;

    for(int i = 0; i<10 ; i++){
        pid = fork();
        if (pid < 0){
            printf(1, "Fork failed\n");
            exit();
        }else if(pid == 0){
            child_process(priorities[i%12]);
        }
    }

    for(int i = 0; i<10 ; i++){
        wait();
    }

    printf(1, "All processes finished.\n");
    exit();
}
