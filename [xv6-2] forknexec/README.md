# OS

### __설명:__

1. 기존의 proc.c에 forknexec 함수를 추가하였다. 
  (forknexec(): 자식 프로세스를 생성하고 즉시 실행하는 함수)   
2. 실행을 위해 proc.c 파일에 해당 함수를 추가하고 새로운 system call을 등록하였다. 이를 통해 user mode에서 함수를 호출하고 kernel mode로 전환하여 함수가 실행되며, 실행 이후 user mode로 다시 돌아오게 된다.

<br>

#1 "Makefile": 테스트를 위한 userapplication.c 추가  

#2 "proc.c": 기존 proc.c 파일에 forknexec 함수 추가
   - **forknexec(): fork()와 exec() 코드를 합쳤다. 인자는 (실행할 프로그램 path, argv)이다.
     - **fork()**  
        - 부모 프로세스의 메모리, 상태를 자식 프로세스로 복사하는 코드이다.
        - 해당 함수의 마지막에 프로세스를 RUNNABLE로 전환하는 코드는 주석으로 처리하였다.
          (아직 즉시 실행 가능한 상태의 프로세스가 아니기 때문이다.)
     - **exec()**  
        - elf program header를 읽고 필요한 segment를 Virtual memory에 load한다.
        - user stack 초기 구조(return address, argc, args pointer, args)를 구성한다.
        - 기존 프로세스 메모리를 백업하고 현재 프로세스로 교체한다.
        - 현재 프로세스의 상태를 RUNNABLE로 전환하여 실행한다.
        - 실행된 프로세스는 userapplication에서 exit()을 통해 종료된다.
        - zombie 프로세스가 발생하는 것을 해결하기 위해 코드 마지막에 wait()을 추가하였다.

#3 "syscall.c": 함수 extern 선언 및 시스템 호출 함수 테이블에 forknexec 추가  

#4 "syscall.h": 새로운 system call forknexec 등록(호출 번호:22)  

#5 "sysfile.c": forknexec 함수 추가   

#6 "defs.h": proc.c 부분에 forknexec 함수 원형 추가  

#7 "user.h": system call 부분에 forknexec 함수 원형 추가  

#8 "usys.S": 시스템 호출을 위해 forknexec 매크로 추가  

#9 "userapplication.c": 테스트 코드 프로그램   
  - args 배열의 마지막 인자를 NULL로 설정하고 3-32번을 '.'으로 처리한 것은 forknexec에서 인자 개수(MAXARG)를 제대로 계산하는지 확인하기 위함이다.
  1) 테스트 1: echo 실행 코드이다. "NEWPROC_CALL" 문자열을 출력하고 자식 프로세스의 pid를 출력한다.
  2) 테스트 2: cat 실행 코드이다. 'abc.txt' 파일에 있는 내용을 출력하고 자식 프로세스의 pid를 출력한다.
  3) 테스트 3: 첫 번째 인자(실행 프로그램 path)가 잘못된 경우 -1 값을 리턴한다.
  4) 테스트 4: args 배열 마지막에 NULL이 없는 경우는 -1 값을 출력한다.

