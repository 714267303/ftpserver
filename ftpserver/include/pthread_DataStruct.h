#include <Myhead.h>
#include "../include/user.h"
typedef struct
{
    int clientFd;
}client;

typedef  ArgCmd dataType;
typedef struct A
{
    dataType data;
    struct A *next;
}Node,*pNode;

typedef struct
{
    pNode head,tail;
    int num;
    int q_capacity; 
    pthread_cond_t con;
    pthread_mutex_t mu_id;
}Que,*pQue;

typedef struct
{
    int writeFd,readFd;
    short busy;
    pQue factory;
}My_thread,*p_thread;

typedef struct
{
    //pQue factory;
    My_thread threadData; 
}HandleData;

int thread_pool_Init(pQue factory,p_thread thread_pool,int thread_num,pthread_t *);
void* threadHandle(void *data);
void *putsHandle(void *data);
int InitSocket(int serverFd,char *Ip,char *port);
int Factory_Init(pQue factory,int capacity);
int puts_pool_Init(pQue factory,p_thread thread_pool,int thread_num,pthread_t *thread_id);

int InsertQue(pQue factory,dataType client);
int DeleteQue(pQue factory ,dataType *client);
