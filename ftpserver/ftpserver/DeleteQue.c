#include <Myhead.h>
#include "../include/pthread_DataStruct.h"

int DeleteQue(pQue factory,dataType *client)
{
    if(factory->num==0)
    {
        return -1;
    }
    *client =factory->head->data;
    factory->num --;
    if(factory->num==1)
    {
        factory->head=factory->tail=NULL;
    }
    else
    {
        pNode cur= factory->head;
        factory->head=cur->next;
        free(cur);
    }
    return 1;
}
