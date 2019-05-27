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
	int i, j; // 제어변수
	char buf[BUF_SIZE];
	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0); // socket 함수호출을 통해서 소켓을 생성
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) // bind 함수호출을 통해 IP주소와 PORT번호 할당
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1) // listen 함수 호출, 연결요청을 받을 수 있게 됨 (서버소켓이 됨)
		error_handling("listen() error");

	/* 1단계_1 : 파일 디스크립터의 설정 */
	FD_ZERO(&reads); // fd_set의 element를 0으로 초기화
	FD_SET(serv_sock, &reads); // reads에 serv_sock을 등록 (관찰대상에 서버소켓 포함) -> 데이터 존재 == 연결요청 존재
	fd_max = serv_sock;

	while (1)
	{
		/* 원본을 보존하기 위한 처리 */
		cpy_reads = reads; // select함수 종료 시 나머지 비트가 0으로 바뀌므로 원본유지 위해 임시저장

		/* 1단계_3 : 타임아웃의 설정 */
		timeout.tv_sec = 5; // 같은 타임아웃을 주기 위해서 select함수 호출 전에 매번 초기화하는 처리를 수행 
		timeout.tv_usec = 5000; // micro second 단위

		/* 2단계 : select 함수의 호출 */
		if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
			break;

		if (fd_num == 0)
			continue;

		/* 3단계 : 호출결과 확인 */
		for (i = 0; i < fd_max + 1; i++)
		{
			if (FD_ISSET(i, &cpy_reads)) // select 함수가 1이상 반환된 경우 실행 -> 변화된 FD 찾음
			{
				if (i == serv_sock) // 연결요청이 있는 경우 (서버소켓의 변화감지) - 서버자신(fd:3)
				{
					adr_sz = sizeof(clnt_adr);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz); // 서버소켓의 변화맞는 경우 accept 처리
					FD_SET(clnt_sock, &reads); // reads에 clnt_sock을 등록 (관찰대상에 클라이언트소켓 포함)
					if (fd_max < clnt_sock)
						fd_max = clnt_sock;
					printf("connected client: %d \n", clnt_sock);
				}
				else // 상태변화가 발생한 소켓이 서버소켓이 아닌 경우
				{
					str_len = read(i, buf, BUF_SIZE);
					if (str_len == 0) // 연결종료 요청이 있는 경우
					{
						FD_CLR(i, &reads); // reads에서 삭제 (관찰대상에서 제거)
						close(i);
						printf("closed client: %d \n", i);
					}
					else // 서버소켓(fd:3)이 아닌 그 이후의 클라이언트소켓(fd:4~)의 경우 처리
					{
						/* 변경된 부분 */
						// 서버랑 연결된 모든 클라이언트들에게 데이터 보냄
						for (j = 4; j < fd_max + 1; j++) // 4번으로 시작하는 이유는 서버소켓(fd:3)이후부터 처리하기 때문
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