#include "stdio.h"
#include "winsock2.h"
#include "start_serv.h"

int main() {
    system("cls");
    printf("Yeba\n");
    printf("Server start up\n");
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(1, 1), &wsd)) {
        printf("Socket lib error\n");
        return 1;
    }
    return createserv();
}
