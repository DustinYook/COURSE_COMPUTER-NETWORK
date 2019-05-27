#include <stdio.h>
#include <pthread.h>
void * thread_summation(void * arg); 

int sum=0; // ��������

int main(int argc, char *argv[])
{
	pthread_t id_t1, id_t2; // ������ 2�� ����
	int range1[]={1, 5}; // ������ ����1
	int range2[]={6, 10}; // ������ ����2
	
	pthread_create(&id_t1, NULL, thread_summation, (void *)range1); // ������1�� range1�� ���ڷ� ����
	pthread_create(&id_t2, NULL, thread_summation, (void *)range2); // ������2�� range2�� ���ڷ� ����

	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);
	printf("result: %d \n", sum);
	return 0;
}

void * thread_summation(void * arg) 
{
	int start=((int*)arg)[0]; // ������
	int end=((int*)arg)[1]; // ����

	while(start<=end)
	{
		sum+=start;
		start++;
	}
	return NULL;
}
