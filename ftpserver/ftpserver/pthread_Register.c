#include <Myhead.h>
#include "../include/command.h"
#include "../include/user.h"
#include "../include/database.h"
#include "../include/thread_create.h"
extern int clientNum;
extern pthread_mutex_t mutex;
int RecordFunc(int uId,My_command *cmd);
int setCode(My_user *user,uId_client *u_c);
void *Register(void *arg)
{
    parg ag=(parg)arg;
    int ret;
    int recvtotal=0;
    
    //int *clientNum=ag->clientNum;
    My_command cmd;
    uId_client *u_c=ag->u_c;//只有client，没有user需要赋值
    My_user user;//?
    int client=u_c[clientNum].client;
    char *solt=ag->solt;
    char *token=ag->token;

    int efd=ag->efd;
    tk_table *table=ag->table;

    memset(&cmd,0,sizeof(cmd));
    //接收命令1
    ret=recv(client,&cmd,sizeof(cmd),0);
    recvtotal+=sizeof(cmd);
    //接收用户名2
    ret=recv(client,&user,sizeof(My_user),0);
    //记录命令
    RecordFunc(user.uId,&cmd);
    //子线程直接就走下来了

    // printf("cmd type:%d,uid=%d\n",cmd.type,user.uId);
    recvtotal+=sizeof(My_user);
    if(cmd.type==First)
    {
        //生成盐值    查找盐值,生成盐值应该在注册部分
        getSolt(user.uId,solt);//genSolt(solt);/
        //发送盐值
        ret=send(client,solt,10,0);
        //接收密文3
        ret=recv(client,&user,sizeof(My_user),0);    
        recvtotal+=sizeof(My_user);
        printf("my cur_path is %d\n",user.cur_Path);
        //验证结果
        ret=-1;
        ret=FindPasswd(user.uId,user.ciphertext);
        if(ret==1) 
            printf("right passwd\n");
        else
            printf("erro passwd\n");
        //发送验证结果
        send(client,&ret,4,0);
        //接收token值4
        if(ret==1)
        {
            recv(client,token,20,0);    
            recvtotal+=20;
            //加入时间轮盘 
            add_to_alar(user.uId);
        }
        //printf("lens=%ld %s\n",strlen(token),token);
        //验证密文
    }
    else if(cmd.type==Second)
    {
        //code,precode值弄成第一次保存的值
        if(1==setCode(&user,u_c))
           printf("setCode success %d\n",user.cur_Path);
        //接收token值
        ret=recv(client,token,20,0);    
        //验证token值
        ret=-1;
        ret=FindToken(user.uId,token,table);
        if(ret==2) 
            printf("right token\n");
        else
            printf("strlen=%ld  erro token:%s\n",strlen(token),token);
    }
    else
    {
        printf("erro!!!\n");
    }
    // printf("recvtotal:%d\n",recvtotal);
    if(ret==1||ret==2)
    {
        //保存token值
        pthread_mutex_lock(&mutex);
        u_c[clientNum].user=user;
        SaveToken(user.uId,token,table);
        clientNum++;
        pthread_mutex_unlock(&mutex);
        struct epoll_event event;
        event.events=EPOLLIN; 
        event.data.fd=client;
        epoll_ctl(efd,EPOLL_CTL_ADD,client,&event);
        
        if(ret==2)
        {
            send(client,&ret,4,0);
            printf("pthread_Register adding\n");
        }
    }
    //deleteClient(table,u_c,user.uId); 
}
int setCode(My_user *user,uId_client *u_c)
{
   int i=0;

   for(;i<clientNum;i++)
   {
       if(u_c[i].user.uId==user->uId)
       {
           user->cur_Path=u_c[i].user.cur_Path;
           return 1;
       }
   }
   return -1;
}
