#ifndef MSGQUEUE
#define MSGQUEUE
//存在的文件
#define MSGQ_KEY_FILE "/tem"
#define MSGQ_KEY_ID 'A'
#define ADS_MSG_TEXT_LEN 256
typedef struct 
{
    long mtype;
    char text[ADS_MSG_TEXT_LEN];
}abs_msg_t;

#define MSG_TYPE_CONFIG 1

int msgq_create(void);
int msgq_send(int msgid,long mtype,const char *text);
int msgq_recv(int msgid,long mtype,  char*out,int out_size,int wait);
int msgq_destroy(int msgid);
#endif