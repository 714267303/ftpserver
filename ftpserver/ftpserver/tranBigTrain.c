#include <Myhead.h>
#include "../include/SendFile.h"
#include "../include/command.h"
int  tranBigTrain(int client,char *filename,char *realName)
{
    Messg messg; 
    memset(&messg,0,sizeof(messg));
    strcpy(messg.data,realName);
    //发送文件名和文件长度
    messg.dataLen=strlen(realName);
    int ret=send(client,&messg,4+messg.dataLen,0);//文件名长度和文件名
    FUNC_CHEAK(ret,-1,"send");
    //发送文件大小
    char path[100]={0};
    sprintf(path,"%s%s",DEFAULTPATH,filename);
    int fileFd=open(path,O_RDONLY);
    struct stat file;
    fstat(fileFd,&file);

    messg.dataLen=sizeof(off_t);
    memcpy(messg.data,&file.st_size,messg.dataLen);
    ret=send(client,&messg,sizeof(int),0);
    FUNC_CHEAK(-1,ret,"send");
    int testFd=open("test",O_CREAT|O_RDWR,0666);
    ret=send(client,messg.data,messg.dataLen,testFd);
    //接收已下载文件的大小
    recv(client,&messg,sizeof(Messg),0);
    //发送文件内容
    long offset=0;
    memcpy(&offset,&messg.data,8);
    printf("you have download %ld\n",offset);
    lseek(fileFd,offset,SEEK_CUR);  
    char *pfile=(char *)mmap(NULL,file.st_size,PROT_READ,MAP_SHARED,fileFd,0);
    while(offset+trainL<file.st_size)
    {
        ret=sendCircle(client,pfile+offset,trainL,testFd);
        offset+=trainL;
        if(ret==-1)
        {
            return -1;
        }
    }
    //发送最后一趟数据
    off_t finalDatalen=file.st_size-offset;
    sendCircle(client,pfile+offset,finalDatalen,0);
    //发送结束标记
    finalDatalen=0;
    send(client,&finalDatalen,sizeof(off_t),0);   
    return 1;
}
