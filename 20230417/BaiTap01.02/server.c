#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "errno.h"

#define BUFF_SIZE 256

int countSubstring(char *buf, char *pattern) {
    int count = 0;
    char *result = buf;
    while ((result = strstr(result, pattern)) != NULL) {
        count++;
        result += strlen(pattern);
    }
    return count;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Must have 1 parameter [PORT]\n");
        return 1;
    }

    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (server != -1)
        printf("Socket created: %d\n", server);
    else {
        printf("Failed to create socket: %d - %s\n", errno, strerror(errno));
        perror("socket() failed\n");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    int bindForSocket = bind(server, (struct sockaddr *) &addr, sizeof(addr));
    if (bindForSocket != -1) {
        printf("Bind address for socket success!\n");
    } else {
        perror("bind() failed\n");
        return 1;
    }

    if (listen(server, 5) != -1) {
        printf("Change socket to listen ...\n");
    } else {
        perror("listen() failed!\n");
        return 1;
    }

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(addr);

    int client = accept(server, (struct sockaddr *) &clientAddr, (socklen_t *) &clientAddrLen);
    printf("Client IP: %s:%d connected\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    char *buf = (char *) malloc(BUFF_SIZE * sizeof(char));
    char buf2[BUFF_SIZE];

    int i = 1;
    while (1) {
        if (i > 1) {
            buf = (char*) realloc(buf, i * BUFF_SIZE * sizeof(char));
        }
        int ret = recv(client, buf2, sizeof(buf2), 0);
        if (ret <= 0) {
            printf("Connection is closed!\n");
            break;
        }
        strcat(buf,buf2);
        printf("%d bytes received\n", ret);
        i++;
    }
    printf("So lan xuat hien 123456789: %d\n", countSubstring(buf,"123456789"));
    free(buf);
    close(client);
    close(server);
}