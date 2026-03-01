#include "types.h"
#include "stat.h"
#include "user.h"
/*
//read만 수행할 시
int main(void) {
    
    printf(1, "[start]: getNumFreePages %d\n", getNumFreePages());

    int p;
    if( (p=fork()) < 0){
        printf(1, "fork error\n");
    }
    else if(p > 0){  //parent process
        wait();
        printf(1, "[parent] getNumFreePages: %d\n", getNumFreePages());
    }
    else{ 	//child process
        printf(1, "[child] getNumFreePages: %d\n", getNumFreePages());
    }

    exit();
}
*/

//write를 수행하여 COW 발생 시
char shared_page[10]; 	//shared memory
int main(void) {
    printf(1, "[start]: getNumFreePages = %d\n", getNumFreePages());

    //초기 값 설정
    shared_page[0] = 'A'; // 페이지에 데이터 쓰기
    printf(1, "[parent] initial shared memory: %c\n", shared_page[0]);

    //fork 호출
    int p;
    if( (p=fork()) < 0) {
        printf(1, "fork error\n");
        exit();
    }else if(p > 0) { //부모 프로세스
        wait(); //자식 종료 대기
        printf(1, "[parent] after child exit: getNumFreePages = %d\n", getNumFreePages());
        printf(1, "[parent] check shared memory: %c\n", shared_page[0]); // 데이터 확인
    } else { //자식 프로세스
        printf(1, "[child] before write: getNumFreePages = %d\n", getNumFreePages());
        printf(1, "[child] check shared memory: %c\n", shared_page[0]);

        //COW 작동을 유발하는 쓰기 작업
        shared_page[0] = 'B';
        printf(1, "[child] after write: getNumFreePages = %d\n", getNumFreePages());
        printf(1, "[child] check shared memory: %c\n", shared_page[0]);
    }

    exit();
}
