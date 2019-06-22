#include <Myhead.h>
#include "../include/file.h"
#include "../include/SendFile.h"
int sendCircle(int client,void *data,off_t dataLen,int fd)
{
    int total=0,ret=0;
    //先发送数据大小
    //再发送文件内容 
    while(total<dataLen)
    {
        ret=send(client,(char *)data+total,dataLen-total,0);
        if(ret==-1)
        {
            return -1;
        }
      //  if(ret!=trainL)
      //  {
      //      printf("ret=%d\n",ret);
      //  }
        total+=ret;
    }
    return 1;
}
int recvCircle(int serverFd,off_t dataLen,int new_fd)
{
    int total=0,ret;
    char data[trainL]={0};
    //先接收数据大小
    
    //再接收文件内容 
    while(total<dataLen)
    {
        ret=recv(serverFd,data+total,dataLen-total,0);
        //write(new_fd,data,ret);
        if(ret==-1)
        {
            return -1;
        }
       // if(ret!=trainL)
       // {
       //     printf("ret=%d\n",ret);
       // }
        total+=ret;
        if(ret==0)
        {
            break;
        }
    }
    My_file file; 
    memcpy((char *)&file,data,dataLen);
    //printf("%5d %20s %32s %5d %10ld %5d %c\n",file.precode,file.filename,file.md5,file.uId,file.size,file.code,file.type);
    printf("%c%20s %10ld \n",file.type,file.filename,file.size);
    return 1;
}
int recv_Ls(int serverFd,int new_fd)
{
    int dataLen,ret;
    ret=recv(serverFd,&dataLen,4,0);
    if(dataLen==0)
    {
        return 0;
    }
    ret=recvCircle(serverFd,dataLen,new_fd);

    return ret;
};

int recvFileCircle(int serverFd,off_t dataLen,int new_fd)
{
    int total=0,ret;
    char data[trainL]={0};
    //先接收数据大小
    
    //再接收文件内容 
    while(total<dataLen)
    {
        ret=recv(serverFd,data,dataLen-total,0);
        write(new_fd,data,ret);
        if(ret==-1)
        {
            return -1;
        }
      //  if(ret!=trainL)
      //  {
      //      printf("ret=%d\n",ret);
      //  }
        total+=ret;
        if(ret==0)
        {
            break;
        }
    }
    return 1;
}
