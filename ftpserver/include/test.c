#include <Myhead.h>

void get_salt(char *salt,char *passwd)
{     
    int i,j; 
    //取出 salt,i 记录密码字符下标,j 记录$出现次数  
    for(i=0,j=0;passwd[i] && j != 3;++i) 
    {
        if(passwd[i] == '$')   
            ++j;     
    } 
    strncpy(salt,passwd,i-1); 
}
int main(int argc,char **argv)
{    
    struct spwd *sp; 
    char *passwd;     
    char salt[512]={0}; 
    if(argc != 2)       
        help(); 
    //输入用户名密码    
    passwd=getpass("请输入密码:");     
    // 得到用户名以及密码,命令行参数的第一个参数为用户名   
    if((sp=getspnam(argv[1])) == NULL)   I
        error_quit("获取用户名和密码");   
    //得到 salt,用得到的密码作参数     
    get_salt(salt,sp->sp_pwdp); 
    //进行密码验证    
    if(strcmp(sp->sp_pwdp,crypt(passwd,salt)) == 0)  
        printf("验证通过!\n");  
    else     
        printf("验证失败!\n"); 
    return 0;
}
