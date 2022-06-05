#ifndef PROJECT3_SERV_DATABASE_H
#define PROJECT3_SERV_DATABASE_H

#include "structs.h"

int base_load(int *user_num, int *maxuser, user_info **database);

int base_save(const int *user_num, user_info **database);

void base_free(const int *user_num, user_info **database);

#endif
