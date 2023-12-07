#ifndef USER_H
#define USER_H

#include <shared.h>

typedef struct user
{
    u8 uid;         // ID пользователя
    char login[12]; // Логин
}user_t;

#endif