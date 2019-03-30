/*
���� ȣ��Ʈ �ּҿ� ������ ���� / ������ Ŭ���̾�Ʈ�� ���� �Ŀ�
Ŭ���̾�Ʈ�� ���� �޽����� �ٸ� Ŭ���Ʈ�鿡�� �����ϴ� ����� ������ �ֽ��ϴ�.
*/

#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <process.h>

#pragma comment(lib,"wsock32.lib")

#define PORT 20000						// ����� ��Ʈ�� 20000
#define IP "127.0.0.1"						// ������ ������ ���� ȣ��Ʈ

void recv_thread(void* kim);					// ������ ���� �Լ��� ������ Ÿ��

int ret = 0;								// ���� ��
SOCKET server;							// ���� ��
HANDLE hMutex;							// ���ؽ���

int main()
{
	// ���� ������ ���� ���� ����
	SOCKADDR_IN server_info;

	// ���� �ʱ�ȭ
	WSADATA wsd;
	char buff[1024];
	char num[1024];
	char num2[1024];
	char *t1 = " <", *t3 = "> ";

	// Welcome Screen
	printf("�����������������������\n");
	printf("��                                        ��\n");
	printf("��     ��Ƽ �����带 �̿��� ��Ƽ ä��     ��\n");
	printf("��           Ŭ���̾�Ʈ (Client)          ��\n");
	printf("��                                        ��\n");
	printf("�����������������������\n");

	// ���ý� �ʱ�ȭ
	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (!hMutex)
	{
		printf("�� Mutex �����Դϴ�. ��\n");
		return 0;

	}

	if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
	{
		printf("�� Winsock �����Դϴ�. ��\n");
		return 0;
	}

	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == SOCKET_ERROR)
	{
		printf("�� Winsock �����Դϴ�. ��\n");
		closesocket(server);
		WSACleanup();
		return 0;

	}

	server_info.sin_addr.s_addr = inet_addr(IP);
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(PORT);

	// ������ ����
	if (connect(server, (SOCKADDR_IN*)&server_info, sizeof(server_info)) == SOCKET_ERROR)
	{
		printf("�� connect() �����Դϴ�. ��\n");
		closesocket(server);
		WSACleanup();
		return 0;

	}

	

	// �������� �켱 �ִ� user �޽��� �м�
	ret = recv(server, num, 1024, 0);

	// ���� á�ٰ� �޽����� �Դٸ�
	if (!strcmp("����á���ϴ�.\n", num))
	{
		printf("�� �ο��� �� á���ϴ�. ��\n");
		closesocket(server);
		WSACleanup();
		return 0;					// ������ ����
	}
	printf("�� ������ ���� ���� ��\n");
	printf("�� %s ȯ���մϴ� ��\n", num);

	// ���� ������ �Ǹ� ������ �۵� - �޴� �޽����� ������� �ǽð� ����
	_beginthread(recv_thread, 0, NULL);

	// ������ �޽����� ������� ���� �ʿ䰡 �����ϴ�.
	while (ret != INVALID_SOCKET || ret != SOCKET_ERROR)
	{

		// ������ : CPU ������ ���ҿ�
		Sleep(5);
		fgets(buff, 1024, stdin);
		sprintf(num2, "%s%s%s", t1, num, t3); // �� �迭 num2 �� ���� ���ڿ� ����ֱ�

		strcat(num2, buff);

		// ���� ��� �߸� �� ����� ������� Ż��
		if (ret == INVALID_SOCKET || ret == SOCKET_ERROR) break;

		ret = send(server, num2, strlen(num2), 0);

		// num2 �ʱ�ȭ
		memset(num2, 0, 1024);

	}

	// ������ ������ ���
	printf("�� ������ ������ ������ϴ�. ��\n");
	closesocket(server);
	WSACleanup();

	return 0;
}

// �޴� ������ �κ�
void recv_thread(void* kim)
{
	int ret_thread = 60000;
	char buff_thread[1024] = { 0 };

	// ������� ���� ���� ����ϴ� ���� �ƴϸ� �޴� �߿� ������ ����� ����ٰ� ���� ����
	while (ret_thread != INVALID_SOCKET || ret_thread != SOCKET_ERROR)
	{
		Sleep(5);	// CPU ������ 100% ������

		// �������� �ִ� �޽����� �ǽð����� ��ٷȴٰ� �޽��ϴ�.
		ret_thread = recv(server, buff_thread, sizeof(buff_thread), 0);

		// �������� �޴� �۾��� �� ��� �������� �� Ż��
		if (ret_thread == INVALID_SOCKET || ret_thread == SOCKET_ERROR)
			break;

		// ���������� ���� ���۸� ���
		printf("�� (ũ��:%d) �޽��� ���� : %s", strlen(buff_thread), buff_thread);
		memset(buff_thread, 0, 1024);	// ���� ���۸� �ʱ��
	}

	// �۾��� ���� ������ ��ȿȭ��Ŵ
	WaitForSingleObject(hMutex, 100L);
	ret = INVALID_SOCKET;
	ReleaseMutex(hMutex);

	return;
}