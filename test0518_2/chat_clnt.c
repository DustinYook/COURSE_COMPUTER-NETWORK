/*========================================================================
* File Name: "chat_clnt.c"
* Description:
* - This program is client source code for chatting program
* Programmed by YOOK DONGHYUN
* Last updated: Version 1.0, May 18, 2018 (by YOOK DONGHYUN)
* ========================================================================
* Version Control (Explain updates in detail)
* ========================================================================
* Name YYYY/MM/DD Version Remarks
* Dong-Hyun Yook 2018/05/18 1.0 clnt.c is generated
* ======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 100 // ������ ũ�Ⱑ ������ �� ������ �˳��� ���� (����� �κ�)
#define NAME_SIZE 20 // �̸����̸� 20�ڷ� ���� (�߰��� �κ�)

/* �Լ����� */
void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

/* �������� ���� */
char name[NAME_SIZE] = "[NULL]"; // Ŭ���̾�Ʈ�� �ĺ��ϱ� ���� ������ ���̵� �����ϱ� ���� ����
char buf[BUF_SIZE];

int main(int argc, char *argv[])
{
	// argc�� argument count�� �ǹ��� -> ���α׷��� ���޵� �μ�(argument)�� ������ ����
	// argv�� argument vector�� �ǹ��� -> ���ڿ��� 1���� �迭����, �� ���ڿ��� chunk�� ���α׷����� ���޵� �μ�(argument)�� ����
	// ����) gcc -o c clnt.c A-> �� 5���� ���ڿ� "gcc", "-o", "c", "clnt.c", "A"�� ����, ���� argc�� 5�̴�.
	// ���⼭ argv[0]�� gcc�̰�, argv[1]�� -o, argv[2]�� c, argv[3]�� clnt.c�̸� arg[4]�� A�̴�.
	int sock;
	pid_t pid;

	struct sockaddr_in serv_adr;
	if (argc != 4) // ������ 3���� name�� �߰��Ǿ� 4�� �ٲ� (����� �κ�)
	{
		printf("Usage : %s <IP> <port> <name>\n", argv[0]); // <name>�� �ϳ� �� �߰��� (����� �κ�)
		exit(1);
	}

	/* �߿��� ó�� */
	sprintf(name, "[%s]", argv[3]); // � Ŭ���̾�Ʈ�� ���ϴ� ���� �ľ��ϱ� ���� �κ� (�߰��� �κ�)
	// sprintf(���� �迭�ּ�, ����, ��������) -> �̸� �Է¹ް� �迭�� �־� ��
	// int sprintf(char* str, const char* format, ...); -> function prototype
	// str�� �Է¹��� ����
	// �ǹ��ؼ�: place holder�� %s�� argv[3]���� ��ü ��Ų ���ڿ� "[%s]"�� name�̶�� ���۰������� �����

	sock = socket(PF_INET, SOCK_STREAM, 0); // socket�Լ��� ���� ���� ����  
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	// connect�Լ��� ���� ���� ���α׷��� ���ܿ�û ������ ��� ť�� ��� �� ȣ��Ϸ�
	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!"); // �������� accept ȣ�� ���ϸ� ���� �̷������ ����

	// �Է����� �ʾƵ� �������� ������������ ���
	pid = fork(); // 1) fork�� ����Ͽ� ��ƾ�� ������
	if (pid == 0)
		write_routine(sock, buf); // �Ȱ��� ���� ����
	else
		read_routine(sock, buf);  // �Ȱ��� ���� ����

	close(sock); // close�Լ��� ȣ��Ǹ� �������Ͽ� EOF�� ���۵Ǹ鼭 ������ �����
	return 0;
}

/* �Լ����� */
void read_routine(int sock, char *buf)
{
	char total_msg[NAME_SIZE + BUF_SIZE]; // ��ü �޼�������: �̸��� ���ۻ���� ��ģ ������ �Ҵ� (����� �κ�)
	while (1)
	{
		int str_len = read(sock, total_msg, NAME_SIZE + BUF_SIZE - 1); // sock���� ��ü �޼������̸�ŭ �о� total_msg�� ���� (����� �κ�)
		// ssize_t read(int fd, void* buf, size_t nbyte);
		// read�Լ��� fd�� �ش�Ǵ� �����̳� ��ġ���� nbyte�� �о �̸� ���� buf�� ����
		// read�Լ��� ��û�� ����Ʈ�� ä��� �� EOF�� ���޽� 0�� ������
		// read�Լ��� �����ϸ� �о���� ����Ʈ ���� �����ϰ�, �����ϸ� -1�� �����ϰ� errno�� ����
		if (str_len == 0) // �о���� ���� ���� ���
			return;
		total_msg[str_len] = 0; // str_len-1���� ���ڿ��� �����ϹǷ� str_len�� EOF�� ���� (����� �κ�)
		fputs(total_msg, stdout); // total_msg�� ������ ������ ����Ϳ� ��� (����� �κ�)
	}
}

void write_routine(int sock, char *buf)
{
	char total_msg[NAME_SIZE + BUF_SIZE]; // ��ü �޼�������: �̸��� ���ۻ���� ��ģ ������ �Ҵ� (����� �κ�)
	while (1)
	{
		fgets(buf, BUF_SIZE, stdin);
		if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
		{
			// 2) shutdown�� ù��° ���ڴ� sock���� read_routine�� read�� ù��° ���� sock�� ���� �� -> ��½�Ʈ���� ��
			shutdown(sock, SHUT_WR); // 3) close�� �ƴ� shutdown���� half-close��
			return;
		}

		sprintf(total_msg, "%s %s", name, buf); // place holder %s�� ��ü�� ���ڿ��� total_msg�� ���ۿ� ����� (�߰��� �κ�)
		// sprintf()�� printf()�� ��
		// printf()�� ȭ�鿡 ����� ����ϱ� ���� �Լ�
		// sprintf()�� ���ۿ� ����� ����ϴ� �Լ�, ���ۿ� �ԷµǴ� ���ڿ��� �����ϱ� ������

		write(sock, total_msg, strlen(total_msg)); // ����(total_msg)���̸�ŭ ����(total_msg)���� �о�� sock�� ����Ű�� ������ ��� (����� �κ�)
		// ssize_t write(int fd, const void* buf, size_t nbyte);
		// write�Լ��� nbyte�� ����� ���� buf���� fd�� ����Ű�� ������ ���
		// write�Լ��� �����ϸ� ����� ����Ʈ ���� �����ϰ�, �����ϸ� -1�� �����ϰ� errno�� ����
	}
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}