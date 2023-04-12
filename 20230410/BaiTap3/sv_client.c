#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "errno.h"
#include <time.h>

#define BUFF_SIZE 1024

/*
 * Viet chuong trinh sv_client, cho phep nguoi dung nhap du lieu
 * la thong tin cua sinh vien: MSSV, Ho ten, ngay sinh, cpa.
 * Cac thong tin tren duoc dong goi va gui sang sv_server.
 * Dia chi cong server duoc nhap tu tham so dong lenh
 *
 * CMD: sv_client [IP_ADDRESS] [PORT]
*/

struct SinhVien {
    char id[20];
    char fullname[30];
    char birthday[11];
    double cpa;
};

/*
 * ham toString() cho struct SinhVien
 */
char *getInfo(struct SinhVien sv, struct sockaddr_in addr) {
    time_t currentTime;
    time(&currentTime); // Lấy thời gian hiện tại

    struct tm *localTime = localtime(&currentTime);
    char dateTimeString[50];
    strftime(dateTimeString, sizeof(dateTimeString), "%d-%m-%Y %H:%M:%S", localTime);

    char *info = (char *) malloc(100 * sizeof(char));

    for (int i = 0; i < strlen(sv.fullname); ++i) {
        if (sv.fullname[i] - ' ' == 0) {
            sv.fullname[i] = '_';
        }
    }
    sprintf(info, "%s %s %s%s%s %.2f", inet_ntoa(addr.sin_addr), dateTimeString, sv.id, sv.fullname, sv.birthday,
            sv.cpa);
    for (int i = 0; i < strlen(info); ++i) {
        if (info[i] == '\n') {
            info[i] = ' ';
        }
    }
    return info;
}


int main(int argc, char *argv[]) {

    if (argc != 3) {
        perror("Must have 2 parameter [IP_ADDRESS] [PORT]!\n");
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

    // Khai bao cau truc dia chi Server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    char buf[BUFF_SIZE];

    while (1) {

        int choose = 0;
        printf("\n1. Nhap thong tin sinh vien\n");
        printf("2. Thoat\n");
        printf("Nhap Lua chon: ");
        scanf("%d", &choose);
        getchar();

        if (choose == 1) {
            struct SinhVien sv;
            printf("\nNhap thong tin sinh vien theo thu tu MSSV, Ho ten, ngay sinh, cpa: \n");
            printf("MSSV: ");
            fgets(sv.id, 20, stdin);
            printf("Ho ten: ");
            fgets(sv.fullname, 30, stdin);
            printf("ngay sinh (dd-MM-yyyy): ");
            fgets(sv.birthday, 11, stdin);
            printf("CPA: ");
            scanf("%lf", &sv.cpa);
            while (sv.cpa > 4 || sv.cpa < 0) {
                printf("cpa phai thuoc tu 0 den 4, hay thu lai!\n");
                printf("CPA: ");
                scanf("%lf", &sv.cpa);
            }

            char *info = getInfo(sv, addr);
            strcpy(buf, info);

            if (sendto(client, buf, strlen(buf), 0, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
                perror("Server disconnect or failed when send() !\n");
                return 1;
            } else {
                printf("send success!\n");
                free(info);
                memset(buf, 0, sizeof(buf));
            }
        } else if (choose == 2) {
            break;
        } else {
            printf("Moi nhap lai!\n");
        }
    }

    close(client);
    return 1;
}