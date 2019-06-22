#include <Myhead.h>
#include "../include/SendFile.h"
int  tranBigTrain(int client,char *filename,char *realName)
{
    Messg messg; 
    memset(&messg,0,sizeof(messg));
    strcpy(messg.data,filename);
    //发送文件名和文件长度
    messg.dataLen=strlen(filename);
    int ret=send(client,&messg,4+messg.dataLen,0);//文件名长度和文件名
    FUNC_CHEAK(ret,-1,"send");
    //发送文件大小
    int fileFd=open(filename,O_RDONLY);
    struct stat file;
    fstat(fileFd,&file);

    messg.dataLen=sizeof(off_t);
    memcpy(messg.data,&file.st_size,messg.dataLen);
    ret=send(client,&messg,sizeof(int),0);
    FUNC_CHEAK(-1,ret,"send");
    int testFd=open("test",O_CREAT|O_RDWR,0666);
    ret=send(client,messg.data,messg.dataLen,testFd);
    
    //发送文件内容
    off_t offset=0;
    time_t start,end;

    end=start=time(NULL);
    char *pfile=(char *)mmap(NULL,file.st_size,PROT_READ,MAP_SHARED,fileFd,0);
    while(offset+trainL<file.st_size)
    {
        ret=sendCircle(client,pfile+offset,trainL,testFd);
        offset+=trainL;
        end=time(NULL);
        if(ret==-1)
        {
            return -1;
        }
        if(end-start>=1)
        {
            printf("\r%5.2f%%",(double)offset/file.st_size*100);
            fflush(stdout);
            start=end;
        }
    }
    //发送最后一趟数据
    off_t finalDatalen=file.st_size-offset;
    sendCircle(client,pfile+offset,finalDatalen,0);
    //发送结束标记
    finalDatalen=0;
    send(client,&finalDatalen,sizeof(off_t),0);   
    printf("\r100.0%%");
    fflush(stdout);
    return 1;
}
