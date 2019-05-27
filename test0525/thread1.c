#include <stdio.h>
#include <pthread.h> // gcc 컴파일 시 반드시 -lpthread를 옵션으로 넣어주어야 함
void* thread_main(void *arg);

int main(int argc, char *argv[]) 
{
	pthread_t t_id; // pthread_t는 쓰레드 타입 (_t는 POSIX 표준)
	int thread_param=5; // 쓰레드 수행함수의 인자로 들어감
	
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0) 
	{ // 첫번째는 쓰레드아이디, 세번째는 쓰레드 수행함수, 네번째는 쓰레드 수행함수의 인자
		puts("pthread_create() error");
		return -1;
	}
	sleep(10); // 쓰레드의 종료를 기다려주는 join 대신 써 놓은 것 
	puts("end of main");
	return 0;
}

void* thread_main(void *arg) // 쓰레드가 수행할 함수
{ // 여러개 필요한 경우 배열(동일형)이나 구조체(이질형)을 사용
	int i;
	int cnt=*((int*)arg); // 형변환을 해주어야 함
	for(i=0; i<cnt; i++)
	{
		sleep(1);  
		puts("running thread");	 
	}
	return NULL;
}