#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

/*
 * Viết chương trình sv_server, nhận dữ liệu từ sv_client
 * in ra màn hình và đồng thời ghi vào file sv_log.txt.
 * Dữ liệu được ghi trên một dòng với mỗi client, kèm theo địa chỉ IP và thời gian client đã gửi.
 * Tham số cổng và tên file log được nhập từ tham số dòng lệnh.'
 *
 * CMD: sv_server [PORT] sv_log.txt
 *
 * VD: 127.0.0.1 2023-04-10 09:00:00 20201234 Nguyen Van A 2002-04-10 3.99
*/
int main(int argc, char *argv[]) {

    FILE *file;

    if (argc != 3) {
        perror("Must have 2 parameter <PORT> <FILE_SV_LOG>!\n");
        return 1;
    }

    // Tao socket theo giao thuc UDP
    int server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // Khai bao cau truc dia chi
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

    // Khai bao cau truc dia chi cua client
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(addr);

    // Voi UDP khong can chuyen sang che do Listener
    char buf[1024];
    while (1) {
        int ret = recvfrom(server, buf, sizeof(buf), 0, (struct sockaddr *) &clientAddr, &clientAddrLen);
        if (ret == -1) {
            printf("recvfrom() failed\n");
            return 1;
        }
        if (ret < sizeof(buf)) {
            buf[ret] = 0;
            file = fopen(argv[2],"a");
            if (file == NULL) {
                printf("Failed to open file.\n");
                continue;
            }
            fprintf(file, "%s\n", buf);
            fclose(file);
            printf("Thong tin sinh vien: %s\n", buf);
        }
    }
}