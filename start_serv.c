#include "start_serv.h"
#include "pthread.h"
#include "stdio.h"
#include "winsock2.h"
#include "structs.h"
#include "database.h"

int offai = 0, user_num = 0, maxuser = 1, port;
user_info *database;

void *client_func(void *par) {
    SOCKET client = (SOCKET) par;
    char recieve[1024], transmit[1024];
    int info, base_id, check = 0;
    info = recv(client, recieve, 1024, 0);
    if (!info || info == SOCKET_ERROR) {
        closesocket(client);
        return (void *) 0;
    }
    if (strcmp(recieve, "log") == 0) {
        while (1) {
            info = recv(client, recieve, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                closesocket(client);
                return (void *) 0;
            }
            for (int i = 0; i < user_num; ++i)
                if (strcmp(recieve, database[i].login) == 0) {
                    check = 1;
                    base_id = i;
                    break;
                }
            if (check) {
                strcpy(transmit, "1");
                info = send(client, transmit, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    closesocket(client);
                    return (void *) 0;
                }
                break;
            } else {
                strcpy(transmit, "0");
                info = send(client, transmit, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    closesocket(client);
                    return (void *) 0;
                }
                check = 0;
            }
        }
        while (1) {
            info = recv(client, recieve, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                closesocket(client);
                return (void *) 0;
            }
            if (strcmp(recieve, database[base_id].pass) == 0) {
                strcpy(transmit, "1");
                info = send(client, transmit, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    closesocket(client);
                    return (void *) 0;
                }
                break;
            } else {
                strcpy(transmit, "0");
                info = send(client, transmit, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    closesocket(client);
                    return (void *) 0;
                }
            }
        }
        if (database[base_id].isonline) {
            closesocket(database[base_id].client);
            database[base_id].isonline = 0;
        }
        database[base_id].client = client;
        strcpy(transmit, "<cont>");
        info = send(client, transmit, 1024, 0);
        if (!info || info == SOCKET_ERROR) {
            closesocket(client);
            return (void *) 0;
        }
        for (int i = 0; i < database[base_id].cont_num; ++i) {
            strcpy(transmit, database[base_id].cont_list[i]);
            info = send(client, transmit, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                closesocket(client);
                return (void *) 0;
            }
        }
        strcpy(transmit, "<endcont>");
        info = send(client, transmit, 1024, 0);
        if (!info || info == SOCKET_ERROR) {
            closesocket(client);
            return (void *) 0;
        }

        strcpy(transmit, "<mes>");
        info = send(client, transmit, 1024, 0);
        if (!info || info == SOCKET_ERROR) {
            closesocket(client);
            return (void *) 0;
        }
        for (int i = 0; i < database[base_id].mes_num; ++i) {
            strcpy(transmit, "<start>");
            info = send(client, transmit, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                closesocket(client);
                return (void *) 0;
            }
            strcpy(transmit, database[base_id].mes_base[i].cont);
            info = send(client, transmit, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                closesocket(client);
                return (void *) 0;
            }
            for (int j = 0; j < database[base_id].mes_base[i].mes_num; ++j) {
                if (database[base_id].mes_base[i].suda_or_tuda[j] == 0)
                    strcpy(transmit, "0");
                else
                    strcpy(transmit, "1");
                info = send(client, transmit, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    closesocket(client);
                    return (void *) 0;
                }
                strcpy(transmit, database[base_id].mes_base[i].mes[j]);
                info = send(client, transmit, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    closesocket(client);
                    return (void *) 0;
                }
            }
            strcpy(transmit, "<end>");
            info = send(client, transmit, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                closesocket(client);
                return (void *) 0;
            }
        }
        strcpy(transmit, "<endmes>");
        info = send(client, transmit, 1024, 0);
        if (!info || info == SOCKET_ERROR) {
            closesocket(client);
            return (void *) 0;
        }
    } else if (strcmp(recieve, "reg") == 0) {
        char tmplogin[100] = {0};
        while (1) {
            info = recv(client, recieve, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                closesocket(client);
                return (void *) 0;
            }
            for (int i = 0; i < user_num; ++i)
                if (strcmp(recieve, database[i].login) == 0) {
                    check = 1;
                    break;
                }
            if (check) {
                strcpy(transmit, "0");
                info = send(client, transmit, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    closesocket(client);
                    return (void *) 0;
                }
                check = 0;
            } else {
                strcpy(transmit, "1");
                info = send(client, transmit, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    closesocket(client);
                    return (void *) 0;
                }
                strcpy(tmplogin, recieve);
                break;
            }
        }
        info = recv(client, recieve, 1024, 0);
        if (!info || info == SOCKET_ERROR) {
            closesocket(client);
            return (void *) 0;
        }
        ++user_num;
        base_id = user_num - 1;
        strcpy(database[base_id].login, tmplogin);
        strcpy(database[base_id].pass, recieve);
        database[base_id].client = client;
        database[base_id].cont_num = 0;
        database[base_id].isonline = 0;
        database[base_id].cont_max_num = 1;
        database[base_id].cont_list = (char **) calloc(1, sizeof(char *));
        database[base_id].cont_list[0] = (char *) calloc(100, sizeof(char));
        database[base_id].mes_num = 0;
        database[base_id].mes_max_num = 1;
        database[base_id].mes_base = (mes_info *) calloc(1, sizeof(mes_info));
        database[base_id].mes_base[0].mes_max_num = 1;
        database[base_id].mes_base[0].mes = (char **) calloc(1, sizeof(char *));
        database[base_id].mes_base[0].mes[0] = (char *) calloc(1000, sizeof(char));
        database[base_id].mes_base[0].suda_or_tuda = (char *) calloc(1, sizeof(char));
        if (user_num == maxuser) {
            maxuser *= 2;
            database = (user_info *) realloc(database, maxuser * sizeof(user_info));
        }
    } else {
        closesocket(client);
        return (void *) 0;
    }
    //основной цикл
    database[base_id].isonline = 1;
    while (1) {
        info = recv(client, recieve, 1024, 0);
        if (!info || info == SOCKET_ERROR) {
            database[base_id].isonline = 0;
            closesocket(client);
            return (void *) 0;
        }
        if (strcmp(recieve, "<addcont>") == 0) {
            info = recv(client, recieve, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                database[base_id].isonline = 0;
                closesocket(client);
                return (void *) 0;
            }
            check = 0;
            for (int i = 0; i < user_num; ++i)
                if (strcmp(recieve, database[i].login) == 0) {
                    check = 1;
                    break;
                }
            if (check) {
                strcpy(database[base_id].cont_list[database[base_id].cont_num], recieve);
                ++database[base_id].cont_num;
                if (database[base_id].cont_num == database[base_id].cont_max_num) {
                    database[base_id].cont_max_num *= 2;
                    database[base_id].cont_list = (char **) realloc(database[base_id].cont_list,
                                                                    database[base_id].cont_max_num * sizeof(char *));
                    for (int i = database[base_id].cont_num; i < database[base_id].cont_max_num; ++i)
                        database[base_id].cont_list[i] = (char *) calloc(100, sizeof(char));
                }
                strcpy(transmit, "ok");
            } else
                strcpy(transmit, "err");
            info = send(client, transmit, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                database[base_id].isonline = 0;
                closesocket(client);
                return (void *) 0;
            }
        } else if (strcmp(recieve, "<online>") == 0) {
            info = recv(client, recieve, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                database[base_id].isonline = 0;
                closesocket(client);
                return (void *) 0;
            }
            check = 0;
            for (int i = 0; i < user_num; ++i) {
                if (strcmp(database[i].login, recieve) == 0) {
                    if (database[i].isonline == 0)
                        strcpy(transmit, "0");
                    else
                        strcpy(transmit, "1");
                    check = 1;
                    break;
                }
            }
            if (!check)
                strcpy(transmit, "err");
            info = send(client, transmit, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                database[base_id].isonline = 0;
                closesocket(client);
                return (void *) 0;
            }
        } else if (strcmp(recieve, "<mes>") == 0) {
            //https://www.youtube.com/watch?v=n1N_obLPRys
            info = recv(client, recieve, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                database[base_id].isonline = 0;
                closesocket(client);
                return (void *) 0;
            }
            int chel_id;
            check = 0;
            for (int i = 0; i < user_num; ++i)
                if (strcmp(recieve, database[i].login) == 0) {
                    chel_id = i;
                    check = 1;
                    break;
                }
            if (check) {
                strcpy(transmit, "ok");
                info = send(client, transmit, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    database[base_id].isonline = 0;
                    closesocket(client);
                    return (void *) 0;
                }
                info = recv(client, recieve, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    database[base_id].isonline = 0;
                    closesocket(client);
                    return (void *) 0;
                }
                check = 0;
                int chel_in_mes;
                for (int i = 0; i < database[base_id].mes_num; ++i)
                    if (strcmp(database[base_id].mes_base[i].cont, database[chel_id].login) == 0) {
                        chel_in_mes = i;
                        check = 1;
                        break;
                    }
                if (check) {
                    strcpy(database[base_id].mes_base[chel_in_mes].mes[database[base_id].mes_base[chel_in_mes].mes_num], recieve);
                    database[base_id].mes_base[chel_in_mes].suda_or_tuda[database[base_id].mes_base[chel_in_mes].mes_num] = 1;
                    ++database[base_id].mes_base[chel_in_mes].mes_num;
                    if (database[base_id].mes_base[chel_in_mes].mes_num == database[base_id].mes_base[chel_in_mes].mes_max_num) {
                        database[base_id].mes_base[chel_in_mes].mes_max_num *= 2;
                        database[base_id].mes_base[chel_in_mes].suda_or_tuda = (char *) realloc(
                                database[base_id].mes_base[chel_in_mes].suda_or_tuda,
                                database[base_id].mes_base[chel_in_mes].mes_max_num * sizeof(char));
                        database[base_id].mes_base[chel_in_mes].mes = (char **) realloc(database[base_id].mes_base[chel_in_mes].mes,
                                                                                        database[base_id].mes_base[chel_in_mes].mes_max_num *
                                                                                        sizeof(char *));
                        for (int i = database[base_id].mes_base[chel_in_mes].mes_num;
                             i < database[base_id].mes_base[chel_in_mes].mes_max_num; ++i)
                            database[base_id].mes_base[chel_in_mes].mes[i] = (char *) calloc(1000, sizeof(char));
                    }
                    for (int i = 0; i < database[chel_id].mes_num; ++i)
                        if (strcmp(database[chel_id].mes_base[i].cont, database[base_id].login) == 0) {
                            chel_in_mes = i;
                            break;
                        }
                    strcpy(database[chel_id].mes_base[chel_in_mes].mes[database[chel_id].mes_base[chel_in_mes].mes_num], recieve);
                    database[chel_id].mes_base[chel_in_mes].suda_or_tuda[database[chel_id].mes_base[chel_in_mes].mes_num] = 0;
                    ++database[chel_id].mes_base[chel_in_mes].mes_num;
                    if (database[chel_id].mes_base[chel_in_mes].mes_num == database[chel_id].mes_base[chel_in_mes].mes_max_num) {
                        database[chel_id].mes_base[chel_in_mes].mes_max_num *= 2;
                        database[chel_id].mes_base[chel_in_mes].suda_or_tuda = (char *) realloc(
                                database[chel_id].mes_base[chel_in_mes].suda_or_tuda,
                                database[chel_id].mes_base[chel_in_mes].mes_max_num * sizeof(char));
                        database[chel_id].mes_base[chel_in_mes].mes = (char **) realloc(database[chel_id].mes_base[chel_in_mes].mes,
                                                                                        database[chel_id].mes_base[chel_in_mes].mes_max_num *
                                                                                        sizeof(char *));
                        for (int i = database[chel_id].mes_base[chel_in_mes].mes_num;
                             i < database[chel_id].mes_base[chel_in_mes].mes_max_num; ++i)
                            database[chel_id].mes_base[chel_in_mes].mes[i] = (char *) calloc(1000, sizeof(char));
                    }
                } else {
                    strcpy(database[base_id].mes_base[database[base_id].mes_num].cont, database[chel_id].login);
                    database[base_id].mes_base[database[base_id].mes_num].mes_num = 1;
                    database[base_id].mes_base[database[base_id].mes_num].mes_max_num = 2;
                    database[base_id].mes_base[database[base_id].mes_num].suda_or_tuda = (char *) calloc(2, sizeof(char));
                    database[base_id].mes_base[database[base_id].mes_num].mes = (char **) calloc(2, sizeof(char *));
                    database[base_id].mes_base[database[base_id].mes_num].mes[0] = (char *) calloc(1000, sizeof(char));
                    database[base_id].mes_base[database[base_id].mes_num].mes[1] = (char *) calloc(1000, sizeof(char));
                    database[base_id].mes_base[database[base_id].mes_num].suda_or_tuda[0] = 1;
                    strcpy(database[base_id].mes_base[database[base_id].mes_num].mes[0], recieve);
                    ++database[base_id].mes_num;
                    if (database[base_id].mes_num == database[base_id].mes_max_num) {
                        database[base_id].mes_max_num *= 2;
                        database[base_id].mes_base = (mes_info *) realloc(database[base_id].mes_base,
                                                                          database[base_id].mes_max_num * sizeof(mes_info));
                    }

                    strcpy(database[chel_id].mes_base[database[chel_id].mes_num].cont, database[chel_id].login);
                    database[chel_id].mes_base[database[chel_id].mes_num].mes_num = 1;
                    database[chel_id].mes_base[database[chel_id].mes_num].mes_max_num = 2;
                    database[chel_id].mes_base[database[chel_id].mes_num].suda_or_tuda = (char *) calloc(2, sizeof(char));
                    database[chel_id].mes_base[database[chel_id].mes_num].mes = (char **) calloc(2, sizeof(char *));
                    database[chel_id].mes_base[database[chel_id].mes_num].mes[0] = (char *) calloc(1000, sizeof(char));
                    database[chel_id].mes_base[database[chel_id].mes_num].mes[1] = (char *) calloc(1000, sizeof(char));
                    database[chel_id].mes_base[database[chel_id].mes_num].suda_or_tuda[0] = 1;
                    strcpy(database[chel_id].mes_base[database[chel_id].mes_num].mes[0], recieve);
                    ++database[chel_id].mes_num;
                    if (database[chel_id].mes_num == database[chel_id].mes_max_num) {
                        database[chel_id].mes_max_num *= 2;
                        database[chel_id].mes_base = (mes_info *) realloc(database[chel_id].mes_base,
                                                                          database[chel_id].mes_max_num * sizeof(mes_info));
                    }
                }
                if (database[chel_id].isonline) {
                    strcpy(transmit, "<mes> ");
                    strcat(transmit, database[base_id].login);
                    strcat(transmit, " ");
                    strcat(transmit, recieve);
                    send(database[chel_id].client, transmit, 1024, 0);
                }
            } else {
                strcpy(transmit, "err");
                info = send(client, transmit, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    database[base_id].isonline = 0;
                    closesocket(client);
                    return (void *) 0;
                }
            }
            //https://www.youtube.com/watch?v=snUtIsX2ZpI
        } else if (strcmp(recieve, "<off>") == 0) {
            database[base_id].isonline = 0;
            break;
        } else if (strcmp(recieve, "<changepas>") == 0) {
            info = recv(client, recieve, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                database[base_id].isonline = 0;
                closesocket(client);
                return (void *) 0;
            }
            strcpy(database[base_id].pass, recieve);
        } else if (strcmp(recieve, "<allcont>") == 0) {
            for (int i = 0; i < user_num; ++i) {
                strcpy(transmit, database[i].login);
                info = send(client, transmit, 1024, 0);
                if (!info || info == SOCKET_ERROR) {
                    database[base_id].isonline = 0;
                    closesocket(client);
                    return (void *) 0;
                }
            }
            strcpy(transmit, "<end>");
            info = send(client, transmit, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                database[base_id].isonline = 0;
                closesocket(client);
                return (void *) 0;
            }
        } else if (strcmp(recieve, "<mesforall>") == 0) {
            info = recv(client, recieve, 1024, 0);
            if (!info || info == SOCKET_ERROR) {
                database[base_id].isonline = 0;
                closesocket(client);
                return (void *) 0;
            }
            strcpy(transmit, "<globalmes> ");
            strcat(transmit, recieve);
            for (int i = 0; i < user_num; ++i)
                if (database[i].isonline)
                    send(database[i].client, transmit, 1024, 0);
        } else
            continue;
    }
    closesocket(client);
    return (void *) 0;
}

void *console_func(void *par) {
    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    char com[1000];
    while (1) {
        scanf("%s", com);
        if (strcmp(com, "exit") == 0) {
            offai = 1;
            int info = connect(client, (struct sockaddr *) &server, sizeof(server));
            closesocket(client);
            break;
        } else
            continue;
    }
    return (void *) 0;
}

void *save_func(void *par) {
    //сначала подождать это бибу, потом уже офать серв с сохранением
    return (void *) 0;
}

int createserv() {
    FILE *port_txt = fopen("port.txt", "r");
    if (!port_txt) {
        port_txt = fopen("port.txt", "w");
        printf("Enter the server port: ");
        scanf("%d", &port);
        fprintf(port_txt, "%d", port);
    } else {
        fscanf(port_txt, "%d", &port);
    }
    int info = base_load(&user_num, &maxuser, &database);
    if (info)
        return info;
    for (int i = 0; i < user_num; ++i) {
        printf("login: %s pass: %s\n", database[i].login, database[i].pass);
        printf("Online: %d\n", database[i].isonline);
        for (int j = 0; j < database[i].cont_num; ++j)
            printf("Cont: %s\n", database[i].cont_list[j]);
        for (int j = 0; j < database[i].mes_num; ++j) {
            printf("Mes with %s:\n", database[i].mes_base[j].cont);
            for (int k = 0; k < database[i].mes_base[j].mes_num; ++k) {
                if (database[i].mes_base[j].suda_or_tuda[k] == 0)
                    printf("%s: %s\n", database[i].mes_base[j].cont, database[i].mes_base[j].mes[k]);
                else
                    printf("Me: %s\n", database[i].mes_base[j].mes[k]);
            }
            printf("\n");
        }
    }
    SOCKET server, client;
    struct sockaddr_in localaddr, clientaddr;
    server = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (server == INVALID_SOCKET) {
        printf("Error create server\n");
        return -1;
    }
    localaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(port);
    if (bind(server, (struct sockaddr *) &localaddr, sizeof(localaddr)) == SOCKET_ERROR) {
        printf("Can't create server\n");
        return -2;
    } else {
        printf("Server is started on port: %d\n", port);
    }
    listen(server, SOMAXCONN);
    //подрубить абобуса
    pthread_t console_thread;
    pthread_create(&console_thread, NULL, &console_func, NULL);
    pthread_detach(console_thread);
    int size;
    while (1) {
        if (offai) {
            //отправить всем сообщение + сохранить данные
            //адекватно офнуть сокеты
            base_save(&user_num, &database);
            printf("The boss of this gym turned off the server\n");
            break;
        }
        size = sizeof(clientaddr);
        client = accept(server, (struct sockaddr *) &clientaddr, &size);
        if (client == INVALID_SOCKET) {
            printf("Error accept client 1\n");
            continue;
        } else {
            printf("Client is accepted\n");
            pthread_t client_thread;
            pthread_create(&client_thread, NULL, &client_func, (void *) client);
            pthread_detach(client_thread);
        }
    }
    base_free(&user_num, &database);
    closesocket(server);
    return 0;
}