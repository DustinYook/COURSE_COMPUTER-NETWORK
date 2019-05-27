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

	// 1) �ʱ�ȭ, �����ִ� �� ���
	FD_ZERO(&reads); // 0���� ����
	FD_SET(0, &reads); // 0 is standard input(console)

	/*
	timeout.tv_sec=5;
	timeout.tv_usec=5000;
	// �̷��� �� ������ �ʱ�ȭ�� �� ���� �ǹǷ� ������ ������ �߻��� (������� ���, ������� �ƴ�)
	// �������� ��� ��� �����ǳ�, ������� ��� �پ�� ���·� ������ �ȵ�
	*/

	while(1)
	{
		temps=reads; // ������ ����Ǳ� ������ ���纻�� �ݵ�� ���� �־�� ��
		
		// 2) Ÿ�Ӿƿ� ����
		/* �ʱ�ȭ���� �ݵ�� �־�� �� - �߿�κ� */
		timeout.tv_sec=5; // �ð��� ���� -> 0�ʰ� �Ǹ� Ÿ�Ӿƿ� �߻�
		timeout.tv_usec=0; 

		result=select(1, &temps, 0, 0, &timeout); 
		// ���ڸ� �� ����ؾ� ��! (write, exception�� 0���� �ϸ� ��)
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