#include <mysql/mysql.h>
#include <Myhead.h>
int queryMaxCode(int uId)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char* user="root";
    char* password="123";
    char* database="project1";//要访问的数据库名称
    char queryadd[50]="select max(code) from file where uId=";
    char query[100]={0};
    sprintf(query,"%s%d",queryadd,uId);
    //sprintf(query,"%s%s%s",query, argv[1],"'");
    puts(query);
    int t,r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
        return -1;
    }else{
        printf("Connected...\n");
    }
    t=mysql_query(conn,query);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        //	printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)
        {
            if((row=mysql_fetch_row(res))!=NULL)
            {	
                //printf("num=%d\n",mysql_num_fields(res));//列数
                if(row[0]==NULL)
                    r=0;
                else
                    r=atoi(row[0]);
            }
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    mysql_close(conn);

    return r;
}
int IsNuLLInSQL(char *querySql)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char* user="root";
    char* password="123";
    char* database="project1";//要访问的数据库名称
    //sprintf(query,"%s%s%s",query, argv[1],"'");
    puts(querySql);
    int t,r=1;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
        return -1;
    }else{
        printf("Connected...\n");
    }
    t=mysql_query(conn,querySql);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        //	printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)
        {
            if((row=mysql_fetch_row(res))!=NULL)
            {	
                printf("ok1\n");
               if(mysql_num_fields(res)==1)
               {
                   if(row[0]==NULL)
                       r=0;
                   else
                       r= atoi(row[0]);
               }
                printf("ok2\n");
                mysql_free_result(res);
                mysql_close(conn);
                return r;
            }

        }else{
            printf("Don't find data\n");
            return -1;
        }
        mysql_free_result(res);
    }
    mysql_close(conn);

    return -1;
}
int getFileType(char *querySql)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char* user="root";
    char* password="123";
    char* database="project1";//要访问的数据库名称
    //sprintf(query,"%s%s%s",query, argv[1],"'");
    puts(querySql);
    int t,r=1;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
        return -1;
    }else{
        printf("Connected...\n");
    }
    t=mysql_query(conn,querySql);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        //	printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)
        {
            if((row=mysql_fetch_row(res))!=NULL)
            {	
                printf("ok1\n");
               if(mysql_num_fields(res)==1)
               {
                   if(row[0]==NULL)
                       r=0;
                   else
                   {
                       char c[5]={0};
                       memcpy(&r,row[0],1);
                       printf("row[0] %s\n",row[0]);
                       printf("c[5] %s\n",c);
                       printf("nice\n");
                   }
               }
                printf("ok2\n");
                mysql_free_result(res);
                mysql_close(conn);
                return r;
            }

        }else{
            printf("Don't find data\n");
            return -1;
        }
        mysql_free_result(res);
    }
    mysql_close(conn);

    return -1;
}
int DeleteSQL(char *querySql)
{
    MYSQL *conn;
    char* server="localhost";
    char* user="root";
    char* password="123";
    char* database="project1";//要访问的数据库名称
    //sprintf(query,"%s%s%s",query, argv[1],"'");
    puts(querySql);
    int t,r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
        return -1;
    }else{
        printf("Connected...\n");
    }
    t=mysql_query(conn,querySql);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        printf("delete success,delete row=%ld\n",(long)mysql_affected_rows(conn));
    }
    mysql_close(conn);

    return -1;
}
int getMd5SQL(char *querySql,char *md5)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char* user="root";
    char* password="123";
    char* database="project1";//要访问的数据库名称
    //sprintf(query,"%s%s%s",query, argv[1],"'");
    puts(querySql);
    int t,r=1;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
        return -1;
    }else{
        printf("Connected...\n");
    }
    t=mysql_query(conn,querySql);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        //	printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)
        {
            if((row=mysql_fetch_row(res))!=NULL)
            {	
                printf("ok1\n");
               if(mysql_num_fields(res)==1)
               {
                   if(row[0]==NULL)
                   {
                       r=0;
                   }
                   else
                   {
                        strncpy(md5,row[0],32);
                   }
                   r= 0;
               }
                printf("ok2\n");
                mysql_free_result(res);
                mysql_close(conn);
                return r;
            }

        }else{
            printf("Don't find data\n");
            r=-1;
        }
        mysql_free_result(res);
    }
    mysql_close(conn);

    return -1;
}
int getAllMd5SQL(char *querySql,char (*md5)[33],int precode,int uId)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char* user="root";
    char* password="123";
    char* database="project1";//要访问的数据库名称
    //sprintf(query,"%s%s%s",query, argv[1],"'");
    puts(querySql);
    int t,r=1,c=0;

    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
        return -1;
    }else{
        printf("Connected...\n");
    }
    t=mysql_query(conn,querySql);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }
    else
    {
        //	printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)
        {
            while((row=mysql_fetch_row(res))!=NULL)
            {	
               if(mysql_num_fields(res)==1)
               {
                   if(row[0]!=0)
                   strncpy(md5[c++],row[0],32);
                   r= 0;
               }
            }
             mysql_free_result(res);
            mysql_close(conn);
            return r;
        }else{
            printf("Don't find data\n");
            r=-1;
        }
        mysql_free_result(res);
    }
    mysql_close(conn);
    return -1;
}
int getDirNum(int uId,int precode,int *dirArry)
{
    char getDir[120]={0};
    sprintf(getDir,"%s%d%s%d%s%s","select code from file where uId=",uId," and precode=",precode," and type=","'d'");
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char* user="root";
    char* password="123";
    char* database="project1";//要访问的数据库名称
    puts(getDir);
    int t,r=1,c=0;

    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
        return -1;
    }else{
        printf("Connected...\n");
    }
    t=mysql_query(conn,getDir);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }
    else
    {
        //	printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)
        {
            while((row=mysql_fetch_row(res))!=NULL)
            {	
               if(mysql_num_fields(res)==1)
               {
                   dirArry[c++]=atoi(row[0]);
                   r= 0;
               }
            }
            mysql_free_result(res);
            mysql_close(conn);
            return r;
        }else{
            printf("Don't find data\n");
            r=-1;
        }
        mysql_free_result(res);
    }
    mysql_close(conn);
    return -1;
}
int dirNameSQL(char *querySql,char *name)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char* user="root";
    char* password="123";
    char* database="project1";//要访问的数据库名称
    //sprintf(query,"%s%s%s",query, argv[1],"'");
    puts(querySql);
    int t,r=1;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
        return -1;
    }else{
        printf("Connected...\n");
    }
    t=mysql_query(conn,querySql);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        //	printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)
        {
            if((row=mysql_fetch_row(res))!=NULL)
            {	
                printf("ok1\n");
               if(mysql_num_fields(res)==1)
               {
                   if(row[0]==NULL)
                   {
                       r=0;
                   }
                   else
                   {
                        strncpy(name,row[0],20);
                   }
                   r= 0;
               }
                printf("ok2\n");
                mysql_free_result(res);
                mysql_close(conn);
                return r;
            }

        }else{
            printf("Don't find data\n");
            r=-1;
        }
        mysql_free_result(res);
    }
    mysql_close(conn);

    return -1;
}
