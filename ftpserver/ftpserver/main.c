#include <Myhead.h>
#include "../include/pthread_DataStruct.h"
#include "../include/command.h"
#include "../include/user.h"
#include "../include/database.h"
extern int tk_num;
extern int clientNum;
extern int efd;
tk_table table[LINKNUM];//user --token
uId_client u_c[LINKNUM];//client --user
Q_Cal_TIME alar;//计时器
pthread_mutex_t mutex;//初始化一把锁,用来锁table和u_c
int main(int num,char *argv[])
{
    //初始化锁
    pthread_mutex_init(&mutex,NULL);
    InitTime(1,0);
    //
    int threadNum=atoi(argv[3]);
    int capacity=atoi(argv[4]);
    //初始化工厂
    pQue factory=(pQue )calloc(1,sizeof(Que));
    //capacity队列大小,
    Factory_Init(factory,capacity);
    //初始化下载线程池
    p_thread thread_pool=(p_thread )calloc((threadNum),sizeof(My_thread)); 
    pthread_t *thread_id=(pthread_t *)calloc((threadNum),sizeof(pthread_t));
    thread_pool_Init(factory,thread_pool,(threadNum),thread_id);
    //---------------------------------------------------------------------------//
    //
    //
    //
    //
    //---------------------------------------------------------------------------//
    //初始化上传线程池
    pQue putsFactory=(pQue)calloc(1,sizeof(Que)); 
    Factory_Init(putsFactory,capacity);
    p_thread puts_pool=(p_thread)calloc(threadNum,sizeof(My_thread));
    pthread_t *p_thread_id=(pthread_t*)calloc(threadNum,sizeof(pthread_t));
    puts_pool_Init(putsFactory,puts_pool,threadNum,p_thread_id);
    //----------------------------------------------------------------------------//
    //
    //
    //----------------------------------------------------------------------------//
    //初始化socket
    int serverFd=socket(AF_INET,SOCK_STREAM,0) ;
    InitSocket(serverFd,argv[1],argv[2]);
    //注册事件
    int i=0,retForepoll;
    efd=epoll_create(1);
    FUNC_CHEAK(efd,-1,"epoll_create");
    struct epoll_event event,*evs;
    evs=(struct epoll_event *)calloc(2*threadNum+1+LINKNUM,sizeof(struct epoll_event));

    for(i=0;i<threadNum;i++)
    {
        event.data.fd=thread_pool[i].readFd;
        event.events=EPOLLIN;
        retForepoll=epoll_ctl(efd,EPOLL_CTL_ADD,thread_pool[i].readFd,&event);
        FUNC_CHEAK(retForepoll,-1,"epoll_ctl");
        //------------------------------------------------------------------//
        event.data.fd=puts_pool[i].readFd;
        event.events=EPOLLIN;
        retForepoll=epoll_ctl(efd,EPOLL_CTL_ADD,puts_pool[i].readFd,&event);
        FUNC_CHEAK(retForepoll,-1,"epoll_ctl");
    }//此处共2*threadNum个描述符被监控
    event.events=EPOLLIN;
    event.data.fd=serverFd;
    retForepoll=epoll_ctl(efd,EPOLL_CTL_ADD,serverFd,&event);
    FUNC_CHEAK(retForepoll,-1,"epoll_ctl2");

    int ctl_Num,j,ret;
    struct sockaddr_in clientAddr;
    socklen_t addrlen=sizeof(struct sockaddr);
    pthread_t commandThread;
    // int client[100]={0};
    //client与uid需要一个对应关系
    char solt[10]={0};
    char token[20]={0};

    init_my_mysql();
    while(1)
    {
        ctl_Num=epoll_wait(efd,evs,2*threadNum+1+clientNum,-1);
        //FUNC_CHEAK(ctl_Num,-1,"epoll_wait");
        //FUNC_CHEAK_NORETURN(-1,ctl_Num,"epoll_wait");
        //printf("while1\n");
        for(i=0;i<ctl_Num;i++)
        {
            if(evs[i].data.fd==serverFd)
            {//54
                u_c[clientNum].client=accept(serverFd,(struct sockaddr*)&clientAddr,&addrlen);
                FUNC_CHEAK(-1, u_c[clientNum].client,"accept");
                //cmd , uc
                ar *ag=(ar *)calloc(1,sizeof(ar));
                ag->u_c=u_c;//此时u_c表中user为空
                ag->efd=efd;
                ag->solt=solt;
                ag->token=token;
                ag->table=table;//此时table里为空
                //ag.clientNum=&clientNum;
                pthread_create(&commandThread,NULL,Register,ag);
                printf("ip:%s port%d\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
                
            }
            for(j=0;j<threadNum;j++)
            {
                if(evs[i].data.fd==thread_pool[j].readFd)
                {
                    ret=read(thread_pool[j].readFd,&retForepoll,1);
                    FUNC_CHEAK(-1,ret,"read");
                    thread_pool[j].busy=0;
                    printf("relaxing\n");
                    //发送条件成立的事件
                    // pthread_cond_signal(&factory->con);
                    //从队列读取客户
                    dataType tempClient;
                    ret= DeleteQue(factory,&tempClient);
                    
                    //发送给子线程
                    printf("ret=%d\n",ret);
                    if(ret==1)
                    {
                        thread_pool[j].busy=1;
                        ret=write(thread_pool[j].readFd,&tempClient,sizeof(dataType));
                        FUNC_CHEAK(-1,ret,"write");
                    }

                    break;
                }
            }
            for(j=0;j<threadNum;j++)
            {
                if(evs[i].data.fd==puts_pool[j].readFd)
                {
                    ret=read(puts_pool[j].readFd,&retForepoll,1);
                    FUNC_CHEAK(-1,ret,"read");
                    puts_pool[j].busy=0;
                    printf("relaxing\n");
                    //发送条件成立的事件
                    // pthread_cond_signal(&factory->con);
                    //从队列读取客户
                    dataType tempClient;
                    ret= DeleteQue(putsFactory,&tempClient);
                    //发送给子线程
                    //printf("ret=%d\n",ret);
                    if(ret==1)
                    {
                        puts_pool[j].busy=1;
                        ret=write(puts_pool[j].readFd,&tempClient,sizeof(dataType));
                        FUNC_CHEAK(-1,ret,"write");
                    }

                    break;
                }
            }
            for(j=0;j<clientNum;j++)
            {
                if(u_c[j].client==evs[i].data.fd) 
                {
                    //接收命令
                    //sleep(5);
                    ArgCmd *argcmd=(ArgCmd *)calloc(1,sizeof(ArgCmd));
                    argcmd->client=u_c[j].client;
                    recv(u_c[j].client,&argcmd->cmd,sizeof(My_command),0);
                    argcmd->cmd.user=u_c[j].user;
                    printf("uId %d,%d\n",argcmd->cmd.user.uId,argcmd->cmd.type);

                    regulate_alar(argcmd->cmd.user.uId);
                    //判断命令类型
                    if(argcmd->cmd.type==0)
                    {
                        //说明断开了
                        deleteClient(table,u_c,argcmd->cmd.user.uId);
                        delete_from_alar(argcmd->cmd.user.uId);
                        break;
                    }
                    switch(argcmd->cmd.type)
                    {
                    case 3://普通命令
                        pthread_create(&commandThread,NULL,commandFunc,argcmd);
                        break;
                    case 2://下载
                        {
                            //上传下载要先取消监听
                            deleteClient(table,u_c,argcmd->cmd.user.uId);
                            //假设要下载的文件存在
                            int t=0;
                            for(t=0;t<threadNum;t++)
                            {
                                if(thread_pool[t].busy==0)
                                {
                                    thread_pool[t].busy=1;
                                    ret=write(thread_pool[t].readFd,argcmd,sizeof(dataType));
                                    FUNC_CHEAK(ret,-1,"write");
                                    break;
                                }
                            }
                            if(t==threadNum)
                            {
                                InsertQue(factory,*argcmd);
                            }
                            free(argcmd);
                        }
                        break;
                    case 1://上传
                        {
                            //上传下载要先取消监听
                            printf("delete uid:%d\n",argcmd->cmd.user.uId);
                            deleteClient(table,u_c,argcmd->cmd.user.uId);
                            int t=0;
                            for(t=0;t<threadNum;t++)
                            {
                                if(puts_pool[t].busy==0)
                                {
                                    printf("ip:%s port%d\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
                                    puts_pool[t].busy=1;
                                    ret=write(puts_pool[t].readFd,argcmd,sizeof(dataType));
                                    FUNC_CHEAK(ret,-1,"write");
                                    break;
                                }
                            }
                            if(t==threadNum)
                            {
                                InsertQue(putsFactory,*argcmd);
                            }
                            free(argcmd);
                            //接收My_file结构体
                            //判断md5码是否存在
                            //若不存在
                            //接收文件
                            //存在则不接受
                        }
                        break;
                    }
                }
            }
        }
    };
    return 0;
}

