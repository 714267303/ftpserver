#include <Myhead.h>
#include "../include/user.h"
#include "../include/command.h"

int RecordFunc(int uId,My_command *cmd)
{
    char opt[120]={0};
    char path[120]={0};
    time_t t;
    t=time(NULL);
    if(cmd->type==First)
        sprintf(opt,"%s  time:%s\n","connecting",ctime(&t));
    else if(cmd->type==Second)
    {
        return 1;
    }
    else
        sprintf(opt,"%s %s\t  time:%s\n",cmd->name,cmd->parameter,ctime(&t));
    sprintf(path,"%s%d",DEFAULTRECORDPATH,uId);
    int fd=open(path,O_APPEND|O_CREAT|O_RDWR,0666);
    FUNC_CHEAK(fd,-1,"open");
    int ret=write(fd,opt,strlen(opt));
    FUNC_CHEAK(ret,-1,"write operation\n");
    return 0;
}

