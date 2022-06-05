#ifndef PROJECT3_SERV_STRUCTS_H
#define PROJECT3_SERV_STRUCTS_H

#include "winsock2.h"

typedef struct mes_info {
    char cont[100];
    int mes_num, mes_max_num;
    char *suda_or_tuda;//0-сюда 1-туда
    char **mes;
} mes_info;

typedef struct user_info {
    char login[100], pass[100];
    char isonline;//0-нет 1 - да
    SOCKET client;
    int cont_num, mes_num, cont_max_num, mes_max_num;
    char **cont_list;
    mes_info *mes_base;
} user_info;
#endif
