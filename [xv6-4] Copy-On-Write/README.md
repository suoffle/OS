# OS

### __요약:__


#1 "kalloc.c": free page의 개수(num_free_pages)와 각 물리 주소에 해당하는 메모리 참조 횟수(pgrefcount)를 선언 및 초기화  
	<kalloc> -> num_free_pages를 1 감소하고, pgrefcount 값 초기화한다.   
	<kfree> -> 함수에서는 참조 횟수에 따라 0보다 크면, pgrefcount 값 1 감소, 	참조 횟수가 0이면, num_free_pages 1 증가하였다.    
	<get_refcount> -> 해당  pgrefcount값 리턴.  
	<inc_refcount> -> 해당 pgrefcount값 1 증가.  
	<dec_refcount> -> 해당 pgrefcount값 1 감소.  
#2 "vm.c":   
	<copyuvm> -> COW을 위해 쓰기 권한 설정 및 pgrefcount 증가.  
	<pagefault> -> write가 발생하였을 시 발생한 page fault 처리 함수. 참조 		횟수에 따라 write 권한만 설정할지와 새로운 페이지를 할당할 여부 판단 및 수행.  
#3 "syscall.c": sys_getNumFreePages 함수 추가 및 [SYS_getNumFree Pages] sys_getNumFreePages 선언으로 새로운 시스템 콜 선언.  
#4 "syscall.h": getNumFreePages의 시스템 콜 번호를 26으로 정의.  
#5 "defs.h": "kalloc.c" 파일 내부에 추가한 함수(get_refcount, inc_refcount, dec_refcount)를 kalloc.c 부분 마지막에 선언.  
#6: "user.h": getNumFreePages에 해당하는 매크로 추가.  
#7: "usys.S": getNumFreePages에 해당하는 매크로 추가.  
#8: "trap.c": page fault 발생 시 pagefault 함수 수행.  
#9: "userapplication": 테스트 프로그램. fork를 통해 자식 프로세스가 읽기 작업만 수행한 경우와 쓰기 작업을 수행한 경우 테스트.  
