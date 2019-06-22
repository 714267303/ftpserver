#include <Myhead.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    MYSQL conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server;
    char *user;
    char *password;
    char *database;
}My_mysql;


int init_my_mysql();
int init_my_mysql_con(MYSQL **conn);

