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

	/* 여기부분 추가적으로 작성 */
	fd_set reads, temps;
	int result, str_len;
	struct timeval timeout;

	FD_ZERO(&reads); // 전체를 0으로 초기화
	FD_SET(0, &reads); // fd가 0이면 표준입력을 의미함

	while (1)
	{
		temps = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		result = select(1, &temps, 0, 0, &timeout);
		if (result == -1)
		{
			puts("select() error!\n");
			break;
		}
		else if (result == 0)
			puts("Time-out!\n");
		else
		{
			if (FD_ISSET(0, &temps))
			{
				str_len = read(0, buf, BUF_SIZE);
				buf[str_len] = 0;
				printf("message from console: %s", buf);
			}
		}
	}
	
	if (argc != 3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");

	close(sock);
	return 0;
}

// 반복문으로 select함수를 이용하여 
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