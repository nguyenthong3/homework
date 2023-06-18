//
// Created by thong on 22/04/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Must have 1 parameter [PORT]\n");
        return 1;
    }

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buf[BUF_SIZE];
    FILE *fp;
    size_t file_size, remain_size, read_size, received_size;
    socklen_t client_addr_len;
    int server_port = atoi(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("socket() failed!\n");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("bind() failed!\n");
        return 1;
    }

    client_addr_len = sizeof(client_addr);
    if (recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len) == -1) {
        printf("recvfrom() failed!\n");
        return 1;
    }
    file_size = atoi(buf);

    if ((fp = fopen("received_file.pdf", "wb")) == NULL) {
        printf("fopen() failed!\n");
        return 1;
    }

    remain_size = file_size;
    while (remain_size > 0) {
        received_size = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (received_size == -1) {
            printf("recvfrom() failed!\n");
            return 1;
        }
        fwrite(buf, 1, received_size, fp);
        remain_size -= received_size;
    }

    fclose(fp);

    close(sockfd);

    return 0;
}