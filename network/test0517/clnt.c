/*========================================================================
* File Name: "clnt.c"
* Description:
* - This program is the echo select client program
* Programmed by YOOK DONGHYUN
* Last updated: Version 1.0, May 17, 2018 (by YOOK DONGHYUN)
* ========================================================================
* Version Control (Explain updates in detail)
* ========================================================================
* Name YYYY/MM/DD Version Remarks
* Dong-Hyun Yook 2018/05/17 1.0 clnt.c is generated
* ======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

int main(int argc, char *argv[])
{
	int sock;
	char buf[BUF_SIZE];
	struct sockaddr_in serv_adr;

	if (argc != 3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0); // socket�Լ��� ���� ���� ����
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	// connect�Լ��� ���� ���� ���α׷��� �����û ������ ��� ť�� ��� �� ȣ�� �Ϸ� 
	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!"); // �������� accept ȣ�� ���ϸ� ���� �̷������ ����
	else
		puts("Connected...........");

	/* ==================================================================================================== */

	fd_set reads; // fd_set�� ����
	fd_set cpy_reads; // fd_set�� ���纻
	int result; // select �Լ��� ��ȯ ����� ����
	int str_len;
	struct timeval timeout;

	/* 1�ܰ�_1 : ���� ��ũ������ ���� */
	FD_ZERO(&reads); // fd_set�� element�� 0���� �ʱ�ȭ
	FD_SET(0, &reads); // ǥ���Է��� FD�� 1�� ���� -> ǥ���Է� ����

	while (1)
	{
		/* ������ �����ϱ� ���� ó�� */
		cpy_reads = reads; // select�Լ� ���� �� ������ ��Ʈ�� 0���� �ٲ�Ƿ� �������� ���� �ӽ�����
		
		/* 1�ܰ�_3 : Ÿ�Ӿƿ��� ���� */
		timeout.tv_sec = 5; // ���� Ÿ�Ӿƿ��� �ֱ� ���ؼ� select�Լ� ȣ�� ���� �Ź� �ʱ�ȭ�ϴ� ó���� ���� 
		timeout.tv_usec = 5000; // micro second ����

		/* 2�ܰ� : select �Լ��� ȣ�� */
		result = select(1, &cpy_reads, 0, 0, &timeout); // �Էµ� ���� ������ 1, ������ 0�� ��ȯ
		
		/* 3�ܰ� : ȣ���� Ȯ�� */
		// ����ó��
		if (result == -1) 
		{
			puts("select() error!");
			break;
		}
		// �Է¾��� ���
		else if (result == 0) 
			puts("Time-out!");
		// �Է��ִ� ���
		else 
		{
			str_len = read(sock, buf, BUF_SIZE);
			buf[str_len] = 0;
			printf("Message from server: %s", buf);
		}
	}
	close(sock); // close�Լ��� ȣ��Ǹ� �������Ͽ� EOF(End Of File)�� ����, �� ������ ����
	return 0;
}

void read_routine(int sock, char *buf)
{
	int str_len = read(sock, buf, BUF_SIZE);
	if (str_len == 0)
		return;

	buf[str_len] = 0;
	printf("Message from server: %s", buf);
}

void write_routine(int sock, char *buf)
{
	fgets(buf, BUF_SIZE, stdin);
	if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
	{
		shutdown(sock, SHUT_WR);
		return;
	}
	write(sock, buf, strlen(buf));
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}