#include <Myhead.h>
#include "../include/user.h"
#include "../include/database.h"
extern MYSQL *conn;
extern MYSQL_RES *res;
extern MYSQL_ROW row;
int genSolt(char *solt)
{
    //随机获取盐值
    srand(time(NULL));
    int i=0,cur=3;
    for(;i<soltLen;i++)
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
    char query[30]="select * from user where uId=";
    char query1[300]={0};
    sprintf(query1,"%s%d",query,uId);
    int ret=mysql_query(conn,query1);
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
            {
                strncpy(solt,row[2],10);
            }
        }
        else
        {
            printf("erro uId\n");
        }
    }
}
//  int SaveToken(int uId,char *token,tk_table *table)
//  {
//      tk_table *cur=table;
//      strcpy(cur[tk_num].token,token);
//      cur[tk_num++].uId=uId;
//      return 1;
//  };
//  int FindToken(int uId,char *token,tk_table *table)
//  {
//      int i=tk_num,j=0;
//      for(;j<i;j++)
//      {
//          if(table[j].uId==uId)
//          {
//              if(strcmp(table[j].token,token)==0)
//              {
//                  return 1;
//              }
//          }
//      }
//      return -1;
//  };
int FindPasswd(int uId,char *ciphertext)
{
    char query[30]="select * from user where uId=";
    char query1[300]={0};
    sprintf(query1,"%s%d%s%s%s",query,uId," and ciphertext='",ciphertext,"'");
    printf("query1 :%s\n",query1);
    int ret=mysql_query(conn,query1);
    if(ret)
    {
        printf("erro sql\n");
    }
    else
    {
        res=mysql_use_result(conn);
        if(res)
        {
            printf("correct uId and ciphertext\n");
        }
        else
        {
            printf("erro password\n");
        }
    }
};

