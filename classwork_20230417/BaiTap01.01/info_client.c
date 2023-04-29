#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "errno.h"

#define BUFF_SIZE 1024

/*
 *  CMD: ./info_client [IP_ADDR] [PORT]
 *
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Must have 2 parameter [IP_ADDRESS] [PORT]\n");
        return 1;
    }

    int client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (client != -1)
        printf("Socket created: %d\n", client);
    else {
        printf("Failed to create socket: %d - %s\n", errno, strerror(errno));
        perror("socket() failed\n");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    char buf[BUFF_SIZE];

    // Xu ly va truyen du lieu
    char computer_name[64];
    printf("Nhap ten may tinh: ");
    scanf("%s", computer_name);
    getchar();

    strcpy(buf, computer_name);
    int pos = strlen(computer_name);
    buf[pos] = 0;
    pos++;

    int disk_count;
    printf("Nhap so luong o dia: ");
    scanf("%d", &disk_count);
    getchar();


    char disk_letter;
    short int disk_size;

    for (int i = 0; i < disk_count; ++i) {
        printf("Nhap ten o dia: ");
        scanf("%c", &disk_letter);

        printf("Nhap kich thuoc o dia: ");
        scanf("%hd", &disk_size);

        getchar();

        buf[pos] = disk_letter;
        pos++;
        memcpy(buf + pos, &disk_size, sizeof(disk_size));
        pos += sizeof(disk_size);
    }
    if (sendto(client, buf, pos, 0, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("Server disconnect or failed when send() !\n");
        return 1;
    } else {
        printf("send success!\n");
        memset(buf, 0, sizeof(buf));
    }
    close(client);
    return 0;
}