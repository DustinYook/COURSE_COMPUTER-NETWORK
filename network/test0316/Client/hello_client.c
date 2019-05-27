#include <stdio.h> // scanf, print.. exit IO function
#include <stdlib.h> // atoi func
#include <string.h> // memset func
#include <unistd.h> // read, write, close func
#include <arpa/inet.h> // htons, htonl, ..
#include <sys/socket.h> // socket
void error_handling(char* message);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	char message[30];
	int str_len;

	if (argc != 3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	str_len = read(sock, message, sizeof(message) - 1); // -1이 있는 이유: 마지막에 null문자 넣을 공간 필요하기 때문
	if (str_len == -1)
		error_handling("read() error!");

	printf("Message from server : %s \n", message);
	close(sock);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
