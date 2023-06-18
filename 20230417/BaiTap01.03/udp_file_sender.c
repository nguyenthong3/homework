#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Must have 3 parameter [IP_ADDRESS] [PORT] [filename]\n");
        return 1;
    }

    int sockfd;
    struct sockaddr_in server_addr;
    char buf[BUF_SIZE];
    FILE *fp;
    size_t file_size, read_size, sent_size;
    int server_port = atoi(argv[2]);

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket() failed!\n");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(server_port);

    if ((fp = fopen(argv[3], "rb")) == NULL) {
        printf("fopen() failed!\n");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    sprintf(buf, "%zu", file_size);
    if (sendto(sockfd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("sendto() failed!\n");
        return 1;
    }

    while ((read_size = fread(buf, 1, BUF_SIZE, fp)) > 0) {
        if ((sent_size = sendto(sockfd, buf, read_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1) {
            printf("sendto() failed!\n");
            return 1;
        }
    }

    fclose(fp);
    close(sockfd);

    return 0;
}