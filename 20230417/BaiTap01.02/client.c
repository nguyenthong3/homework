#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "errno.h"

#define BUFF_SIZE 256

/*
 * CMD: ./client [IP_ADDRESS] [PORT]
 */
int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Must have 2 parameter [IP_ADDRESS] [PORT]\n");
        return 1;
    }

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (client != -1) {
        printf("Socket created: %d", client);
    } else {
        printf("Failed to create socket: %d - %s\n", errno, strerror(errno));
        perror("socket() failed\n");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    int conn = connect(client, (struct sockaddr *) &addr, sizeof(addr));

    if (conn != -1) {
        printf("Connect to Server success\n");
    } else {
        perror("Connect() failed!\n");
        return 1;
    }

    char buf[BUFF_SIZE];

    FILE *file;

    file = fopen("log_test.txt", "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return 1;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    int c = size / BUFF_SIZE;
    printf("size: %ld, c: %d\n", size, c);

    if (c < 1) {
        while (fgets(buf, sizeof(buf), file) != NULL) {
            int ret = send(client, buf, strlen(buf), 0);
            if (ret != -1) {
                printf("%d bytes are sent\n", ret);
            }
        }
        if (send(client, buf, strlen(buf), 0) <= 0) {
            perror("Server disconnect or failed when send() !\n");
            return 1;
        }
    } else {
        for (int i = 1; i <= c + 1; ++i) {
            while (fgets(buf, sizeof(buf), file) != NULL) {
                int ret = send(client, buf, strlen(buf), 0);
                if (send(client, buf, strlen(buf), 0) <= 0) {
                    perror("Server disconnect or failed when send() !\n");
                    return 1;
                }
                if (ret != -1) {
                    printf("%d bytes are sent\n", ret);
                }
            }
            memset(buf, 0, sizeof(buf));
        }
    }

    fclose(file);
    close(client);
}
