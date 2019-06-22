#include <Myhead.h>
#define FILENAME "file1"
#define trainL 4096
typedef struct
{
    pid_t pid;
    int fd;
    short busy;
}ProcessData;
typedef struct
{
    int dataLen;
    char data[trainL];
}Messg;
int childHandle(int );
int getFd(int ,int *);
int Init_Process(ProcessData*p,int num);
int  tranFile(int client,char *);
int sendFd(ProcessData*,int,int);
int sendCircle(int ,void *,off_t ,int );
int recvCircle(int serverFd,off_t dataLen,int new_fd);
int  tranBigTrain(int client,char *filename,char *realName);
int recvCircleSize(int serverFd,off_t *size,int eight);
int recv_Ls(int serverFd,int new_fd);
