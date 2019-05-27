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

	// 여기 부분이 중요! - echo_client.c의 것과 비교해 볼 것!
	pid=fork(); // 1) fork를 사용하여 루틴을 분할함
	if(pid==0)
		write_routine(sock, buf); // 똑같은 인자 받음
	else 
		read_routine(sock, buf); // 똑같은 인자 받음

	close(sock);
	return 0;
}

void read_routine(int sock, char *buf)
{
	while(1)
	{
		int str_len=read(sock, buf, BUF_SIZE); // 여기
		if(str_len==0)
			return;
		else sleep(3); // 일부러 넣은 코드 - fork 안 쓴 클라이언트에서는 3초 동안 데이터 전송이 불가 (read, write가 따로 진행됨)
		// 즉, block 상태로 있는 것이 아니라 데이터전송은 계속 일어남에 유의! - 기존것에도 해서 차이점 비교해볼 것!
		// 입출력 루틴 미분할 시 데이터 전송을 하지 못함, 하지만 이 경우에는 read와 write가 따로 일어남
		// UDP에서는 분할 된 횟수만큼 딱딱딱오나 TCP는 데이터 경계가 없어 보낸 횟수와 받는 횟수가 다를 수 있다.

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
			// 2) shutdown의 첫번째 인자는 sock으로 read_routine의 read의 첫번째 인자 sock과 동일 함(라인 47) -> 출력스트림만 끔
			shutdown(sock, SHUT_WR); // 3) close가 아닌 shutdown으로 half-close함
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