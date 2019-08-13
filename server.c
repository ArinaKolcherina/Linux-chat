#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct sockaddr_in server, client;

int connections[100];
int counter = 0;

pthread_t tid[100];

void* clientHandler() {	
	int index = 0;
	pthread_t id = pthread_self();
	while(!pthread_equal(id, tid[index])) index++;
	
	size_t l;
	while(1) {
		recv(connections[index], (char*)&l, sizeof(size_t), NULL);
		char* msg = malloc(l+1);
		msg[l] = '\0';
		recv(connections[index], msg, l, NULL);
		
		if(strcmp(msg, "exit\n") == 0) {
			printf("%d %s\n", index, "has been disconnected");
			connections[index] = -1;
			pthread_exit(0);
		}
		else {
			printf("%d %s", index, msg);
			for(int i = 0; i < counter; i++) {
				if(i == index) continue;
				if(connections[i] == -1) continue;
				send(connections[i], (char*)&l, sizeof(size_t), NULL);
				send(connections[i], msg, l, NULL);
			}
		}
		free(msg);
	}
}

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("error: invalid socket (server)\n");
        return -1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");//INADDR_ANY;
    server.sin_port = htons(7000);
    server.sin_family = AF_INET;

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("error: failed connection\n");
        return -1;
    }
    else
        printf("connected\n");

    listen(sock, 100);

    char *addr_server = inet_ntoa(server.sin_addr);
    int port_server = ntohs(server.sin_port);
    printf("ip-address of server: %s:%d\n", addr_server, port_server);
		
    int sock_client;
    int c = sizeof(struct sockaddr_in);

    while (1)
    {
      sock_client = accept(sock, (struct sockaddr *)&client, (socklen_t *)&c);
      char *addr_client = inet_ntoa(client.sin_addr);
      int port_client = ntohs(client.sin_port);

      if (sock_client == -1)
      {
          printf("error: invalid socket (client)\n");
          return -1;
      }
      else
      {
        printf("connected with client\n");
        printf("ip-address of client: %s:%d\n", addr_client, port_client);
        connections[counter] = sock_client;
        
        if(pthread_create(&(tid[counter]), NULL, &clientHandler, NULL) != 0) {
        	printf("\nCan't create thread");
        }
        
        counter++;
      }
    }

    return 0;
}
