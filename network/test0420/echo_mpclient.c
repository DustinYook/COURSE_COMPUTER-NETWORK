#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

int main(int argc, char *argv[])
{
	int sock;
	pid_t pid;
	char buf[BUF_SIZE];
	struct sockaddr_in serv_adr;
	if(argc!=3) 
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);  
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");

	// ���� �κ��� �߿�! - echo_client.c�� �Ͱ� ���� �� ��!
	pid=fork(); // 1) fork�� ����Ͽ� ��ƾ�� ������
	if(pid==0)
		write_routine(sock, buf); // �Ȱ��� ���� ����
	else 
		read_routine(sock, buf); // �Ȱ��� ���� ����

	close(sock);
	return 0;
}

void read_routine(int sock, char *buf)
{
	while(1)
	{
		int str_len=read(sock, buf, BUF_SIZE); // ����
		if(str_len==0)
			return;
		else sleep(3); // �Ϻη� ���� �ڵ� - fork �� �� Ŭ���̾�Ʈ������ 3�� ���� ������ ������ �Ұ� (read, write�� ���� �����)
		// ��, block ���·� �ִ� ���� �ƴ϶� ������������ ��� �Ͼ�� ����! - �����Ϳ��� �ؼ� ������ ���غ� ��!
		// ����� ��ƾ �̺��� �� ������ ������ ���� ����, ������ �� ��쿡�� read�� write�� ���� �Ͼ
		// UDP������ ���� �� Ƚ����ŭ ���������� TCP�� ������ ��谡 ���� ���� Ƚ���� �޴� Ƚ���� �ٸ� �� �ִ�.

		buf[str_len]=0;
		printf("Message from server: %s", buf);
	}
}
void write_routine(int sock, char *buf)
{
	while(1)
	{
		fgets(buf, BUF_SIZE, stdin);
		printf("if you want to end, pleas type Q\n");
		if(!strcmp(buf,"q\n") || !strcmp(buf,"Q\n"))
		{	
			// 2) shutdown�� ù��° ���ڴ� sock���� read_routine�� read�� ù��° ���� sock�� ���� ��(���� 47) -> ��½�Ʈ���� ��
			shutdown(sock, SHUT_WR); // 3) close�� �ƴ� shutdown���� half-close��
			return;
		}
		write(sock, buf, strlen(buf));
	}
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}