// ���迡 100% ���� - ���� ����ó����
#include <stdio.h>
#include <unistd.h>
int gval=10;

int main(int argc, char *argv[])
{
	pid_t pid;
	int lval=20;
	gval++, lval+=5;
	
	pid=fork(); // ���μ����� 2�� ����
	if(pid==0)	// if Child Process 
		gval+=2, lval+=2; // �ڽ� ���μ����� pid�� 0�̶� �� �κ��� ���� ##
	else			// if Parent Process
		gval-=2, lval-=2; // �θ� ���μ����� pid�� 0�� �ƴ϶� �� �κ��� ���� ##
	
	if(pid==0)
		printf("Child Proc: [%d, %d] \n", gval, lval); // +2, +2
	else
		printf("Parent Proc: [%d, %d] \n", gval, lval); // -2, -2
	return 0;
}
// ������ accept�� �ؼ� 
// clnt_sock = accept(); // ���� ��ũ���� �޾ƿ�
// ������� ó���� �ٸ� ���μ����� ���������� �ǹ� -> �Ѳ����� ó���� �� �ְ� ������
// �����⸦ �ڽ����μ�����, ó�� ���μ����� �θ����μ����� �������ڴ� �ǹ��ε�