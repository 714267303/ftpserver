#include <Myhead.h>
#ifndef __USER_H__
#define __USER_H__
#define LINKNUM 10//服务器端连接数量
#define soltLen 10
#define USER_CMD 30
typedef struct
{
    int pos;
    int uId[LINKNUM];
}timer_Node;

typedef struct
{
    int cur;
    timer_Node tm_Cal_Time[30];
}Q_Cal_TIME;

typedef struct
{
    int uId;
    char username[20];
    char ciphertext[50];
    int cur_Path ;//当前路径
}My_user;

typedef struct
{
    int uId;
    char token[20];
}tk_table;
typedef struct
{
    My_user user;
    int client;
}uId_client;
typedef struct 
{
    char type;
    char name[10];
    char parameter[20];
    My_user user;
}My_command;

typedef struct
{
    My_command cmd;
    int client;
}ArgCmd;

typedef struct
{
    uId_client *u_c;
    int efd;
    char *solt;
    char *token;
    tk_table *table;
}ar,*parg;
//链表----------------------
int genSolt(char *solt);
int FindPasswd(int uId,char *ciphertext);
int SaveToken(int uId,char *token,tk_table*);
int FindToken(int uId,char *token,tk_table*);
int getSolt(int uId,char *solt);
int deleteClient(tk_table *table,uId_client *u_c,int uId);
int FindMd5(char *md5,long *size);
long downloadFile(int client,char *md5);
long putsFileServer(int client,char *md5,int uId);
int genToken(char *token);
#endif
//链表操作
