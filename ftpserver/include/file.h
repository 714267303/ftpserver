#include <Myhead.h>
#include "../Compute-file-or-string-md5-master/md5.h"
typedef struct 
{
    int precode;
    int code;
    char filename[20];
    char md5[MD5_STR_LEN];
    int uId;
    long size;
    char type;
}My_file;
