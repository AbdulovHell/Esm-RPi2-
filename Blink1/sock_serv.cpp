#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "sock_serv.h"

void* SocketServer(void* threadID)
{
	int sock, listener;
	struct sockaddr_in addr;
	char buf[1024];
	int bytes_read;

	printf("Server start...");
	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0)
	{
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(4550);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(2);
	}

	listen(listener, 1);

	while (1) {
		sock = accept(listener, NULL, NULL);
		if (sock < 0) {
			perror("accept");
			exit(3);
		}

		while (1) {
			bytes_read = recv(sock, buf, 1024, 0);
			if (bytes_read <= 0) break;
			printf(buf);
			send(sock, buf, bytes_read, 0);
		}

		close(sock);
	}
}
