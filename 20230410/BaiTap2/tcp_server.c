#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "errno.h"

/*
 * Viet chuong trinh tcp_server , doi ket noi o cong xac dinh boi
 * tham so dong lenh. Moi khi co client ket noi den, thi gui xau chao
 * duoc chi ra trong moi tep tin xac dinh, sau do ghi toan bo noi dung
 * client gui den vao mot tep tin khac dc chi ra trong tham so dong lenh
 *
 * tcp_server <PORT> <FILE_HELLO> <FILE_STORE_CLIENT_MESSAGE>
 *
 * VI DU CAU LENH CMD: ./tcp_server 9000 hello.txt client_send.txt
*/
int main(int argc, char *argv[]) {

    FILE *file;

    if (argc != 4) {
        perror("Must have 3 parameter <PORT> <FILE_HELLO> <FILE_STORE_CLIENT_MESSAGE> \n");
        exit(1);
    }

    // Khai bao socket server
    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (server != -1)
        printf("Socket created: %d\n", server);
    else {
        printf("Failed to create socket: %d - %s\n", errno, strerror(errno));
        perror("socket() failed\n");
        exit(1);
    }

    // Khai bao cau truc dia chi
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    // Gan cau truc dia chi cho cho socket
    int bindForSocket = bind(server, (struct sockaddr *) &addr, sizeof(addr));

    if (bindForSocket != -1) {
        printf("Bind address for socket success!\n");
    } else {
        perror("bind() failed\n");
        exit(1);
    }

    // Chuyen server socket sang trang thai cho ket noi
    if (listen(server, 5) != -1) {
        printf("Change socket to listen ...\n");
    } else {
        perror("listen() failed!\n");
        exit(1);
    }

    // Khai bao cau truc dia chi cua client
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(addr);

    // Chap nhan ket noi den tu client
    int client = accept(server, (struct sockaddr *) &clientAddr, (socklen_t *) &clientAddrLen);
    printf("Client IP: %s:%d connected\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));


    // Gui tin nhan chao mung den client
    char msg[50];
    file = fopen(argv[2], "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return 1;
    }
    while (fgets(msg, sizeof(msg), file) != NULL) {
        int ret = send(client, msg, strlen(msg), 0);
        if (ret != -1) {
            printf("%d bytes are sent\n", ret);
        }
    }
    fclose(file);

    char buf[256];

    while (1) {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0) {
            printf("Connection are closed.\n");
            break;
        }
        if (ret < sizeof(buf))
            buf[ret] = 0;

        printf("%d bytes received\n", ret);
        printf("%s\n", buf);

        file = fopen(argv[3], "a");
        if (file == NULL) {
            printf("Failed to open file.\n");
            continue;
        }
        if (strncmp(buf,"exit",4) == 0) break;
        fprintf(file, "%s", buf);
        fclose(file);
    }

    // Dong ket noi
    close(client);
    close(server);

}