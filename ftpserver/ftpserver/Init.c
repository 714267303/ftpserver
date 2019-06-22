#include <Myhead.h>
#include "../include/database.h"
#include "../include/pthread_DataStruct.h"
#include "../include/SendFile.h"
#include "../include/user.h"
#include "../include/command.h"

extern Q_Cal_TIME alar;
extern tk_table table[LINKNUM];//
extern uId_client u_c[LINKNUM];//
extern int clientNum;
extern pthread_mutex_t mutex;

int deleteClient(tk_table *table,uId_client *u_c,int uId); 
int queryMaxCode(int uId);
int IsNuLLInSQL(char *querySql);
int getMd5SQL(char *querySql,char *md5);
int RecordFunc(int uId,My_command *cmd);
int find_timer_Node(int *uId,int pos,int obj);
int Factory_Init(pQue factory,int capacity)
{
    pthread_cond_init(&factory->con,NULL);
    pthread_mutex_init(&factory->mu_id,NULL);
    factory->num=0;
    factory->q_capacity=capacity;
    factory->head=factory->tail=NULL;
    return 0;
}
void sighandle(int sig)
{
    //指针加一
    alar.cur=(alar.cur+1)%30;
    if(alar.tm_Cal_Time[alar.cur].pos!=0)
    {
        int i=0;
        while(i<alar.tm_Cal_Time[alar.cur].pos)
        {
            pthread_mutex_lock(&mutex);
            deleteClient(table,u_c,alar.tm_Cal_Time[alar.cur].uId[i]);
            pthread_mutex_unlock(&mutex);
            i++;
        }
        alar.tm_Cal_Time[alar.cur].pos=0;
    }
//    printf("alrm coming\n");
}
void regulate_alar(int uId)
{
    //查找发生响应的uId,
    int i=0;
    for(;i<30;i++)
    {
        if(find_timer_Node(alar.tm_Cal_Time[i].uId,alar.tm_Cal_Time[i].pos,uId)==1)
        {
            alar.tm_Cal_Time[i].pos--;
            int pre=(alar.cur-1+30)%30;
            int *pos=&alar.tm_Cal_Time[pre].pos;
            alar.tm_Cal_Time[pre].uId[*pos]=uId;
            (*pos)++;
            return;
        };
    }
}
int delete_from_alar(int uId)
{
    int i=0;
    for(;i<30;i++)
    {
        if(find_timer_Node(alar.tm_Cal_Time[i].uId,alar.tm_Cal_Time[i].pos,uId)==1)
        {
            alar.tm_Cal_Time[i].pos--;
            return 1;
        };
    }
    return -1;
}
int find_timer_Node(int *uId,int pos,int obj)
{
    int i=0;
    while(i<pos)
    {
        if(uId[i]==obj)
        {
            int j=i;
            for(;j<pos-1;j++)
            {
                uId[j]=uId[j+1];
            }
            return 1;
        }
        i++;
    }
    return -1;
}
void add_to_alar(int uId)
{
     int pre=(alar.cur-1+30)%30;
     int *pos=&alar.tm_Cal_Time[pre].pos;
     alar.tm_Cal_Time[pre].uId[*pos]=uId;
     (*pos)++;
}
void InitTime(int tv_sec,int tv_usec)
{
    signal(SIGALRM,sighandle);
    alarm(0);
    struct itimerval tm;
    tm.it_value.tv_sec=tv_sec;
    tm.it_value.tv_usec=tv_usec;
    tm.it_interval.tv_sec=tv_sec;
    tm.it_interval.tv_usec=tv_usec;
    if(-1==setitimer(ITIMER_REAL,&tm,NULL))
    {
        perror("setitimer");
        exit(-1);
    };
    memset(&alar,0,sizeof(alar));
}
int thread_pool_Init(pQue factory,p_thread thread_pool,int thread_num,pthread_t *thread_id)
{
    int i=0;
    int fd1[2];
    for(;i<thread_num;i++)
    {
        // pipe(fd1);
        socketpair(AF_LOCAL,SOCK_STREAM,0,fd1);
        thread_pool[i].busy=0;
        thread_pool[i].writeFd=fd1[1];//主线程写
        thread_pool[i].readFd=fd1[0];//主线程读
        //      thread_pool[i].childFdWrite=fd2[1];
        //      thread_pool[i].childFdRead=fd2[1];
        thread_pool[i].factory=factory;
        pthread_create(&thread_id[i],NULL,threadHandle,&thread_pool[i]);    
    }
    return 1;
}
int puts_pool_Init(pQue factory,p_thread thread_pool,int thread_num,pthread_t *thread_id)
{
    int i=0;
    int fd1[2];
    for(;i<thread_num;i++)
    {
        // pipe(fd1);
        socketpair(AF_LOCAL,SOCK_STREAM,0,fd1);
        thread_pool[i].busy=0;
        thread_pool[i].writeFd=fd1[1];//主线程写
        thread_pool[i].readFd=fd1[0];//主线程读
        //      thread_pool[i].childFdWrite=fd2[1];
        //      thread_pool[i].childFdRead=fd2[1];
        thread_pool[i].factory=factory;
        pthread_create(&thread_id[i],NULL,putsHandle,&thread_pool[i]);    
    }
    return 1;
}
void *threadHandle(void *data)
{
    char buf[1]={0};
    p_thread p=(p_thread)data;
    int ret;

    ArgCmd argcmd;
    while(1)
    {
        ret=read(p->writeFd,&argcmd,sizeof(dataType));
        FUNC_CHEAK_NORETURN(-1,ret,"read");
        //--------------------------------------------//
        //向客户端传输文件
        //查找数据库，查看文件名对应的md5码,发送查询结果
        //只能下载当前目录的文件
        char sql1[120]="select md5 from file where filename='";
        char sql[300]={0};
        sprintf(sql,"%s%s%s%s%d%s%d",sql1,argcmd.cmd.parameter,"'"," and uId=",argcmd.cmd.user.uId," and precode=",argcmd.cmd.user.cur_Path);
        char md5[33]={0};
        ret=getMd5SQL(sql,md5);
        //发送查询结果
        if(ret==-1)
        {
            send(argcmd.client,&ret,4,0); 
            goto over;
        };
        send(argcmd.client,&ret,4,0); 
        //传输文件内容
        
        printf("working\n");
        printf("arg:%d,%s\n",argcmd.cmd.user.uId,argcmd.cmd.parameter);
        tranBigTrain(argcmd.client,md5,argcmd.cmd.parameter);
        RecordFunc(argcmd.cmd.user.uId,&argcmd.cmd);
        close(argcmd.client);
        //--------------------------------------------//
over:
        ret=write(p->writeFd,buf,1);
        
        FUNC_CHEAK_NORETURN(-1,ret,"read");
    }
}
void *putsHandle(void *data)
{
    char buf[1]={0};
    p_thread p=(p_thread)data;
    int ret;

    ArgCmd argcmd;
    My_user user;
    My_command cmd;

    char md5[MD5_STR_LEN+1]={0};
    long size=0;
    char type ='-';
    while(1)
    {
        ret=read(p->writeFd,&argcmd,sizeof(dataType));
        //----------------------------------------------
        FUNC_CHEAK_NORETURN(-1,ret,"read");
        cmd=argcmd.cmd;
        user=cmd.user;
        //向接收客户端传输的文件
        //1.接收md5码
        recv(argcmd.client,md5,MD5_STR_LEN,0);
        printf("server ac md5:%s\n",md5);
        //2.发送判断值
        ret=FindMd5(md5,&size);
        send(argcmd.client,&ret,4,0);
        printf("send ret=%d*****************\n",ret);
        //3.自行判断
        if(ret==-1)//数据库不存在
        {
            printf("working\n");
            size=putsFileServer(argcmd.client,md5,user.uId);
            close(argcmd.client);//四次挥手
        }
        //加入数据库
        if(size!=-1)
        {
            int precode=user.cur_Path;

            MYSQL *conn;
            init_my_mysql_con(&conn);
            char insertQuery[30]="insert into  file values(";
            char query2[300]={0};
            sprintf(query2,"%s%d%s%s%s%s%s%s%s%s%s%d%s%ld%s%d%s%s%c%s%s",insertQuery,precode,",","'",cmd.parameter,"'",",","'",md5,"'",",",user.uId,",",size,",",queryMaxCode(user.uId)+1,",","'",type,"'",")");
            printf("%s\n",query2);
            ret=mysql_query(conn,query2);
            FUNC_CHEAK_NORETURN(-1,ret,"command.c insert:");
        }                    
        //---------------------------------
        RecordFunc(argcmd.cmd.user.uId,&argcmd.cmd);
        ret=write(p->writeFd,buf,1);
        FUNC_CHEAK_NORETURN(-1,ret,"read");
    }
}
long putsFileServer(int client,char *md5,int uId)
{ 
    char data[trainL]={0};
    int ret,dataLen;
    off_t filesize,filesizeCp;
    off_t dowloadsize=0;
    time_t start,end;
    //接收文件名
    ret=recv(client,&dataLen,sizeof(int),0);//接受长度和文件名
    FUNC_CHEAK(-1,ret,"recv");
    ret=recv(client,data,dataLen,0);
    FUNC_CHEAK(-1,ret,"recv");
    printf("filenamelen:%d\tfilename:%s\t",dataLen,data);
    //接受文件大小
    ret=recv(client,&dataLen,sizeof(int),0);
    FUNC_CHEAK(ret,-1,"recv");
    ret=recv(client,&filesize,dataLen,0);
    FUNC_CHEAK(ret,-1,"recv");
    printf("filesize=%ld\n",filesize);
    filesizeCp=filesize;
    //创建文件
    char filePath[400]={0};
    char oldPath[410]={0};
    sprintf(filePath,"%s%s%s%d",DEFAULTPATH,md5,".",uId);
    sprintf(oldPath,"%s%s",DEFAULTPATH,md5);
    int new_fd=open(filePath,O_CREAT|O_RDWR,0666);
    printf("filePath: %s\n",filePath);
    FUNC_CHEAK(-1,new_fd,"open");


    end=start=time(NULL);
    while(dowloadsize+trainL<filesizeCp)
    {
        //接收数据大小
        //接收数据内容
        recvCircle(client,trainL,new_fd);
        dowloadsize+=trainL;
        end=time(NULL);
        if(end-start>=1)
        {
            printf("\r%5.2f%%",(double)dowloadsize/filesizeCp*100);
            fflush(stdout);
            start=end;
        }
    }
    ret=recvCircle(client,filesizeCp-dowloadsize,new_fd);
    if(ret!=-1)
    {
        dowloadsize=filesizeCp;
    }
    printf("\r100.0%%");
    fflush(stdout);

    if(dowloadsize<filesizeCp)
    {
        printf("transfile not successful\n");
        return -1;
    }
    else
    {
        int ret=rename(filePath,oldPath);
        FUNC_CHEAK_NORETURN(ret,-1,"rename*******************");
    }
    close(new_fd);
    return filesizeCp;
}
