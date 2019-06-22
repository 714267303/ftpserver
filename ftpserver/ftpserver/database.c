#include "../include/database.h"
#include <Myhead.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>

    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
int init_my_mysql()
{
    char *server="localhost";
    char *user="root";
    char *password="123";
    char *database="project1";
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("erro connecting\n");
        return -1;
    }
    else
    {
    //    printf("connecting\n");
        return 1;
    }
};
int init_my_mysql_con(MYSQL **conn)
{
    char *server="localhost";
    char *user="root";
    char *password="123";
    char *database="project1";
    *conn=mysql_init(NULL);
    if(!mysql_real_connect(*conn,server,user,password,database,0,NULL,0))
    {
        printf("erro connecting\n");
        return -1;
    }
    else
    {
        printf("Mysql  connecting\n");
        return 1;
    }
};
