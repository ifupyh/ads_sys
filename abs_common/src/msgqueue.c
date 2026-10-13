#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "msgqueue.h"

int msgq_create(void)
{
    key_t key=ftok(MSGQ_KEY_FILE,MSGQ_KEY_ID);
    if (key==-1)
    {
        perror("ftok");
    }
    int msgid=msgget(key,IPC_CREAT|0666);
    if (msgid==-1)
    {
        perror("msgid");
        return -1;
    }
    printf("msgq_create msgid=%d\n,key=0x%x",msgid,(unsigned)key);
    return msgid;
}
int msgq_send(int msgid,long mtype,const char *text)
{
    abs_msg_t msg;
    memset(&msg,0,sizeof(msg));
    msg.mtype=mtype;
    snprintf(msg.text,sizeof(msg.text),"%s",text);
    if(msgsnd(msgid,&msg,strlen(msg.text)+1,0) == -1){
        perror("msgsnd 失败");
        return -1;
    }
    printf("[success]msgq_send mtype=%ld,text=%s\n",mtype,msg.text);
    return 0;
}
int msgq_recv(int msgid,long mtype, char*out,int out_size,int wait)
{
    abs_msg_t msg;
    memset(&msg,0,sizeof(msg));//清零
    if(msgrcv(msgid,&msg,out_size,mtype,wait?0:IPC_NOWAIT) == -1){
        if(errno == ENOMSG){
            printf("msgq_recv no message\n");
            return 1;
        }
        perror("msgrcv  失败");
        return -1;
    }
    //收到消息正文长度
    snprintf(out,out_size,"%s",msg.text);
    return 0; 
}
int msgq_destroy(int msgid){
    //删除消息队列，判断删除成功还是失败
    if(msgctl(msgid,IPC_RMID,NULL) == -1){
        perror("msgctl  失败");
        return -1;
    }    
    printf("msgq_destroy msgid=%d 成功\n",msgid);
    return 0;
}
