// 시험에 100% 출제 - 직접 따라처보기
#include <stdio.h>
#include <unistd.h>
int gval=10;

int main(int argc, char *argv[])
{
	pid_t pid;
	int lval=20;
	gval++, lval+=5;
	
	pid=fork(); // 프로세스가 2개 생성
	if(pid==0)	// if Child Process 
		gval+=2, lval+=2; // 자식 프로세스의 pid는 0이라서 이 부분을 수행 ##
	else			// if Parent Process
		gval-=2, lval-=2; // 부모 프로세스의 pid는 0이 아니라서 이 부분을 수행 ##
	
	if(pid==0)
		printf("Child Proc: [%d, %d] \n", gval, lval); // +2, +2
	else
		printf("Parent Proc: [%d, %d] \n", gval, lval); // -2, -2
	return 0;
}
// 서버가 accept을 해서 
// clnt_sock = accept(); // 파일 디스크립터 받아옴
// 문지기와 처리를 다른 프로세스로 나눠놓으란 의미 -> 한꺼번에 처리할 수 있게 만들자
// 문지기를 자식프로세스로, 처리 프로세스를 부모프로세스로 설정하자는 의미인듯