#include <stdio.h>
#include "../include/SendFile.h"
#include "../include/command.h"
 int recvFileCircle(int serverFd,off_t dataLen,int new_fd);
long downloadFile(int client,char *md5)
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
    char filePath[100]={0};
    sprintf(filePath,"%s%s",DEFAULTCLIENT,data);
    int new_fd=open(filePath,O_CREAT|O_RDWR,0666);
    printf("filePath: %s\n",filePath);
    FUNC_CHEAK(-1,new_fd,"open");
    end=start=time(NULL);
    //发送已下载的文件大小
    struct stat file;
    Messg messg;
    memset(&messg,0,sizeof(messg));
    fstat(new_fd,&file);
    messg.dataLen=sizeof(off_t);
    off_t ofset=file.st_size;
   // ofset=ofset/4096;
   // ofset=ofset*4096;
    memcpy(messg.data,&ofset,messg.dataLen);
    ret=send(client,&messg,sizeof(Messg),0);
    FUNC_CHEAK(-1,ret,"send");
    
    dowloadsize+=file.st_size;
    printf("i have download %ld\n",dowloadsize);
    lseek(new_fd,dowloadsize,SEEK_CUR);
    //接收文件内容

    while(dowloadsize+trainL<filesizeCp)
    {
        //接收数据大小
        //接收数据内容
        recvFileCircle(client,trainL,new_fd);
        dowloadsize+=trainL;
        end=time(NULL);
        if(end-start>=1)
        {
            printf("\r%5.2f%%",(double)dowloadsize/filesizeCp*100);
            fflush(stdout);
            start=end;
        }
    }
    recvFileCircle(client,filesizeCp-dowloadsize,new_fd);
    printf("\r100.0%%");
    fflush(stdout);

    return filesizeCp;
}

