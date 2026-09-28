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
### __<추가 수정 사항>__
__1) "kalloc.c"__
- kmem 구조체: num_free_pages, pgrefcount를 전역변수 -> kmem 구조체 멤버로 변경.
  - 이유: 물리 페이지를 할당하고 해제하는 과정에서 kmem 구조체에 있는 free list가 같이 수정됨. 따라서 3개의 변수가 동일한 락을 통해 관리되는 것이 맞다고 판단.
- freerange(): 페이지 참조 횟수 초기화는 1로 수정.
  - 이유: 0인 경우를 오류로 처리하기 위해 수정하였음.
- kfree():
  - pgrefcount 값이 0인 경우 -> 오류로 처리.
  - pgrefcount 값이 1 이상인 경우 -> 감소했을 때 0이면 free list에 넣고 num_free_pages 1 증가.
  - kmem.lock 획득 중에 get_refcount() 함수 호출 대신 변수에 직접 접근하도록 수정.
    - 이유: kmem.lock 획득 중에 get_refcount() 함수를 호출하면 동일한 락을 획득하게 됨. (애초에 기본 xv6 spinlock은 중복 획득 불가)
- kalloc(): 기존 num_free_pages-- 위치 수정.
  - 이유: 실제 메모리 할당 이후 free page 수 감소시키는 것이 순서에 맞다고 판단하여 수정.
- get_refcount(), inc_refcount(), dec_refcount(): kmem 구조체에 포함된 lock을 사용해 kmem 멤버 pgrefcount 값 보호.
  - 이유: 공유 물리 페이지는 여러 CPU의 프로세스가 동시 접속이 가능한 상태임으로 락 획득이 필요함.
- get_num_free_pages(): 새로운 커널 내부 함수 등록
  - 전역 변수로 존재하던 num_free_pages가 구조체에 포함되면서 extern으로 참조하던 외부에서 오류 발생 -> 외부에서 호출해서 사용할 수 있는 함수 생성.      
   <br>
__2) "vm.c"__
- copyuvm(): 부모 PTE에서 쓰기 권한을 제거하는 위치를 모든 자식 PTE 매핑 성공 후로 수정.
  - 이유: 매핑 실패 시 부모 PTE 권한을 복구하지 않는 문제 확인.
- pagefault(): COW 처리 시, 'PTE_P | PTE_U | PTE_W' 권한 직접 설정하던 것을 '기존 flag + PTE_W'로 수정.<br><br>   

__3) "defs.h"__
- get_num_free_pages(void) 커널 내부 함수 추가.  <br> <br> 

__4) "sysproc.c"__
- 시스템콜 getNumFreePages 진입 함수를 커널 내부 함수와 연결.
<br>

```
+ 메모

1) xv6는 하나의 프로세스가 하나의 CPU에서만 실행이 가능하기 때문에 pte에 lock을 잡아야 하는 경우 거의 존재하지 않음.
2) 물리 메모리는 여러 프로세스가 동시에 접근할 수 있기 때문에 lock을 통해 관리가 필요함.
3) num_free_pages, pgrefcount에 관해서 kmem과 다른 lock으로 처리하는 경우, lock 순서에 주의 필요.
4) 이미 락이 걸려 있는 상황에서 동일한 락을 잡는 경우를 회피하기 위해 직접적인 변수에 접근함, 아닌 경우는 함수를 통해 락을 잡아 보호.
```

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
