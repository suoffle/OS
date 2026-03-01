# OS

### __설명:__

1. 기존의 proc.c에 forknexec 함수를 추가하는 과제이다. 
  (forknexec(): 자식 프로세스를 생성하고 실행하는 함수이다.)   
2. 실행을 위해 proc.c 파일에 해당 함수를 추가하고 새로운 system call을 등록하였다. 이를 통해 user mode에서 함수를 호출하고 kernel mode로 전환하여 함수가 실행되며, 실행 이후 user mode로 다시 돌아오게 된다.

<br>

#1 "Makefile": 테스트를 위한 userapplication.c 추가  
#2 "proc.c": 기존 proc.c 파일에 forknexec 함수  추가  
#3 "syscall.c": 함수 extern 선언 및 시스템 호출 함수 테이블에 forknexec 추가  
#4 "syscall.h": 새로운 system call forknexec 등록(호출 번호:22)  
#5 "sysfile.c": forknexec 함수 추가   
#6 "defs.h": proc.c 부분에 forknexec 함수 원형 추가  
#7 "user.h": system call 부분에 forknexec 함수 원형 추가  
#8 "usys.S": 시스템 호출을 위해 forknexec 매크로 추가  
#9 "userapplication.c": 테스트 코드 프로그램   
