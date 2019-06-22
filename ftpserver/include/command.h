#define First 4//connect
#define Second 5
#define PUTS 1
#define GETS 2
#define NORMAL 3
#define MAXDEEP 20
#define DEFAULTPATH "../../../../Downloads/privateSpace/"
#define DEFAULTCLIENT "../../../../Downloads/ClientSpace/"
#define DEFAULTRECORDPATH "../../../../Downloads/Record/"
#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)
#include <Myhead.h>
void * commandFunc(void *);
void *Register(void *arg);
void regulate_alar(int uId);
void add_to_alar(int uId);
 void InitTime(int tv_sec,int tv_usec);
 int delete_from_alar(int uId);
