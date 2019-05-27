#include <stdio.h>
#include <pthread.h>
void * thread_summation(void * arg); 

int sum=0; // 공유변수

int main(int argc, char *argv[])
{
	pthread_t id_t1, id_t2; // 쓰레드 2개 생성
	int range1[]={1, 5}; // 전달할 인자1
	int range2[]={6, 10}; // 전달할 인자2
	
	pthread_create(&id_t1, NULL, thread_summation, (void *)range1); // 쓰레드1은 range1을 인자로 받음
	pthread_create(&id_t2, NULL, thread_summation, (void *)range2); // 쓰레드2는 range2를 인자로 받음

	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);
	printf("result: %d \n", sum);
	return 0;
}

void * thread_summation(void * arg) 
{
	int start=((int*)arg)[0]; // 시작점
	int end=((int*)arg)[1]; // 끝점

	while(start<=end)
	{
		sum+=start;
		start++;
	}
	return NULL;
}
