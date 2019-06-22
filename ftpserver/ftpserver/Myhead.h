#include<sys/time.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <pthread.h>
#include<signal.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include<stdlib.h>
#include<time.h>
#include <grp.h>
#include<pwd.h>
#include<sys/select.h>
#include<sys/mman.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <crypt.h>
#define FUNC_CHEAK(ret,retValue,FuncName){if(ret==retValue){perror(FuncName);return -1;}}
#define FUNC_CHEAK_NORETURN(ret,retValue,FuncName){if(ret==retValue){perror(FuncName);}}
#define TH_FUNC_CHEAK(ret,funcname){if(ret!=0){printf("%s is erro",funcname);return -1;}}
