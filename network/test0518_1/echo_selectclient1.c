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
	int result; // select 함수 반환값 저장
	int fd_max; // 어디까지 검사할지 나타내는 변수
	int i; // 제어변수
	struct timeval timeout; // 타임아웃 설정을 위한 구조체

	/* 1단계_1 : 파일 디스크립터의 설정 */
	FD_ZERO(&reads); // fd_set의 element를 0으로 초기화
	FD_SET(0, &reads); // 표준입력의 FD를 관찰대상으로 지정 (비트값을 1로 설정)
	FD_SET(sock, &reads); // sock의 FD를 관찰대상으로 지정 (비트값을 1로 설정)

	/* 1단계_2 : 검사의 범위 지정 */
	fd_max = sock; // select함수의 첫번째 인자 maxfd는 어디까지 검사할지 나타냄

	while (1)
	{
		/* 원본을 보존하기 위한 처리 */
		cpy_reads = reads; // select함수 종료 시 나머지 비트가 0으로 바뀌므로 원본유지 위해 임시저장

		/* 1단계_3 : 타임아웃의 설정 */
		// 같은 타임아웃을 주기 위해서 select함수 호출 전에 매번 초기화하는 처리를 수행 
		timeout.tv_sec = 5; // second 단위
		timeout.tv_usec = 5000; // micro second 단위

		/* 2단계 : select 함수의 호출 */
		result = select(fd_max + 1, &cpy_reads, 0, 0, &timeout); // 입력된 값이 있으면 1, 없으면 0을 반환

		/* 3단계 : 호출결과 확인 */
		if (result == -1) // 예외처리
		{
			puts("select() error!");
			break;
		}
		else if (result == 0) // 변화없는 경우
			puts("Time-out!");
		else // 변화 감지된 경우
		{
			for (i = 0; i < fd_max + 1; i++) // 변화감지된 것을 찾기 위해 for문을 돌림
			{
				if (FD_ISSET(i, &cpy_reads)) // select 함수가 1이상 반환된 경우 실행 -> 변화된 FD 찾음
				{
					if (FD_ISSET(0, &cpy_reads)) // 표준입력이 있는 경우 처리
						write_routine(sock, buf);
					else if(FD_ISSET(sock, &cpy_reads)) // 읽어들이는 처리
						read_routine(sock, buf);
				}
			}
		}
	}
	close(sock); // close함수가 호출되면 서버소켓에 EOF(End Of File)이 전송, 즉 연결이 종료
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