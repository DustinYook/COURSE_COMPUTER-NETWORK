#include <stdio.h>
#include <pthread.h> // gcc ������ �� �ݵ�� -lpthread�� �ɼ����� �־��־�� ��
void* thread_main(void *arg);

int main(int argc, char *argv[]) 
{
	pthread_t t_id; // pthread_t�� ������ Ÿ�� (_t�� POSIX ǥ��)
	int thread_param=5; // ������ �����Լ��� ���ڷ� ��
	
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0) 
	{ // ù��°�� ��������̵�, ����°�� ������ �����Լ�, �׹�°�� ������ �����Լ��� ����
		puts("pthread_create() error");
		return -1;
	}
	sleep(10); // �������� ���Ḧ ��ٷ��ִ� join ��� �� ���� �� 
	puts("end of main");
	return 0;
}

void* thread_main(void *arg) // �����尡 ������ �Լ�
{ // ������ �ʿ��� ��� �迭(������)�̳� ����ü(������)�� ���
	int i;
	int cnt=*((int*)arg); // ����ȯ�� ���־�� ��
	for(i=0; i<cnt; i++)
	{
		sleep(1);  
		puts("running thread");	 
	}
	return NULL;
}