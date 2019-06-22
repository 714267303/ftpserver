#include <Myhead.h>
#include <ctype.h>
#include "../include/pthread_DataStruct.h"
#include "../include/command.h"
#include "../include/user.h"
#include "../include/database.h"
#include "../include/SendFile.h"
#include "md5.h"
int Compute_string_md5(unsigned char *dest_str, unsigned int dest_len, char *md5_str);
int Compute_file_md5(const char *file_path, char *md5_str);
int main(int arg,char *argv[])
{   
    char solt[10]={0};
    char token[20]={0};
    char passwd[20]={0};
    My_command cmd;
    My_user user;
beging:
    memset(&user,0,sizeof(user));
    memset(&cmd,0,sizeof(cmd));
    printf("请输入用户ID:");
    scanf("%d",&user.uId);
    printf("\n请输入用户密码:");
    scanf("%s",passwd);
    int serverFd;
    int sendtotal=0,ret;
    {
        serverFd=socket(AF_INET,SOCK_STREAM,0);
        InitSocket(serverFd,argv[1],argv[2]);

        cmd.type=First;
        //发送命令1
        send(serverFd,&cmd,sizeof(cmd),0); 
        sendtotal+=sizeof(cmd);
        //发送用户名2
        user.cur_Path=0;
        send(serverFd,&user,sizeof(user),0);
        sendtotal+=sizeof(user);
        //接收盐值:
        recv(serverFd,solt,10,0);
        //根据盐值产生加密密文
        printf("ok solt：%s\n",solt);
        strncpy(user.ciphertext,crypt(passwd,solt),50); 
        //发送密密文3
        printf("crypt:%s\n",user.ciphertext);
        send(serverFd,&user,sizeof(user),0);
        printf("when isend  cur_Path%d\n",user.cur_Path);
        sendtotal+=sizeof(user);
        //接收验证结果
        recv(serverFd,&ret,4,0);
        //发送token值4
        if(ret==1)
        {
            strncpy(token,"hello",20);
            send(serverFd,token,20,0);
            sendtotal+=sizeof(token);
            printf("welcome to your private space!\n");
        }
        else
        {
            printf("sorry,password erro!\n");
            goto beging;
        }
    }
    printf("sendtotal=%d\n",sendtotal);
    //wait(NULL);
    char user_cmd[30];
    getchar();//吃掉回车
    while(1)
    {
commandBeging:
        //输入命令
        fgets(user_cmd,30,stdin);
        My_command input_cmd;
        memset(&input_cmd,0,sizeof(My_command));
        sscanf(user_cmd,"%s %s",input_cmd.name,input_cmd.parameter);
        input_cmd.type=NORMAL;
        input_cmd.user=user;
        printf("your cmd:%s cmd lenth:%ld cmd parameter:%s\n",input_cmd.name,strlen(input_cmd.name),input_cmd.parameter);
        char md5[MD5_STR_LEN+1]={0};
        if(strncmp(input_cmd.name,"puts",4)==0)
        {
            //printf("type=puts\n");
            input_cmd.type=PUTS;
        }
        else if(strncmp(input_cmd.name,"gets",4)==0)
        {
            input_cmd.type=GETS;
        }
        if(input_cmd.type!=NORMAL)
        {
            if(0==fork())
            {
                int serverFd2=socket(AF_INET,SOCK_STREAM,0);
                printf("connecting...\n");
                InitSocket(serverFd2,argv[1],argv[2]);
                cmd.type=Second;
                cmd.user=user;
                //发送命令
                send(serverFd2,&cmd,sizeof(cmd),0);
                sendtotal+=sizeof(cmd);
                //发送用户名
                send(serverFd2,&user,sizeof(user),0);
                sendtotal+=sizeof(user);
                //发送token值
                strncpy(token,"hello",20);
                send(serverFd2,token,20,0);
                //printf("client.c token:%s\n",token);
                sendtotal+=sizeof(token);
                //while(1);

                int buf;
                if(input_cmd.type==PUTS)
                {
                    Compute_file_md5(input_cmd.parameter,md5);                   
                    //先判断文件是否存在
                    if(access(input_cmd.parameter,F_OK|R_OK)==-1)
                    {
                        printf("cheak your file\n");
                        goto commandBeging;
                    }
                }
                recv(serverFd2,&buf,4,0);
                if(input_cmd.type==PUTS)
                {
                    //sleep(1);//**********************************************************************
                    //发送命令
                    send(serverFd2,&input_cmd,sizeof(My_command),0);//该命令导致服务器疯狂while(1);
                    //计算,发送md5码
                    //printf("ret=%d i can send\n",buf);
                    send(serverFd2,md5,MD5_STR_LEN,0);
                    // printf("md5 %s\n",md5);
                    //接收判断值
                    recv(serverFd2,&ret,4,0);;//客户端阻塞在这里
                    if(ret==-1)//服务器不存在该文件，发送之
                    {
                        printf("send normally\n");
                        tranBigTrain(serverFd2,input_cmd.parameter,NULL); 
                    }
                    else
                    {
                        printf("ret=%d seckill\n",ret);
                    }

                }
                else if(input_cmd.type ==GETS)
                {
                    send(serverFd2,&input_cmd,sizeof(My_command),0);//该命令导致服务器疯狂while(1);
                    //接收查询结果
                    recv(serverFd2,&ret,4,0);
                    if(ret==-1)
                    {
                        printf("file not exist\n");
                        goto end;                    
                    }
                    //接收文件
                    downloadFile(serverFd2,NULL);
                }
                else
                {
                    printf("client.c command erro\n");     
                }
                //四次挥手！
end:
                close(serverFd2);
                //exit(1);
                return 1;
            }
        }
        else if(input_cmd.type==NORMAL)
        {
            //发送命令
            if(strcmp(input_cmd.name,"cd")==0)
            {
                char *path=input_cmd.parameter;
                int pos=0;
                while(path[pos])
                {
                    if(path[pos]=='.')
                    {
                        pos+=3;
                    }
                    else if(path[pos]=='/'&&pos==0)
                    {
                        pos+=1;
                    }
                    else if(isalpha(path[pos]))
                    {
                        pos++;
                    }
                    else
                    {
                        printf("cheak your path\n"); 
                        goto commandBeging;
                    }
                }

            }
            else if(strcmp(input_cmd.name,"mkdir")==0)
            {
                char *path=input_cmd.parameter;
                int pos=0;
                while(path[pos])
                {
                    if(isalpha(path[pos]))
                    {
                        pos++;
                    }
                    else
                    {
                        printf("cheak your dirName\n");
                        goto commandBeging;
                    }
                }
            }
            send(serverFd,&input_cmd,sizeof(My_command),0);
            //循环接收数据,当小火车为空时结束
            if(strcmp(input_cmd.name,"ls")==0)
                while(recv_Ls(serverFd,STDOUT_FILENO)!=0);
            else if(strcmp(input_cmd.name,"remove")==0)
            {
                //接收remove结果
                recv(serverFd,&ret,4,0);
                if(-1==ret)
                {
                    printf("remove erro\n");
                }
                else if(1==ret)
                {
                    printf("remove success\n");
                }
            }
            else if(strcmp(input_cmd.name,"cd")==0)
            {
                //接收返回值
                recv(serverFd,&ret,4,0);
                if(ret==-1)
                {
                    printf("erro cd path\n");
                }
                else
                {
                    //打印路径
                }
            }
            else if(strcmp(input_cmd.name,"mkdir")==0)
            {
                //接收是否为同名文件夹
                recv(serverFd,&ret,4,0);
                if(ret!=-1)
                {
                    printf("overwrite the dir\n");
                    //发送是否覆盖文件夹
                }
            }
            else if(strcmp(input_cmd.name,"pwd")==0)
            {
                char path[410]={0};
                recv(serverFd,path,410,0);
                printf("%s\n",path);
            }
        }
        else
        {
            printf("client.c command type erro\n");
        }
        waitpid(-1,0,WNOHANG);
    }
    printf("erro main thread game over\n");
    return 0;
}

