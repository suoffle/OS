# OS

### __요약:__

- COW: 공유 메모리를 효율적으로 사용하기 위한 방법으로, fork() 시 부모 프로세스와 자식 프로세스가 동일한 물리 페이지를 read-only로 공유하고, write 발생 시 해당 물리 페이지를 새롭게 할당하는 방법이다.  

- fork() 시 물리 페이지를 즉시 복사하지 않고 부모와 자식 프로세스가 동일한 물리 페이지를 공유하도록 한다. 이때 해당 페이지는 raed-only 상태로 설정한다. 이후 여러 프로세스(2개 이상)가 참조 중인 페이지에서 write가 발생하면 page fault를 발생하여 새로운 물리 페이지를 할당하여 내용을 복사하도록 한다.  

---
- 2개 프로세스가 동시 참조 중인 물리 페이지에 write 발생 전:  
부모 프로세스의 PTE와 자식 프로세스의 PTE가 동일한 물리 페이지들을 참조한다.  

- 2개 프로세스가 동시 참조중인 물리 페이지에 write 발생 후:  
부모 프로세스의 PTE와 자식 프로세스의 PTE가 가리키는 물리 페이지 중 write가 발생한 물리 페이지를 가리키는 주소가 달라진다.   

---

#1 "kalloc.c": free page의 개수(num_free_pages)와 각 물리 주소에 해당하는 메모리 참조 횟수(pgrefcount)를 선언 및 초기화하였다.  
  - kalloc() -> num_free_pages를 1 감소하고 pgrefcount 값을 초기화한다.   
  - kfree() -> 함수에서는 참조 횟수에 따라 0보다 크면 pgrefcount 값 1 감소, 참조 횟수가 0이면 num_free_pages 1 증가하였다.    
  - get_refcount() -> 해당  pgrefcount 값 리턴.  
  - inc_refcount() -> 해당 pgrefcount 값 1 증가.  
  - dec_refcount() -> 해당 pgrefcount 값 1 감소.  

#2 "vm.c":   
  - copyuvm() -> COW을 위해 쓰기 권한 설정 및 pgrefcount 증가.  
  - pagefault() -> write가 발생하였을 시 발생한 page fault 처리 함수. 참조 횟수에 따라 write 권한만 설정할지와 새로운 페이지를 할당할 여부 판단 및 수행.  

#3 "syscall.c": sys_getNumFreePages 함수 추가 및 [SYS_getNumFree Pages] sys_getNumFreePages 선언으로 새로운 시스템 콜 선언.  

#4 "syscall.h": getNumFreePages의 시스템 콜 번호를 26으로 정의.  

#5 "defs.h": "kalloc.c" 파일 내부에 추가한 함수(get_refcount, inc_refcount, dec_refcount)를 kalloc.c 부분 마지막에 선언.  

#6: "user.h": getNumFreePages에 해당하는 매크로 추가.  

#7: "usys.S": getNumFreePages에 해당하는 매크로 추가.  

#8: "trap.c": page fault 발생 시 pagefault 함수 수행.  

#9: "userapplication": 테스트 프로그램. fork를 통해 자식 프로세스가 읽기 작업만 수행한 경우와 쓰기 작업을 수행한 경우를 테스트한다.
