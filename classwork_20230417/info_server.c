#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

/*
 * CMD: ./info_server [PORT]
 */
int main(int argc, char *argv[]) {

    if (argc != 2) {
        perror("Must have 2 parameter <PORT> !\n");
        return 1;
    }

    int server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

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

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    // Truyen nhan du lieu
    char buf[BUFF_SIZE];


    while (1) {
        int ret = recvfrom(server, buf, sizeof(buf), 0, (struct sockaddr *) &clientAddr, &clientAddrLen);
        if (ret == -1) {
            printf("recvfrom() failed\n");
            return 1;
        }
        buf[ret] = 0;
        int pos = 0;

        // Tach du lieu tu buffer
        char computer_name[64];
        strcpy(computer_name, buf);
        pos += strlen(computer_name) + 1;

        printf("Computer name: %s\n", computer_name);

        int num_drives = (ret - pos) / 3;
        for (int i = 0; i < num_drives; i++) {
            char drive_letter;
            short int drive_size;

            drive_letter = buf[pos];
            pos++;

            memcpy(&drive_size, buf + pos, sizeof(drive_size));
            pos += sizeof(drive_size);

            printf("%c: %hd\n", drive_letter, drive_size);
        }
    }
}
