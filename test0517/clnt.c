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

	sock = socket(PF_INET, SOCK_STREAM, 0); // socket함수를 통해 소켓 생성
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	// connect함수를 통해 서버 프로그램에 연결요청 서버의 대기 큐에 등록 시 호출 완료 
	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!"); // 서버에서 accept 호출 안하면 서비스 이루어지지 않음
	else
		puts("Connected...........");

	/* ==================================================================================================== */

	fd_set reads; // fd_set의 원본
	fd_set cpy_reads; // fd_set의 복사본
	int result; // select 함수의 반환 결과를 저장
	int str_len;
	struct timeval timeout;

	/* 1단계_1 : 파일 디스크립터의 설정 */
	FD_ZERO(&reads); // fd_set의 element를 0으로 초기화
	FD_SET(0, &reads); // 표준입력의 FD를 1로 설정 -> 표준입력 관리

	while (1)
	{
		/* 원본을 보존하기 위한 처리 */
		cpy_reads = reads; // select함수 종료 시 나머지 비트가 0으로 바뀌므로 원본유지 위해 임시저장
		
		/* 1단계_3 : 타임아웃의 설정 */
		timeout.tv_sec = 5; // 같은 타임아웃을 주기 위해서 select함수 호출 전에 매번 초기화하는 처리를 수행 
		timeout.tv_usec = 5000; // micro second 단위

		/* 2단계 : select 함수의 호출 */
		result = select(1, &cpy_reads, 0, 0, &timeout); // 입력된 값이 있으면 1, 없으면 0을 반환
		
		/* 3단계 : 호출결과 확인 */
		// 예외처리
		if (result == -1) 
		{
			puts("select() error!");
			break;
		}
		// 입력없는 경우
		else if (result == 0) 
			puts("Time-out!");
		// 입력있는 경우
		else 
		{
			str_len = read(sock, buf, BUF_SIZE);
			buf[str_len] = 0;
			printf("Message from server: %s", buf);
		}
	}
	close(sock); // close함수가 호출되면 서버소켓에 EOF(End Of File)이 전송, 즉 연결이 종료
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