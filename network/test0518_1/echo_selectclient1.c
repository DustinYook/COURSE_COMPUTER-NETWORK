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
char buf[BUF_SIZE];

int main(int argc, char *argv[])
{
	int sock;
	int str_len;
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
	int result; // select �Լ� ��ȯ�� ����
	int fd_max; // ������ �˻����� ��Ÿ���� ����
	int i; // �����
	struct timeval timeout; // Ÿ�Ӿƿ� ������ ���� ����ü

	/* 1�ܰ�_1 : ���� ��ũ������ ���� */
	FD_ZERO(&reads); // fd_set�� element�� 0���� �ʱ�ȭ
	FD_SET(0, &reads); // ǥ���Է��� FD�� ����������� ���� (��Ʈ���� 1�� ����)
	FD_SET(sock, &reads); // sock�� FD�� ����������� ���� (��Ʈ���� 1�� ����)

	/* 1�ܰ�_2 : �˻��� ���� ���� */
	fd_max = sock; // select�Լ��� ù��° ���� maxfd�� ������ �˻����� ��Ÿ��

	while (1)
	{
		/* ������ �����ϱ� ���� ó�� */
		cpy_reads = reads; // select�Լ� ���� �� ������ ��Ʈ�� 0���� �ٲ�Ƿ� �������� ���� �ӽ�����

		/* 1�ܰ�_3 : Ÿ�Ӿƿ��� ���� */
		// ���� Ÿ�Ӿƿ��� �ֱ� ���ؼ� select�Լ� ȣ�� ���� �Ź� �ʱ�ȭ�ϴ� ó���� ���� 
		timeout.tv_sec = 5; // second ����
		timeout.tv_usec = 5000; // micro second ����

		/* 2�ܰ� : select �Լ��� ȣ�� */
		result = select(fd_max + 1, &cpy_reads, 0, 0, &timeout); // �Էµ� ���� ������ 1, ������ 0�� ��ȯ

		/* 3�ܰ� : ȣ���� Ȯ�� */
		if (result == -1) // ����ó��
		{
			puts("select() error!");
			break;
		}
		else if (result == 0) // ��ȭ���� ���
			puts("Time-out!");
		else // ��ȭ ������ ���
		{
			for (i = 0; i < fd_max + 1; i++) // ��ȭ������ ���� ã�� ���� for���� ����
			{
				if (FD_ISSET(i, &cpy_reads)) // select �Լ��� 1�̻� ��ȯ�� ��� ���� -> ��ȭ�� FD ã��
				{
					if (FD_ISSET(0, &cpy_reads)) // ǥ���Է��� �ִ� ��� ó��
						write_routine(sock, buf);
					else if(FD_ISSET(sock, &cpy_reads)) // �о���̴� ó��
						read_routine(sock, buf);
				}
			}
		}
	}
	close(sock); // close�Լ��� ȣ��Ǹ� �������Ͽ� EOF(End Of File)�� ����, �� ������ ����
	return 0;
}

void read_routine(int sock, char *buf)
{
	int str_len = read(sock, buf, BUF_SIZE);
	buf[str_len] = 0;
	printf("Message from server: %s", buf);
}

void write_routine(int sock, char *buf)
{
	fgets(buf, BUF_SIZE, stdin);
	if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
	{
		close(sock);
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