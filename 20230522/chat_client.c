//
// Created by thong on 19/06/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        perror("Must have 2 parameter <ADDR> <PORT> \n");
        exit(1);
    }

    int sender = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    if (connect(sender, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("connect() failed");
        return 1;
    }

    fd_set fdread;
    char buf[256];

    while (1)
    {
        FD_ZERO(&fdread);
        FD_SET(STDIN_FILENO, &fdread);
        FD_SET(sender, &fdread);

        int ret = select(sender + 1, &fdread, NULL, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &fdread))
        {
            fgets(buf, sizeof(buf), stdin);
            send(sender, buf, strlen(buf), 0);
        }

        if (FD_ISSET(sender, &fdread))
        {
            ret = recv(sender, buf, sizeof(buf), 0);
            buf[ret] = 0;
            printf("%s\n", buf);
        }
    }

    close(sender);

    return 0;
}