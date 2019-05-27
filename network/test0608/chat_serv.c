// 이 코드는 기말고사에서 반드시 출제하니 반드시 모든 내용 숙지할 것!
// 프로그램 목적 : fork 대신 thread를 이용한 다중접속서버의 구현방식
// 코드 컴파일 할 때 아래의 옵션을 반드시 써주어야 에러 발생하지 않음
// 1) -lpthread : <pthread.h>를 이용하기 위한 옵션
// 2) -D_REENTRANT : unsafe한 함수를 safe한 함수로 전환시키는 옵션
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define BUF_SIZE 100
#define MAX_CLNT 256

/* 함수원형 */
void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

/* 전역변수 */
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT]; // 클라이언트 소켓을 담을 변수 정의 -> 공유변수
pthread_mutex_t mutx; // mutex - 0단계 : mutex 정의

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id; // thread - 0단계 : thread 정의 
	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&mutx, NULL); // mutex - 1단계 : pthread_mutex_init 
									 // (학부수준에서 두번째 인자는 NULL로 설정)
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	// 여기에 lock과 unlock을 한 이유 : 쓰레드와 main이 동시에 소켓에 접근하는 것을 방지하기 위함
	while (1)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

		// 시험유형 : 동기화가 필요한 부분에 코드를 추가하시오 (임계영역 파악중요)
		pthread_mutex_lock(&mutx); // mutex - 2단계 : pthread_mutex_lock
		/* Critical Section Start */
		clnt_socks[clnt_cnt++] = clnt_sock; // mutex - 2단계 : critical section
		// 클라이언트로부터 연결요청이 왔을 떄 클라이언트를 배열에 넣음 -> 이것만 주어짐
		/* Critical Section End */
		pthread_mutex_unlock(&mutx); // mutex - 2단계 : pthread_mutex_unlock

		// thread - 3단계 : pthread_create
		// pthread_create(쓰레드, NULL, 수행함수, 함수인자); -> 두번째 인자는 NULL
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);

		// thread - 4단계 : pthread_join 또는 pthread_detach
		pthread_detach(t_id); // 쓰레드 종료시까지 대기
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}

void * handle_clnt(void * arg)
{
	int clnt_sock = *((int*)arg); // clnt_sock은 지역변수
	int str_len = 0, i;
	char msg[BUF_SIZE];

	/* 데이터 읽어 그대로 보내주는 처리수행 */
	while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
		send_msg(msg, str_len);

	pthread_mutex_lock(&mutx); // mutex - 2단계 : pthread_mutex_lock

	/* Critical Section Start */
	// 아래 코드에 대한 설명
	// 1) 4 5  6  7 8 9 -> 6번 클라이언트가 종료 요청
	// 2) 4 5 [ ] 7 8 9 -> 10번 클라이언트가 연결 요청
	// 3) 4 5 [ ] 7 8 9 10 -> 6번 자리는 계속 비어있는 문제점 발생
	// 3번의 문제를 해결하기 위해 아래와 같은 알고리즘을 이용
	// 1) 4 5 6 7 8 9 -> 6번 클라이언트가 종료 요청
	// 2) 4 5 7 8 9 -> 10번 클라이언트가 연결요청
	// 3) 4 5 7 8 9 10 -> 앞으로 당겨오는 역할을 수행
	for (i = 0; i < clnt_cnt; i++) // remove disconnected client
	{
		if (clnt_sock == clnt_socks[i])
		{
			while (i < clnt_cnt - 1)
			{
				// 조교님 꿀팁 : 전역변수 접근하면 임계영역
				clnt_socks[i] = clnt_socks[i + 1]; // 실질적인 임계영역
				i++;
			}
			break;
		}
	} clnt_cnt--;
	/* Critical Section End */

	pthread_mutex_unlock(&mutx); // mutex - 2단계 : pthread_mutex_unlock
	close(clnt_sock);
	return NULL;
}

/* 연결된 클라이언트에게 메세지를 보냄 */
void send_msg(char * msg, int len)   // send to all
{
	int i; // 지역변수
	
	pthread_mutex_lock(&mutx); // mutex - 2단계 : pthread_mutex_lock
	// pthread_mutex_lock과 unlock을 아래의 for문 안에 넣으면 안되는 건 아니지만
	// lock과 unlock은 처리가 오래걸리기 때문에 호출을 최소하는 것이 좋아 for문 밖으로 뺀 것
	
	/* Critical Section Start */
	for (i = 0; i < clnt_cnt; i++)
		write(clnt_socks[i], msg, len); // 전역변수인 clnt_socks에 접근 -> 임계영역
	/* Critical Section End */
	
	pthread_mutex_unlock(&mutx); // mutex - 2단계 : pthread_mutex_unlock
}

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}