#include <Myhead.h>
#include "../include/user.h"
#include "../include/database.h"
extern MYSQL *conn;
extern MYSQL_RES *res;
extern MYSQL_ROW row;
int tk_num=0;
int clientNum=0;
int efd=0;
int deleteClient(tk_table *table,uId_client *u_c,int uId)
{
    struct epoll_event event;  
    int i=tk_num-1,j=0;
    for(i=tk_num-1;i>=0;i--)
    {
        if(table[i].uId==uId)
        {
            j=i;
            while(j+1<tk_num)
            {
                table[j].uId=table[j+1].uId;
                strncpy(table[j].token,table[j+1].token,20);
                j++;
            }
            tk_num--;
            break;
        }
    }
    for(i=clientNum-1;i>=0;i--)
    {
        if(u_c[i].user.uId==uId)
        {
            event.data.fd=u_c[i].client;
            printf("------------------------delete epoll_ctl\n");
            j=i;
            while(j+1<clientNum)
            {
                u_c[j].client=u_c[j+1].client;
                u_c[j].user=u_c[j+1].user;
                j++;
            }
            clientNum--;
            break;
        }
    }
    event.events=EPOLLIN;
    int ret=epoll_ctl(efd,EPOLL_CTL_DEL,event.data.fd,&event);
    FUNC_CHEAK(ret,-1,"epoll_ctl");
};
int genSolt(char *solt)
{
    //随机获取盐值
    srand(time(NULL));
    int i=0,cur=3;
    for(i=0;i<6;i++)
    {
        int flag=rand()%3;
        switch(flag)
        {
        case 0:
            solt[cur+i]=rand()%26+'A';
            break;
        case 1:
            solt[cur+i]=rand()%26+'a';
            break;
        case 2:
            solt[cur+i]=rand()%10+'0';
            break;
        }
    }
    strncpy(solt,"$6$",3);
    solt[soltLen-1]='$';
    printf("%s\n",solt);
    return 1;
}; 
int getSolt(int uId,char *solt)
{
    init_my_mysql();
    char query[30]="select * from user where uId=";
    char query1[300]={0};
    sprintf(query1,"%s%d",query,uId);
    int ret=mysql_query(conn,query1);
//    printf("%s\n",query1);
    if(ret)
    {
        printf("solt erro sql\n");
    }
    else
    {
        res=mysql_use_result(conn);
        if(res)
        {
            if((row=mysql_fetch_row(res))!=NULL)
            {
                strncpy(solt,row[2],10);
            }
           // printf("get solt:%s\n",solt);
            return 1;
        }
        else
        {
            printf("erro password\n");
        }
    }
    return -1;  
};
int genToken(char *token)
{
    int i;
    for(i=0;i<5;i++)
    {
        int flag=rand()%3;
        switch(flag)
        {
        case 0:
            token[i]=rand()%26+'A';
            break;
        case 1:
            token[i]=rand()%26+'a';
            break;
        case 2:
            token[i]=rand()%10+'0';
            break;
        }
    }
    return 1;
}
int SaveToken(int uId,char *token,tk_table *table)
{
    tk_table *cur=table;
    strncpy(cur[tk_num].token,token,19);
    cur[tk_num].uId=uId;
    printf("SaveToken uid:%d   token:%s\n",uId,token);
    tk_num++;
    return 1;
};
int FindToken(int uId,char *token,tk_table *table)
{
    int j=0;
    //printf("tk_num=%d\n",tk_num);
    for(;j<tk_num;j++)
    {
        if(table[j].uId==uId)
        {
            //printf("compare:%d\n",strncmp(table[j].token,token,20));
            if(strncmp(table[j].token,token,19)==0)
            {
                return 2;
            }
        }
    }
    return -1;
};
int FindPasswd(int uId,char *ciphertext)
{
    init_my_mysql();
    char query[30]="select * from user where uId=";
    char query1[300]={0};
    sprintf(query1,"%s%d%s%s%s%s",query,uId," and ciphertext='",ciphertext,"'",";");

    int ret=mysql_query(conn,query1);
   // printf("%s\n",query1);
    if(ret)
    {
        printf("erro sql %s\n",mysql_error(conn));
    }
    else
    {
        res=mysql_use_result(conn);
        if(res)
        {
           if((row=mysql_fetch_row(res))!=NULL)
                 return 1;
        }
        else
        {
            printf("erro password\n");
        }
    }
    return -1;
};

