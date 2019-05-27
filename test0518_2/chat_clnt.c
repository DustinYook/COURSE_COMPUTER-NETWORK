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
#define BUF_SIZE 100 // 버퍼의 크기가 부족할 수 있으니 넉넉히 잡음 (변경된 부분)
#define NAME_SIZE 20 // 이름길이를 20자로 제한 (추가된 부분)

/* 함수원형 */
void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

/* 전역변수 선언 */
char name[NAME_SIZE] = "[NULL]"; // 클라이언트를 식별하기 위한 일종의 아이디를 저장하기 위한 공간
char buf[BUF_SIZE];

int main(int argc, char *argv[])
{
	// argc는 argument count를 의미함 -> 프로그램에 전달될 인수(argument)의 개수를 포함
	// argv는 argument vector를 의미함 -> 문자열의 1차원 배열형태, 각 문자열의 chunk는 프로그램으로 전달될 인수(argument)의 개수
	// 예시) gcc -o c clnt.c A-> 총 5개의 문자열 "gcc", "-o", "c", "clnt.c", "A"로 구성, 따라서 argc는 5이다.
	// 여기서 argv[0]는 gcc이고, argv[1]는 -o, argv[2]는 c, argv[3]는 clnt.c이며 arg[4]는 A이다.
	int sock;
	pid_t pid;

	struct sockaddr_in serv_adr;
	if (argc != 4) // 기존의 3에서 name이 추가되어 4로 바뀜 (변경된 부분)
	{
		printf("Usage : %s <IP> <port> <name>\n", argv[0]); // <name>이 하나 더 추가됨 (변경된 부분)
		exit(1);
	}

	/* 중요한 처리 */
	sprintf(name, "[%s]", argv[3]); // 어떤 클라이언트가 말하는 건지 파악하기 위한 부분 (추가된 부분)
	// sprintf(넣을 배열주소, 형식, 넣을내용) -> 이름 입력받고 배열로 넣어 줌
	// int sprintf(char* str, const char* format, ...); -> function prototype
	// str은 입력받은 정보
	// 의미해석: place holder인 %s를 argv[3]으로 대체 시킨 문자열 "[%s]"를 name이라는 버퍼공간으로 출력함

	sock = socket(PF_INET, SOCK_STREAM, 0); // socket함수를 통해 소켓 생성  
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	// connect함수를 통해 서버 프로그램에 연겨요청 서비의 대기 큐에 등록 시 호출완료
	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!"); // 서버에서 accept 호출 안하면 서비스 이루어지지 않음

	// 입력하지 않아도 서버에게 정보받으려고 사용
	pid = fork(); // 1) fork를 사용하여 루틴을 분할함
	if (pid == 0)
		write_routine(sock, buf); // 똑같은 인자 받음
	else
		read_routine(sock, buf);  // 똑같은 인자 받음

	close(sock); // close함수가 호출되면 서버소켓에 EOF가 전송되면서 연결이 종료됨
	return 0;
}

/* 함수정의 */
void read_routine(int sock, char *buf)
{
	char total_msg[NAME_SIZE + BUF_SIZE]; // 전체 메세지길이: 이름과 버퍼사이즈를 합친 영역을 할당 (변경된 부분)
	while (1)
	{
		int str_len = read(sock, total_msg, NAME_SIZE + BUF_SIZE - 1); // sock에서 전체 메세지길이만큼 읽어 total_msg에 넣음 (변경된 부분)
		// ssize_t read(int fd, void* buf, size_t nbyte);
		// read함수는 fd에 해당되는 파일이나 장치에서 nbyte를 읽어서 이를 변수 buf에 넣음
		// read함수는 요청한 바이트를 채우기 전 EOF에 도달시 0을 리턴함
		// read함수는 성공하면 읽어들인 바이트 수를 리턴하고, 실패하면 -1을 리턴하고 errno를 설정
		if (str_len == 0) // 읽어들인 것이 없는 경우
			return;
		total_msg[str_len] = 0; // str_len-1까지 문자열이 존재하므로 str_len을 EOF로 설정 (변경된 부분)
		fputs(total_msg, stdout); // total_msg에 저정된 내용을 모니터에 출력 (변경된 부분)
	}
}

void write_routine(int sock, char *buf)
{
	char total_msg[NAME_SIZE + BUF_SIZE]; // 전체 메세지길이: 이름과 버퍼사이즈를 합친 영역을 할당 (변경된 부분)
	while (1)
	{
		fgets(buf, BUF_SIZE, stdin);
		if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
		{
			// 2) shutdown의 첫번째 인자는 sock으로 read_routine의 read의 첫번째 인자 sock과 동일 함 -> 출력스트림만 끔
			shutdown(sock, SHUT_WR); // 3) close가 아닌 shutdown으로 half-close함
			return;
		}

		sprintf(total_msg, "%s %s", name, buf); // place holder %s가 대체된 문자열이 total_msg의 버퍼에 저장됨 (추가된 부분)
		// sprintf()와 printf()의 비교
		// printf()는 화면에 결과를 출력하기 위한 함수
		// sprintf()는 버퍼에 결과를 출력하는 함수, 버퍼에 입력되는 문자열을 조작하기 쉬워짐

		write(sock, total_msg, strlen(total_msg)); // 버퍼(total_msg)길이만큼 버퍼(total_msg)에서 읽어와 sock이 가리키는 곳으로 출력 (변경된 부분)
		// ssize_t write(int fd, const void* buf, size_t nbyte);
		// write함수는 nbyte를 사용자 버퍼 buf에서 fd가 가리키는 곳으로 출력
		// write함수는 성공하면 출력한 바이트 수를 리턴하고, 실패하면 -1을 리턴하고 errno를 설정
	}
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}