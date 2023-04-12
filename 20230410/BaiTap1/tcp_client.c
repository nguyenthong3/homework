#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "errno.h"

/*
* Viet chuong trinh tcp_client ket noi den 1 may chu
* xac dinh boi dia chi IP va Port, sau do nhan du lieu 
* tu ban phim va gui den server. Tham so truyen vao co dang:
* tcp_client <IP_ADDR> <PORT>
 *
 * VI DU CAU LENH CMD: ./tcp_client 127.0.0.1 9000
*/
int main(int argc, char* argv[]) {

    if (argc != 3) {
        perror("Must have 2 parameter <IP_ADDRESS> <PORT>\n");
        exit(1);
    }

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (client != -1)
        printf("Socket created: %d\n", client);
    else {
        printf("Failed to create socket: %d - %s\n", errno, strerror(errno));
        perror("socket() failed\n");
        exit(1);
    }

    // Xac dinh cau truc dia chi cua Client
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]); // Dia chi cua server
    addr.sin_port = htons(atoi(argv[2])); // Cong cua server


    int ret = connect(client, (struct sockaddr *) &addr, sizeof(addr));

    if (ret != -1) {
        printf("Connect to Server success\n");
    } else {
        perror("Connect() failed!\n");
        exit(1);
    }

    char buf[256];

    // nhan msg tu Server
    ret = recv(client, buf, sizeof(buf), 0);
    if (ret == -1 || ret == 0) {
        perror("Connection closed or has error while recv() !\n");
        return 1;
    } else {
        if (ret < sizeof(buf)) buf[ret] = 0;
        printf("Server says: %s\n", buf);
    }

    while (1)
    {
        printf("Enter message: ");
        fgets(buf, sizeof(buf), stdin);

        if (send(client, buf, strlen(buf), 0) <= 0) {
            perror("Server disconnect or failed when send() !\n");
            return 1;
        }
        if (strncmp(buf, "exit", 4) == 0)
            break;
    }

    close(client);
    return 1;
}