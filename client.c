#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct sockaddr_in client;

int sock;

void* clientHandler() {
	size_t l;
	while(1) {
		recv(sock, (char*)&l, sizeof(size_t), NULL);
		char* msg = malloc(l+1);
		msg[l] = '\0';
		recv(sock, msg, l, NULL);
		printf("%s", msg);
		free(msg);
	}
}

int main()
{
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
  {
    printf("error: invalid socket\n");
    return -1;
  }

  client.sin_addr.s_addr = inet_addr("127.0.0.1");//INADDR_ANY;
  client.sin_port = htons(7000);
  client.sin_family = AF_INET;

  printf("wait for the connect...\n");
  if (connect(sock, (struct sockaddr *)&client, sizeof(client)) == -1)
  {
    printf("error: failed connection\n");
    return -1;
  }
  printf("Connected\n");
  
	pthread_t tid;
	if(pthread_create(&tid, NULL, &clientHandler, NULL) != 0) {
  	printf("\nCan't create thread");
  }
	
	char* msg = NULL;
	size_t l;
	
	while(msg == NULL || strcmp(msg, "exit\n") != 0) {
		size_t len = getline(&msg, &l, stdin);
		
		send(sock, (char*)&len, sizeof(size_t), NULL);
		send(sock, msg, len, NULL);
	}
	
	pthread_cancel(tid);
	close(sock);
  
  return 0;
}
