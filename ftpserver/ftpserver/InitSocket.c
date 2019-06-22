#include <Myhead.h>


int InitSocket(int serverFd,char *Ip,char *port)
{
    in_addr_t ip=inet_addr(Ip);
    uint16_t p=htons(atoi(port));
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_addr.s_addr=ip;
    addr.sin_port=p;
    addr.sin_family=AF_INET;
    int ret=bind(serverFd,(struct sockaddr*)&addr,sizeof(struct sockaddr));
    FUNC_CHEAK(ret,-1,"bind");
    ret=listen(serverFd,10);
    FUNC_CHEAK(-1,ret,"listen");
    return 1;
}
