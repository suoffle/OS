# OS

### __요약:__
1. 스케줄러 구현: 프로세스 구조체에 우선순위 변수(priority)를 추가해 기존 스케줄러(scheduler) 함수를 수정하였다.  
2. starvation 현상 확인: 구현한 스케줄러를 통해 우선순위가 낮은 프로세스들에 starvation 현상이 나타나는 것을 확인한다.
3. aging 변수 추가: starvation 현상 방지를 위해 대기 시간을 age 변수에 기록하고 임계값(100) 초과 시 해당 프로세스가 바로 실행될 수 있게 하였다.
---
### __<추가 수정 사항(2026.09.08)>__ 
- userinit: priority와 age 초기화 코드 제거.  
   - 해결: userinit()에서 실행하는 allocproc()에서 prioriy와 age를 동일하게 초기화함으로 중복 작업 제거.
    
- scheduler: age 임계값을 넘은 프로세스 발견 이후의 RUNNABLE 프로세스 age를 증가되지 않는 문제 발견.
   - 원인: break문으로 인해 priority_p 프로세스 저장 후 반복문이 종료되어 age가 증가되지 않은 프로세스들 발생.
   - 해결: break 대신 find_proc flag 사용. find_proc이 1인 경우 aging 프로세스 선택 과정과 우선순위 높은 프로세스 탐색 과정 생략.
   
- scheduler: age 초기화 작업의 부재 확인.
   - 해결: 다음에 실행하고자 하는 프로세스의 상태를 RUNNING으로 전환한 직후에 age를 0으로 초기화.
---
<br>

#1 "proc.c": 
  - userinit(): 프로세스의 priority는 5, age는 0으로 초기화하는 코드를 추가하였다.
  - set_proc_priority(): 프로세스 우선순위 지정 함수를 추가하였다.
  - get_proc_priority(): 프로세스 우선순위 리턴 함수를 추가하였다.
  - get_proc_age(): 프로세스 age 값 리턴 함수를 추가하였다.
    - scheduler가 한 번 수행될 때마다 RUNNABLE 프로세스의 age가 1씩 증가하도록 하였다.
    - age가 임계값(100)을 초과하면 즉시 해당 프로세스를 수행하도록 하였다. 임계값은 몇 가지 값을 시험적으로 적용해 본 결과 100이 적당한 것 같다고 판단해 100을 선택하게 되었다.  

#2 "proc.h": proc 구조체에 priority와 age 변수 추가.   

#3 "syscall.c": sys_set_proc_priority, sys_get_proc_priority, sys_get_proc_age 시스템 콜 선언 및 시스템 콜 번호와 함수 매핑.  

#4 "syscall.h": set_proc_priority, get_proc_priority, get_proc_age의 system call 번호를 각각 23, 24, 25로 설정.  
#5 "sysfile.c": sys_set_proc_priority, sys_get_proc_priority, sys_get_proc_age 함수를 추가.  

#6 "defs.h": set_proc_priority, get_proc_priority, get_proc_age 함수 추가.  

#7 "user.h": set_proc_priority, get_proc_priority, get_proc_age의 함수 원형 정의.  

#8 "usys.S": set_proc_priority, get_proc_priority, get_proc_age에 해당하는 매크로를 추가.  

#9 "userapplication.c": 서로 다른 우선순위의 프로세스 실행 프로그램. 'proc.c'에서 추가한 3가지 함수를 통해 starvation 상황과 starvation 해결 이후 결과를 확인하였다.  

