#include <stdio.h>
#include <process.h>
#include <winsock.h>
#include <windows.h>

#pragma comment(lib, "wsock32.lib")	
#define PORT 20000							// �����Ʈ�� 20000
#define MAX_CLIENT 5							// �ִ� ��� �ο� �� 5��( �ۼ��� ���� )
#define ALLOW 6000							// �ִ� ���� ���� ���� ��ȣ 6000

void recv_client(void * kim);					// ������ �Լ� ������ Ÿ��
int client_num = 0;							// ���� Ƚ�� (Ŭ���̾�Ʈ ����)
int number = 0;								// Ŭ���Ʈ ��ȣ
char user_hi[] = "�� �մ�";					// Welcome ���� �ʱ� ��
char user_full[] = "����á���ϴ�.\n";			// Welcome ����� �ʰ��� ����� ��
int client_sock[ALLOW];						// client_sock (Ŭ���̾�Ʈ Welcome Socket)
HANDLE hMutex;								// ���ý�
SOCKADDR_IN server_info, client_info;

int main()
{
	int addrsize, ret;
	SOCKET server;
	
	// Welcome Screen
	printf("�����������������������\n");
	printf("��                                        ��\n");
	printf("��     ��Ƽ �����带 �̿��� ��Ƽ ä��     ��\n");
	printf("��             ���� (Server)              ��\n");
	printf("��                                        ��\n");
	printf("�����������������������\n");


	// ���ý� ����
	hMutex = CreateMutex(NULL, FALSE, NULL);	// ���� ���н� ����
	if (!hMutex)
	{
		printf("�� Mutex �����Դϴ�. ��\n");
		CloseHandle(hMutex);
		return 0;
	}

	// ���� �ʱ�ȭ
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
	{
		printf("��Winsock �����Դϴ�.��\n");
		WSACleanup();
		return 0;

	}
	// ���� ����
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == SOCKET_ERROR)
	{
		printf("��socket() �����Դϴ�.��\n");
		closesocket(server);
		WSACleanup();
		return 0;
	}

	server_info.sin_addr.s_addr = htonl(INADDR_ANY);
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(PORT);

	// Bind
	if (bind(server, (SOCKADDR_IN*)&server_info, sizeof(server_info)) == SOCKET_ERROR)
	{
		printf("��bind() �����Դϴ�.��\n");
		closesocket(server);
		WSACleanup();
		return 0;

	}

	printf("�� ä�ù��� ���۵Ǿ����ϴ�. %d�� ���ҽ��ϴ�. ��\n", MAX_CLIENT - client_num);
	listen(server, 10);
	addrsize = sizeof(client_info);

	// Ŭ���̾�Ʈ ������ ��ٸ��ϴ�.
	while (1)
	{
		// ���ŷ ������� Client �� ��ٸ��ϴ�.
		client_sock[number] = accept(server, (SOCKADDR_IN*)&client_info, &addrsize);

		if (client_num < MAX_CLIENT)		// ���� �����Ҷ�
		{
			printf("|| �� %d�� �մ��� (IP:%s, ���� ��ȣ:%d) �����Ͽ����ϴ�.\n", number + 1, inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));
			_beginthread(recv_client, 0, &client_sock[number]);
			Sleep(10);
			
		}

		else								// ���� �ο� �ʰ�
		{
			addrsize = sizeof(client_info);
			if (client_sock[number] == INVALID_SOCKET)
			{
				printf("accept() �����Դϴ�.\n");
				closesocket(client_sock[number]);
				closesocket(server);
				WSACleanup();
				return 1;
			}

			ret = send(client_sock[number], user_full, sizeof(user_full), 0);
		}
		closesocket(client_sock[number]);
	}
	return 0;
}

void recv_client(void * kim)
{
	// ���� ����
	WaitForSingleObject(hMutex, INFINITE);
	client_num++;																	// Ŭ���̾�Ʈ ������ ����
	number++;																	// Ŭ���̾�Ʈ ��ȣ ����
	printf("|| �� ���� ���� �ο��� %d�� ���ҽ��ϴ�.\n", MAX_CLIENT - client_num);		// ������ �Ǵ�

	ReleaseMutex(hMutex);

	char user[50] = { 0 };														// accept �� ���Ͽ��� �� ���� ����
	char buff[1024] = { 0 };
	int ret, i;


	_itoa(number, user, 10);														// Ŭ���̾�Ʈ ��ȣ
	strcat(user, user_hi);															// ���� ȯ�� �޽��� ȯ��
	ret = send(*(SOCKET*)kim, user, sizeof(user), 0);								// ����

	while (ret != SOCKET_ERROR || ret != INVALID_SOCKET)
	{
		ret = recv(*(SOCKET*)kim, buff, 1024, 0);									// Ŭ���̾�Ʈ�� �޽����� ����

		// broadcast �κ�
		for (i = 0; i < ALLOW; i++)												
		{							
			// ���� Ŭ���̾�Ʈ ������ �޸� �ּҿ� ������ Ŭ���̾�Ʈ ���� �޸� �ּҰ� �ٸ����� ����
			WaitForSingleObject(hMutex, INFINITE);
			if (((unsigned*)&client_sock[i] != (SOCKET*)kim))
			{
				send(client_sock[i], buff, strlen(buff), 0);
			}
			ReleaseMutex(hMutex);
		}

		// ���� â�� �մ԰��� ��ȭ ��� ����
		if (strlen(buff) != 0) 
			printf("|| ��(ũ��:%d) %s", strlen(buff), buff);

		memset(buff, 0, 1024);

	}

	// ���ӵ� ������ ������ ������ ��
	WaitForSingleObject(hMutex, INFINITE);
	client_num--;
	printf("|| ���մ��� �������ϴ�.\n || ���� ���� �ο� %d���Դϴ�.\n", MAX_CLIENT - client_num);
	number--;
	ReleaseMutex(hMutex);

	closesocket(*(int*)kim);

	return;
}