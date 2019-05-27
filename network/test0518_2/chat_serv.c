/*========================================================================
* File Name: "chat_serv.c"
* Description:
* - This program is server source code for chatting program
* Programmed by YOOK DONGHYUN
* Last updated: Version 1.0, May 18, 2018 (by YOOK DONGHYUN)
* ========================================================================
* Version Control (Explain updates in detail)
* ========================================================================
* Name YYYY/MM/DD Version Remarks
* Dong-Hyun Yook 2018/05/18 1.0 chat_serv.c is generated
* ======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 100

void error_handling(char *buf);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;

	socklen_t adr_sz;
	int fd_max, str_len, fd_num;
	int i, j; // �����
	char buf[BUF_SIZE];
	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0); // socket �Լ�ȣ���� ���ؼ� ������ ����
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) // bind �Լ�ȣ���� ���� IP�ּҿ� PORT��ȣ �Ҵ�
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1) // listen �Լ� ȣ��, �����û�� ���� �� �ְ� �� (���������� ��)
		error_handling("listen() error");

	/* 1�ܰ�_1 : ���� ��ũ������ ���� */
	FD_ZERO(&reads); // fd_set�� element�� 0���� �ʱ�ȭ
	FD_SET(serv_sock, &reads); // reads�� serv_sock�� ��� (������� �������� ����) -> ������ ���� == �����û ����
	fd_max = serv_sock;

	while (1)
	{
		/* ������ �����ϱ� ���� ó�� */
		cpy_reads = reads; // select�Լ� ���� �� ������ ��Ʈ�� 0���� �ٲ�Ƿ� �������� ���� �ӽ�����

		/* 1�ܰ�_3 : Ÿ�Ӿƿ��� ���� */
		timeout.tv_sec = 5; // ���� Ÿ�Ӿƿ��� �ֱ� ���ؼ� select�Լ� ȣ�� ���� �Ź� �ʱ�ȭ�ϴ� ó���� ���� 
		timeout.tv_usec = 5000; // micro second ����

		/* 2�ܰ� : select �Լ��� ȣ�� */
		if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
			break;

		if (fd_num == 0)
			continue;

		/* 3�ܰ� : ȣ���� Ȯ�� */
		for (i = 0; i < fd_max + 1; i++)
		{
			if (FD_ISSET(i, &cpy_reads)) // select �Լ��� 1�̻� ��ȯ�� ��� ���� -> ��ȭ�� FD ã��
			{
				if (i == serv_sock) // �����û�� �ִ� ��� (���������� ��ȭ����) - �����ڽ�(fd:3)
				{
					adr_sz = sizeof(clnt_adr);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz); // ���������� ��ȭ�´� ��� accept ó��
					FD_SET(clnt_sock, &reads); // reads�� clnt_sock�� ��� (������� Ŭ���̾�Ʈ���� ����)
					if (fd_max < clnt_sock)
						fd_max = clnt_sock;
					printf("connected client: %d \n", clnt_sock);
				}
				else // ���º�ȭ�� �߻��� ������ ���������� �ƴ� ���
				{
					str_len = read(i, buf, BUF_SIZE);
					if (str_len == 0) // �������� ��û�� �ִ� ���
					{
						FD_CLR(i, &reads); // reads���� ���� (������󿡼� ����)
						close(i);
						printf("closed client: %d \n", i);
					}
					else // ��������(fd:3)�� �ƴ� �� ������ Ŭ���̾�Ʈ����(fd:4~)�� ��� ó��
					{
						/* ����� �κ� */
						// ������ ����� ��� Ŭ���̾�Ʈ�鿡�� ������ ����
						for (j = 4; j < fd_max + 1; j++) // 4������ �����ϴ� ������ ��������(fd:3)���ĺ��� ó���ϱ� ����
							write(j, buf, str_len);
					}
						
				}
			}
		}
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}