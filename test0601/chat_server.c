// �� �ڵ�� �⸻��翡�� �ݵ�� �����ϴ� �ݵ�� ��� ���� ������ ��!
// ���α׷� ���� : fork ��� thread�� �̿��� �������Ӽ����� �������
// �ڵ� ������ �� �� �Ʒ��� �ɼ��� �ݵ�� ���־�� ���� �߻����� ����
// 1) -lpthread : <pthread.h>�� �̿��ϱ� ���� �ɼ�
// 2) -D_REENTRANT : unsafe�� �Լ��� safe�� �Լ��� ��ȯ��Ű�� �ɼ�
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

/* �Լ����� */
void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

/* �������� */
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT]; // Ŭ���̾�Ʈ ������ ���� ���� ���� -> ��������
pthread_mutex_t mutx; // mutex - 0�ܰ� : mutex ����

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id; // thread - 0�ܰ� : thread ���� 
	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&mutx, NULL); // mutex - 1�ܰ� : pthread_mutex_init 
									 // (�кμ��ؿ��� �ι�° ���ڴ� NULL�� ����)
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	// ���⿡ lock�� unlock�� �� ���� : ������� main�� ���ÿ� ���Ͽ� �����ϴ� ���� �����ϱ� ����
	while (1)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

		// �������� : ����ȭ�� �ʿ��� �κп� �ڵ带 �߰��Ͻÿ� (�Ӱ迵�� �ľ��߿�)
		pthread_mutex_lock(&mutx); // mutex - 2�ܰ� : pthread_mutex_lock
		/* Critical Section Start */
		clnt_socks[clnt_cnt++] = clnt_sock; // mutex - 2�ܰ� : critical section
		// Ŭ���̾�Ʈ�κ��� �����û�� ���� �� Ŭ���̾�Ʈ�� �迭�� ���� -> �̰͸� �־���
		/* Critical Section End */
		pthread_mutex_unlock(&mutx); // mutex - 2�ܰ� : pthread_mutex_unlock

		// thread - 3�ܰ� : pthread_create
		// pthread_create(������, NULL, �����Լ�, �Լ�����); -> �ι�° ���ڴ� NULL
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);

		// thread - 4�ܰ� : pthread_join �Ǵ� pthread_detach
		pthread_detach(t_id); // ������ ����ñ��� ���
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}

void * handle_clnt(void * arg)
{
	int clnt_sock = *((int*)arg); // clnt_sock�� ��������
	int str_len = 0, i;
	char msg[BUF_SIZE];

	/* ������ �о� �״�� �����ִ� ó������ */
	while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
		send_msg(msg, str_len);

	pthread_mutex_lock(&mutx); // mutex - 2�ܰ� : pthread_mutex_lock

	/* Critical Section Start */
	// �Ʒ� �ڵ忡 ���� ����
	// 1) 4 5  6  7 8 9 -> 6�� Ŭ���̾�Ʈ�� ���� ��û
	// 2) 4 5 [ ] 7 8 9 -> 10�� Ŭ���̾�Ʈ�� ���� ��û
	// 3) 4 5 [ ] 7 8 9 10 -> 6�� �ڸ��� ��� ����ִ� ������ �߻�
	// 3���� ������ �ذ��ϱ� ���� �Ʒ��� ���� �˰����� �̿�
	// 1) 4 5 6 7 8 9 -> 6�� Ŭ���̾�Ʈ�� ���� ��û
	// 2) 4 5 7 8 9 -> 10�� Ŭ���̾�Ʈ�� �����û
	// 3) 4 5 7 8 9 10 -> ������ ��ܿ��� ������ ����
	for (i = 0; i < clnt_cnt; i++) // remove disconnected client
	{
		if (clnt_sock == clnt_socks[i])
		{
			while (i < clnt_cnt - 1)
			{
				// ������ ���� : �������� �����ϸ� �Ӱ迵��
				clnt_socks[i] = clnt_socks[i + 1]; // �������� �Ӱ迵��
				i++;
			}
			break;
		}
	} clnt_cnt--;
	/* Critical Section End */

	pthread_mutex_unlock(&mutx); // mutex - 2�ܰ� : pthread_mutex_unlock
	close(clnt_sock);
	return NULL;
}

/* ����� Ŭ���̾�Ʈ���� �޼����� ���� */
void send_msg(char * msg, int len)   // send to all
{
	int i; // ��������
	
	pthread_mutex_lock(&mutx); // mutex - 2�ܰ� : pthread_mutex_lock
	// pthread_mutex_lock�� unlock�� �Ʒ��� for�� �ȿ� ������ �ȵǴ� �� �ƴ�����
	// lock�� unlock�� ó���� �����ɸ��� ������ ȣ���� �ּ��ϴ� ���� ���� for�� ������ �� ��
	
	/* Critical Section Start */
	for (i = 0; i < clnt_cnt; i++)
		write(clnt_socks[i], msg, len); // ���������� clnt_socks�� ���� -> �Ӱ迵��
	/* Critical Section End */
	
	pthread_mutex_unlock(&mutx); // mutex - 2�ܰ� : pthread_mutex_unlock
}

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}