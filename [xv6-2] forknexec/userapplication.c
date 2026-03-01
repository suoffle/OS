#include "types.h"
#include "user.h"
int main(int argc, char *argv[]){
	const char *args[33] = {"echo","NEWPROC_CALL",0};
	for(int i=3;i<33;i++) args[i]=".";

	int ret;
	printf(1,"Test forknexec syscall\n");
	ret=forknexec(args[0],args);
	printf(1,"returned: %d\n",ret);

	//cat file
	args[0]="cat";
	args[1]="abc.txt";
	printf(1,"\ncat abc.txt\n");
	ret=forknexec(args[0],args);
	printf(1,"returned: %d\n",ret);

	//첫 번째 인자가 잘못된 경우
	args[0]="none";
	printf(1,"\nArgument_error_non_exist_file\n");
	ret=forknexec(args[0],args);
	printf(1,"returned: %d\n",ret);

	//마지막에 NULL이 없는 경우
	args[0]="rm",args[1]="notexistfile.c",args[2]="hi";
	printf(1,"\nArgument_error_no_NULL\n");
	ret=forknexec(args[0],args);
	printf(1,"returned: %d\n",ret);

	exit();
}
