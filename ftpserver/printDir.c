#include <Myhead.h>

int main(int arg,char *argv[])
{
	DIR *dir=opendir(argv[1]);
	struct dirent *p;
	struct stat status;
	while((p=readdir(dir))!=NULL)
	{
		stat(p->d_name,&status);
	    
        char mode[10]={0};
        if(p->d_type==4)
        {
            mode[0]='d';
        }
        else
        {
            mode[0]='-';
        }
        int i=0;
        char k;
        while(i<3)
        {
            int j=0;
            int qualify=1;
            switch(i)
            {
            case 0:k='x';break;
              case 1:k='w';break;
              case 2:k='r';break;
            }
            for(j=i;j<=i+6;j+=3)
            {
                int m= qualify<<j;
                mode[9-j]=(m&status.st_mode)==m?(k):'-';
            }
            i++;
        }
        i=0;
        /* printf("i_mode:%x,ino=%ld,reclen=%d,type=%d,name=%s\n",status.st_mode,p->d_ino,p->d_reclen,p->d_type,p->d_name); */
        while(i<10)
        {
            printf("%c",mode[i++]);
        };
        
         struct passwd *pw=getpwuid(status.st_uid);
         struct group *gr=getgrgid(status.st_gid);
        printf(" %ld %s %s %-7ld",status.st_nlink,pw->pw_name,gr->gr_name,status.st_size);
        printf(" %s\n",p->d_name);
    }
	return 0;
}
