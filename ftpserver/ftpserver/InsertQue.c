#include <Myhead.h>
#include "../include/pthread_DataStruct.h"

int InsertQue(pQue factory,dataType client)
{
    pNode cur=(pNode)calloc(1,sizeof(Node));
    cur->data=client;
    factory->num++;
    if(factory->head==NULL)
    {
        factory->head=factory->tail=cur;
    }
    else 
    {
        factory->tail->next=cur;
        factory->tail=cur;
    }
    return 1;
};
