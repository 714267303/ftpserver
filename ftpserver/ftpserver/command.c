#include <Myhead.h>
#include "../include/command.h"
#include "mysql/mysql.h"
#include "../include/file.h"
#include "../include/user.h"
#include "../include/database.h"
#include "../include/SendFile.h"
int IsNuLLInSQL(char *querySql);
int getFileType(char *querySql);
int Compute_file_md5(const char *file_path, char *md5_str);
int DeleteSQL(char *querySql);
int getMd5SQL(char *querySql,char *md5);
int RecordFunc(int uId,My_command *cmd);
int getwords(char *path,char *name,int *pos);
int  modify_UC_table(int ,int);
int CD_Function(ArgCmd *p,int precode);
int REMOVE_Function(My_command *cmd,int *ret,int precode);
int queryMaxCode(int uId);
int getAllMd5SQL(char *querySql,char (*md5)[33],int precode,int uId);
int getDirNum(int uId,int precode,int *dirArry);
int dirNameSQL(char *querySql,char *name);
int REMOVE_Dir_Function(int uId,int precode);
int PWD_Function(int uId,int code,char (*name)[21],int *pos);
int setCode(My_user *user,uId_client *u_c);
extern tk_table table[LINKNUM];//
extern uId_client u_c[LINKNUM];//
extern int clientNum;

void * commandFunc(void *cmdArg)
{
    MYSQL_RES *res;
    MYSQL_ROW row;

    MYSQL *conn;
    ArgCmd *p=(ArgCmd *)cmdArg;
    My_file file;
    My_command cmd=p->cmd;
    int client =p->client;
    My_user user=cmd.user;
    int precode=user.cur_Path;

    if(strcmp(cmd.name,"ls")==0)
    {
        //发送ls,暂时不带参数
        char query[30]="select * from file where uId=";
        char query1[300]={0};
        char query2[30]="and precode=";
        sprintf(query1,"%s %d %s %d %s",query,user.uId,query2,precode,";");

        //   printf("excute ls\n");
        printf("%s\n",query1);
        //char path[50]={0};
        int ret=init_my_mysql_con(&conn);
        FUNC_CHEAK_NORETURN(-1,ret,"init_my_mysql_con");

        ret=mysql_query(conn,query1);

        unsigned int t=0;
        char attr[7][33];
        //    printf("-----------------------\n");
        Messg msg;
        if(ret)
        {
            printf("erro sql %s\n",mysql_error(conn));
        }
        else
        {
            res=mysql_use_result(conn);
            if(res)
            {   
                while((row=mysql_fetch_row(res))!=NULL)
                {
                    //              printf("excute ok\n");
                    memset(attr,0,sizeof(attr));
                    for(t=0;t<mysql_num_fields(res);t++)
                    {
                        strcpy(attr[t],row[t]);        
                    }
                    file.precode=atoi(attr[0]);
                    strcpy(file.filename,attr[1]);
                    strcpy(file.md5,attr[2]);
                    file.uId=atoi(attr[3]);
                    file.size=atol(attr[4]);
                    file.code=atoi(attr[5]);
                    file.type=attr[6][0];

                    msg.dataLen=sizeof(file);
                    memcpy(msg.data,&file,msg.dataLen);
                    My_file file2;
                    memcpy(&file2,msg.data,msg.dataLen);
                    printf("i will send\n%d %s %s %d %ld %d %c\n",file2.precode,file2.filename,file2.md5,file2.uId,file2.size,file2.code,file2.type);
                    sendCircle(client,&msg.dataLen,4,0);
                    sendCircle(client,msg.data,msg.dataLen,0);
                }
            }
        }
        //再发一节结束符
        printf("excute ok\n");
        msg.dataLen=0;
        sendCircle(client,&msg,4,0);
        RecordFunc(p->cmd.user.uId,&p->cmd);
        mysql_free_result(res);
        mysql_close(conn);
    }
    else if(strcmp(p->cmd.name,"cd")==0)
    {
        //切换当前路径
        CD_Function(p,precode);
    }
    else if(strcmp(p->cmd.name,"pwd")==0)
    {
        //显示当前路径
        int code,pos=0;
        char (*dirStack)[21]=(char (*)[21])calloc(MAXDEEP,21); 
        setCode(&user,u_c);
        code=user.cur_Path;
        //printf("my code now is:%d\n",code);
        PWD_Function(p->cmd.user.uId,code,dirStack,&pos);//(int uId,int code,char (*name)[21],int *pos) 
        char path[410]={0};
        while(pos>=0)
        {
            pos--;
            strcat(path,"\\");
            strcat(path,dirStack[pos]);
        }
        send(client,path,410,0);
    }
    else if(strcmp(p->cmd.name,"remove")==0)
    {
        int ret;
        //删除文件
        printf("remove\n");       
        //判读文件类型
        char delSql[150]={0};
        sprintf(delSql,"%s%s%s%s%d%s%d","select type  from file where filename='",p->cmd.parameter,"'"," and uId=",p->cmd.user.uId," and precode=",precode);
        ret=getFileType(delSql);
        if(ret=='d')
        {
            printf("dir\n");
            memset(delSql,0,sizeof(delSql));
            sprintf(delSql,"%s%s%s%s%d%s%d","select code  from file where filename='",p->cmd.parameter,"'"," and uId=",p->cmd.user.uId," and precode=",precode);
            int code=IsNuLLInSQL(delSql);
            memset(delSql,0,sizeof(delSql));
            sprintf(delSql,"%s%d","delete from file where code=",code);
            DeleteSQL(delSql);
            REMOVE_Dir_Function(p->cmd.user.uId,code);
        }
        else if(ret=='-')
        {
            printf("file\n");
            REMOVE_Function(&p->cmd,&ret,precode);//int REMOVE_Function(My_command *cmd,int *ret,int precode)
        }
        send(client,&ret,4,0);
    }
    else if(strcmp(p->cmd.name,"mkdir")==0)
    {
        //先查询数据库看该目录下是否有相同名字的目录
        char qSql[100]={0};
        sprintf(qSql,"%s%s%s%s%d%s%s","select precode from file where filename='",p->cmd.parameter,"'"," and uId=",p->cmd.user.uId," and type=","'d'");
        int ret=IsNuLLInSQL(qSql);
        send(client,&ret,4,0);
        if(ret!=-1)
        {
            //该目录下已经有同名文件夹
                //删除原数据库的数据,只能删除当前目录下的
                char delSql[150]={0};
                sprintf(delSql,"%s%s%s%s%d%s%s%s%d","delete from file where filename='",p->cmd.parameter,"'"," and uId=",p->cmd.user.uId," and type=","'d'"," and precode=",precode);
                DeleteSQL(delSql);
                //递归删除该目录下的文件,修改cmd的parameter和precode就行了,把md5码全部存储到数组里面然后递归删除
                //应该传入文件夹的code
                memset(delSql,0,sizeof(delSql));
                sprintf(delSql,"%s%s%s%s%d%s%d","select code  from file where filename='",p->cmd.parameter,"'"," and uId=",p->cmd.user.uId," and precode=",precode);
                int code=IsNuLLInSQL(delSql);
                REMOVE_Dir_Function(p->cmd.user.uId,code);   
        };  

        MYSQL *conn;
        init_my_mysql_con(&conn);
        char insertQuery[30]="insert into  file values(";
        char query2[300]={0};
        sprintf(query2,"%s%d%s%s%s%s%s%s%s%s%s%d%s%ld%s%d%s%s%c%s%s",insertQuery,precode,",","'",cmd.parameter,"'",",","'","","'",",",user.uId,",",(long)0,",",queryMaxCode(user.uId)+1,",","'",'d',"'",")");
        printf("%s\n",query2);
        ret=mysql_query(conn,query2);
        FUNC_CHEAK_NORETURN(-1,ret,"command.c insert:");
    }
    else 
    {

    }
    free(cmdArg);
    return NULL;
};
int FindMd5(char *md5,long *size)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    init_my_mysql_con(&conn);
    char query[30]="select * from file where md5=";
    char query1[200]={0};
    sprintf(query1,"%s%s%32s%s",query,"'",md5,"'");
    printf("%s\n",query1);



    int ret=mysql_query(conn,query1);
    if(ret)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }
    else
    {
        res=mysql_use_result(conn);
        if(res)
        {
            if((row=mysql_fetch_row(res))!=NULL)
            {
                //for(t=0;t<mysql_num_fields(res);t++)
                *size=atol(row[4]);
        mysql_free_result(res);
        mysql_close(conn);
                return 1;
            }
        }
    }
        mysql_free_result(res);
        mysql_close(conn);
    return -1;
}
int getwords(char *path,char *name,int *pos)
{
    int i=0;
    while(path[*pos]&&path[*pos]!='/')
    {
        name[i++]=path[*pos];
        *pos+=1;
    }
    if(path[*pos]=='/')
    {
        *pos+=1;
        return 1;
    }
    return 0;
}
int modify_UC_table(int precode,int uId)
{
    int i=0;
    for(;i<clientNum;i++)
    {
        if(u_c[i].user.uId==uId)
        {
            u_c[i].user.cur_Path=precode;
            break;
        }
    }
    return 1;
}
int CD_Function(ArgCmd *p,int precode)
{
    printf("cd\n");
    My_user user=p->cmd.user;
    int client=p->client;
    int pos=0,ret;
    char *path=p->cmd.parameter;
start:
    while(path[pos])
    {
        if(path[pos]=='.')
        {
            pos+=3;
            char sql[50]="select precode from file where code=";
            char precodeSql[120]={0};
            sprintf(precodeSql,"%s%d",sql,precode);
            precode=IsNuLLInSQL(precodeSql);
            if(precode==-1)
            {
                precode=0;
            }
        }
        else if(path[pos]=='/')
        {
            precode=0;
            pos+=1;
        }
        else
        {
            char dirName[21]={0};
            ret=getwords(path,dirName,&pos);//获取目录名,直到遇到/或者结束符
            printf("dirname:%s\n",dirName);
            char sql[50]="select code from file where filename='";
            char precodeSql[120]={0};
            sprintf(precodeSql,"%s%s%s%s%d%s",sql,dirName,"'"," and uId=",user.uId," and type='d'");//还需判断文件类型是否为目录
            precode=IsNuLLInSQL(precodeSql);
            if(precode==-1)
            {
                //说明没有这个目录,返回
                send(client,&precode,4,0);
                break;
            }
            if(ret)
            {
                goto start;
            } 

        }                                        
    }
    //如果能正常出来，那么说明cd 路径正确,修改u_c表中的code
    send(client,&precode,4,0);
    modify_UC_table(precode,user.uId);
}
int REMOVE_Function(My_command *cmd,int *ret,int precode)
{
    char querySql[100]={0};
    My_user user=cmd->user;
    sprintf(querySql,"%s%s%s %s%d%s%d","select * from file where filename='",cmd->parameter,"'","and uId=",user.uId," and precode=",precode);
    *ret=IsNuLLInSQL(querySql);
    if(*ret!=-1)
    {
        char md5[MD5_STR_LEN+1]={0};
        char path[120]={0};
        //根据文件名查找文件在数据库中对应的md5码
        char md5Sql[120]={0};
        sprintf(md5Sql,"%s%s%s%s%d","select md5 from file where filename='",cmd->parameter,"'"," and uId=",user.uId);
        *ret=getMd5SQL(md5Sql,md5);
        if(*ret==-1)
        {
            printf("getMd5SQL erro\n");
        }
        //查询链接数量
        memset(querySql,0,sizeof(querySql));
        sprintf(querySql,"%s%s%s","select count(md5) from file where md5='",md5,"'");
        sprintf(path,"%s%s",DEFAULTPATH,md5);
        *ret=IsNuLLInSQL(querySql);
        if(*ret==1)
        {
            unlink(path);
        }
        memset(querySql,0,sizeof(querySql));
        sprintf(querySql,"%s%s%s%s%d%s%d","delete from file where md5='",md5,"'"," and uId=",cmd->user.uId," and precode=",precode);
        DeleteSQL(querySql);
    }

}
int REMOVE_By_MD5(char *md5,int precode,int uId)
{
    char querySql[100]={0};
    int ret;
    char path[120]={0};
    //查询链接数量
    memset(querySql,0,sizeof(querySql));
    sprintf(querySql,"%s%s%s","select count(md5) from file where md5='",md5,"'");
    sprintf(path,"%s%s",DEFAULTPATH,md5);
    ret=IsNuLLInSQL(querySql);
    if(ret==1)
    {
        unlink(path);
    }
    memset(querySql,0,sizeof(querySql));
    sprintf(querySql,"%s%s%s%s%d%s%d","delete from file where md5='",md5,"'"," and uId=",uId," and precode=",precode);
    DeleteSQL(querySql);
}
int REMOVE_Dir_Function(int uId,int precode)
{//int getAllMd5SQL(char *querySql,char (*md5)[33],int precode,int uId);
    //查询该目录下文件的数量
    char getNumSql[120]={0};
    sprintf(getNumSql,"%s%d%s%d%s","select count(md5) from file where precode=",precode," and uId=",uId," and type='-'");
    int m_i=IsNuLLInSQL(getNumSql);
    int i=0;
    char sql[120]={0};

    if(m_i!=0)
    {   
        printf("m_i=%d\n", m_i);
        //获取文件夹下的md5码
        char md5Sql[120]={0};
        sprintf(md5Sql,"%s%d%s%d","select md5 from file where uId=",uId," and precode=",precode);
        char (*fileNum)[33]=(char (*)[33])calloc(m_i,33);
        getAllMd5SQL(md5Sql,fileNum,precode,uId);
        //循环删除文件
        for(i=0;i<m_i;i++)
        {
            REMOVE_By_MD5(fileNum[i],precode,uId);        
        }
        free(fileNum);
    }

    //获取递归文件夹下的目录
    sprintf(sql,"%s%d%s%d%s%s","select count(code) from file where uId=",uId," and precode=",precode," and type=","'d'");
    int ret=IsNuLLInSQL(sql);
    if(ret==0||ret==-1)
    {
        return 0;
    }
    int *dirPrecode=(int *)calloc(ret,sizeof(int));
    getDirNum(uId,precode,dirPrecode);  
    char delSql[150]={0};
    for(i=0;i<ret;i++)
    {
        REMOVE_Dir_Function(uId,dirPrecode[i]);
        memset(delSql,0,sizeof(delSql));
        sprintf(delSql,"%s%d","delete from file where code=",dirPrecode[i]);
        DeleteSQL(delSql);
    }
    //删库
    free(dirPrecode);
}
int PWD_Function(int uId,int code,char (*name)[21],int *pos)
{
    if(code==0)
    {
        return 1;
    }
    char getNameSQL[150]={0};
    sprintf(getNameSQL,"%s%d","select filename from file where code =",code);
    dirNameSQL(getNameSQL,name[*pos]); 
    printf("%s diff:%ld\n",name[*pos],name[(*pos)++]-name[0]);
    char getPrecodeSQL[100]={0};
    sprintf(getPrecodeSQL,"%s%d","select precode from file where code =",code);
    int precode=IsNuLLInSQL(getPrecodeSQL);
    PWD_Function(uId,precode,name,pos);
}

