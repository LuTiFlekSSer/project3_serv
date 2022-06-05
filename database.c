#include "database.h"
#include "stdlib.h"
#include "structs.h"
#include "stdio.h"
#include "string.h"

int base_load(int *user_num, int *maxuser, user_info **database) {
    FILE *user_list = fopen("user_list.txt", "rw");
    if (!user_list) {
        printf("No user_list\n");
        return -3;
    }
    *database = (user_info *) calloc(1, sizeof(user_info));
    while (1) {
        char templog[100] = {0};
        fscanf(user_list, "%s", templog);
        if (feof(user_list))
            break;
        strcpy((*database)[(*user_num)].login, templog);
        ++(*user_num);
        if (*user_num == *maxuser) {
            (*maxuser) *= 2;
            (*database) = (user_info *) realloc((*database), (*maxuser) * sizeof(user_info));
        }
    }
//здесь загрузка файлов
    for (int i = 0; i < *user_num; ++i) {
        char tmpforuserfile[110] = {0};
        strcpy(tmpforuserfile, "database/");
        strcat(tmpforuserfile, (*database)[i].login);
        FILE *user = fopen(tmpforuserfile, "r");
        if (!user) {
            printf("No user file for %s\n", (*database)[i].login);
            free(*database);
            return -4;
        }
        char check = 0;
        (*database)[i].isonline = 0;
        (*database)[i].mes_max_num = 1;
        (*database)[i].mes_num = 0;
        while (1) {
            char tmpmes[1000] = {0};
            fscanf(user, "%s", tmpmes);
            if (feof(user))
                break;
            if (strcmp(tmpmes, "<pas>") == 0) {
                fscanf(user, "%s", tmpmes);
                if (strcmp(tmpmes, "<endpas>") == 0) {
                    printf("No pas for %s\n", (*database)[i].login);
                    free(*database);
                    return -5;
                }
                strcpy((*database)[i].pass, tmpmes);
                fscanf(user, "%s", tmpmes);
                if (strcmp(tmpmes, "<endpas>") != 0) {
                    printf("Wrong base format for %s\n", (*database)[i].login);
                    free(*database);
                    return -6;
                }
                continue;
            } else if (strcmp(tmpmes, "<cont>") == 0) {
                (*database)[i].cont_list = (char **) calloc(1, sizeof(char *));
                (*database)[i].cont_list[0] = (char *) calloc(100, sizeof(char));
                (*database)[i].cont_max_num = 1;
                (*database)[i].cont_num = 0;
                while (1) {
                    fscanf(user, "%s", tmpmes);
                    if (feof(user) || strcmp(tmpmes, "<endcont>") == 0)
                        break;
                    if (strcmp(tmpmes, "<mes>") == 0) {
                        printf("Wrong base format for %s\n", (*database)[i].login);
                        ++(*database)[i].cont_num;
                        base_free(user_num, database);
                        return -7;
                    } else {
                        strcpy((*database)[i].cont_list[(*database)[i].cont_num], tmpmes);
                        ++(*database)[i].cont_num;
                        if ((*database)[i].cont_num == (*database)[i].cont_max_num) {
                            (*database)[i].cont_max_num *= 2;
                            (*database)[i].cont_list = (char **) realloc((*database)[i].cont_list,
                                                                         (*database)[i].cont_max_num *
                                                                         sizeof(char *));
                            for (int j = (*database)[i].cont_num; j < (*database)[i].cont_max_num; ++j)
                                (*database)[i].cont_list[j] = (char *) calloc(100, sizeof(char));
                        }
                    }
                }
                continue;
            } else if (strcmp(tmpmes, "<mes>") == 0) {
                (*database)[i].mes_base = (mes_info *) calloc(1, sizeof(mes_info));
                while (1) {
                    fscanf(user, "%s", tmpmes);
                    if (strcmp(tmpmes, "<endmes>") == 0)
                        break;
                    if (strcmp(tmpmes, "<start>") == 0) {
                        fscanf(user, "%s", tmpmes);
                        (*database)[i].mes_base[(*database)[i].mes_num].mes = (char **) calloc(1, sizeof(char *));
                        (*database)[i].mes_base[(*database)[i].mes_num].mes[0] = (char *) calloc(1000, sizeof(char));
                        (*database)[i].mes_base[(*database)[i].mes_num].suda_or_tuda = (char *) calloc(1, sizeof(char));
                        strcpy((*database)[i].mes_base[(*database)[i].mes_num].cont, tmpmes);
                        (*database)[i].mes_base[(*database)[i].mes_num].mes_max_num = 1;
                        (*database)[i].mes_base[(*database)[i].mes_num].mes_num = 0;
                        fgetc(user);
                        while (1) {
                            int k = 0;
                            while (1) {
                                char symb;
                                symb = fgetc(user);
                                if (symb == '\n')
                                    break;
                                tmpmes[k++] = symb;
                            }
                            tmpmes[k] = '\0';
                            if (strcmp(tmpmes, "<end>") == 0)
                                break;
                            if (tmpmes[0] == '0') {
                                (*database)[i].mes_base[(*database)[i].mes_num].suda_or_tuda[(*database)[i].mes_base[(*database)[i].mes_num].mes_num] = 0;
                                for (int l = 2; l < k; ++l)
                                    (*database)[i].mes_base[(*database)[i].mes_num].mes[(*database)[i].mes_base[(*database)[i].mes_num].mes_num][
                                            l - 2] = tmpmes[l];
                            } else {
                                (*database)[i].mes_base[(*database)[i].mes_num].suda_or_tuda[(*database)[i].mes_base[(*database)[i].mes_num].mes_num] = 1;
                                for (int l = 2; l < k; ++l)
                                    (*database)[i].mes_base[(*database)[i].mes_num].mes[(*database)[i].mes_base[(*database)[i].mes_num].mes_num][
                                            l - 2] = tmpmes[l];
                            }
                            ++(*database)[i].mes_base[(*database)[i].mes_num].mes_num;
                            if ((*database)[i].mes_base[(*database)[i].mes_num].mes_num ==
                                (*database)[i].mes_base[(*database)[i].mes_num].mes_max_num) {
                                (*database)[i].mes_base[(*database)[i].mes_num].mes_max_num *= 2;
                                (*database)[i].mes_base[(*database)[i].mes_num].mes = (char **) realloc(
                                        (*database)[i].mes_base[(*database)[i].mes_num].mes,
                                        (*database)[i].mes_base[(*database)[i].mes_num].mes_max_num *
                                        sizeof(char *));
                                (*database)[i].mes_base[(*database)[i].mes_num].suda_or_tuda = (char *) realloc(
                                        (*database)[i].mes_base[(*database)[i].mes_num].suda_or_tuda,
                                        (*database)[i].mes_base[(*database)[i].mes_num].mes_max_num *
                                        sizeof(char));
                                for (int j = (*database)[i].mes_base[(*database)[i].mes_num].mes_num;
                                     j < (*database)[i].mes_base[(*database)[i].mes_num].mes_max_num; ++j) {
                                    (*database)[i].mes_base[(*database)[i].mes_num].mes[j] = (char *) calloc(1000,
                                                                                                             sizeof(char));
                                }
                            }
                        }
                        ++(*database)[i].mes_num;
                        if ((*database)[i].mes_num == (*database)[i].mes_max_num) {
                            (*database)[i].mes_max_num *= 2;
                            (*database)[i].mes_base = (mes_info *) realloc((*database)[i].mes_base,
                                                                           (*database)[i].mes_max_num * sizeof(mes_info));
                        }
                    }
                }
            } else
                break;
            check = 1;
        }
        if (!check) {
            printf("Wrong base format for %s\n", (*database)[i].login);
            base_free(user_num, database);//возможно будет крашится при разных обстоятельствах
            return -7;
        }
        fclose(user);
    }
    fclose(user_list);
    return 0;
}

int base_save(const int *user_num, user_info **database) {
    FILE *user_list = fopen("user_list.txt", "w");
    for (int i = 0; i < (*user_num); ++i) {
        fprintf(user_list, "%s\n", (*database)[i].login);
        char tmpforuserfile[110] = {0};
        strcpy(tmpforuserfile, "database/");
        strcat(tmpforuserfile, (*database)[i].login);
        FILE *user = fopen(tmpforuserfile, "w");
        fprintf(user, "<pas>\n%s\n<endpas>\n", (*database)[i].pass);
        fprintf(user, "<cont>\n");
        for (int j = 0; j < (*database)[i].cont_num; ++j)
            fprintf(user, "%s\n", (*database)[i].cont_list[j]);
        fprintf(user, "<endcont>\n<mes>\n");
        for (int j = 0; j < (*database)[i].mes_num; ++j) {
            fprintf(user, "<start>\n%s\n", (*database)[i].mes_base[j].cont);
            for (int k = 0; k < (*database)[i].mes_base[j].mes_num; ++k)
                if ((*database)[i].mes_base[j].suda_or_tuda[k] == 0)
                    fprintf(user, "0 %s\n", (*database)[i].mes_base[j].mes[k]);
                else
                    fprintf(user, "1 %s\n", (*database)[i].mes_base[j].mes[k]);
            fprintf(user, "<end>\n");
        }
        fprintf(user, "<endmes>\n");
        fclose(user);
    }
    fclose(user_list);
    return 0;
}

void base_free(const int *user_num, user_info **database) {
    for (int i = 0; i < (*user_num); ++i) {
        for (int j = 0; j < (*database)[i].mes_num; ++j) {
            free((*database)[i].mes_base[j].suda_or_tuda);
            for (int k = 0; k < (*database)[i].mes_base[j].mes_max_num; ++k)
                free((*database)[i].mes_base[j].mes[k]);
            free((*database)[i].mes_base[j].mes);
        }
        free((*database)[i].mes_base);
        for (int j = 0; j < (*database)[i].cont_max_num; ++j) {
            free((*database)[i].cont_list[j]);
        }
        free((*database)[i].cont_list);
    }
    free((*database));
}