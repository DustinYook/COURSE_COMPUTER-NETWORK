#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
	fd_set reads, temps;
	int result, str_len;
	char buf[BUF_SIZE];
	struct timeval timeout;

	// 1) 초기화, 관심있는 것 등록
	FD_ZERO(&reads); // 0으로 설정
	FD_SET(0, &reads); // 0 is standard input(console)

	/*
	timeout.tv_sec=5;
	timeout.tv_usec=5000;
	// 이렇게 써 놓으면 초기화가 한 번만 되므로 논리적 오류가 발생함 (우분투의 경우, 윈도우는 아님)
	// 윈도우의 경우 계속 유지되나, 우분투의 경우 줄어든 상태로 복구가 안됨
	*/

	while(1)
	{
		temps=reads; // 원본이 변경되기 때문에 복사본을 반드시 갖고 있어야 함
		
		// 2) 타임아웃 설정
		/* 초기화문이 반드시 있어야 함 - 중요부분 */
		timeout.tv_sec=5; // 시간이 지남 -> 0초가 되면 타임아웃 발생
		timeout.tv_usec=0; 

		result=select(1, &temps, 0, 0, &timeout); 
		// 인자를 잘 기억해야 함! (write, exception은 0으로 하면 됨)
		if(result==-1)
		{
			puts("select() error!");
			break;
		}
		else if(result==0)
		{
			puts("Time-out!");
		}
		else 
		{
			if(FD_ISSET(0, &temps)) 
			{
				str_len=read(0, buf, BUF_SIZE);
				buf[str_len]=0;
				printf("message from console: %s", buf);
			}
		}
	}
	return 0;
}