//
// Created by thong on 19/06/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>

int main() {
    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (server == -1) {
        perror("socket() failed!");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(server, (struct sockaddr *) &addr, sizeof(addr))) {
        perror("bind() failed");
        return 1;
    }

    if (listen(server, 5)) {
        perror("listen() failed");
        return 1;
    }

    fd_set fdread;

    int clients[64];
    int num_clients = 0;

    char buf[256];

    struct timeval tv;

    while (1) {
        // Xóa tất cả socket trong tập fdread
        FD_ZERO(&fdread);

        // Thêm socket server vào tập fdread
        FD_SET(server, &fdread);
        int maxdp = server + 1;

        // Thêm các socket client vào tập fdread
        for (int i = 0; i < num_clients; i++) {
            FD_SET(clients[i], &fdread);
            if (maxdp < clients[i] + 1)
                maxdp = clients[i] + 1;
        }

        // Thiet lap thoi gian cho
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        // Chờ đến khi sự kiện xảy ra
        int ret = select(maxdp, &fdread, NULL, NULL, NULL);

        if (ret < 0) {
            perror("select() failed");
            return 1;
        }

        if (ret == 0) {
            printf("Timed out!!!\n");
            continue;
        }

        // Kiểm tra sự kiện có yêu cầu kết nối
        if (FD_ISSET(server, &fdread)) {
            int client = accept(server, NULL, NULL);
            printf("Ket noi moi: %d\n", client);
            clients[num_clients++] = client;
        }

        // Kiểm tra sự kiện có dữ liệu truyền đến socket client
        for (int i = 0; i < num_clients; i++)
            if (FD_ISSET(clients[i], &fdread)) {
                ret = recv(clients[i], buf, sizeof(buf), 0);
                if (ret <= 0) {
                    // TODO: Client đã ngắt kết nối, xóa client ra khỏi mảng
                    clients[i] = 0;
                    continue;
                }
                buf[ret] = 0;
                printf("Client %d: %s\n", clients[i], buf);
                for (int j = 0; j < num_clients; ++j) {
                    if (i != j) {
                        send(clients[j], buf, strlen(buf), 0);
                    }
                }
            }
    }

    close(server);

    return 0;
}