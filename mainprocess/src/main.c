#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "curl_util.h"
#include "msgqueue.h"
#include "config_parse.h"
#include "config_logic.h"
#include "heart_logic.h"
#include "mqtt_util.h"
#include "sn_util.h"
#define MQTT_USER "ifupyh"
#define MQTT_PASSWORD "123456"

static void connect_mqtt(void)
{
    config_t cfg;
    if (config_parse_parse(&cfg,NULL))
    {
        printf("[main] 进程解析ini文件失败");
        return;
    }
    if (mqtt_util_connect(cfg.mqttUrl,get_device_sn(),MQTT_USER,MQTT_PASSWORD)!=0)
    {
        printf("[main] 连接 MQTT broker 失败\n");
        return;
    }
    heart_subscribe_cmd();
}

#define HEART_BEAT_INTERVAL 30
static volatile sig_atomic_t g_run= 1;
static void on_sigint(int sig)
{ 
    (void)sig;
    g_run = 0;
}

static void *handle_register_request(void *param)
{ 
    (void)param;
    printf("【主进程】子线程 启动...\n");
    int msgqid = msgq_create();
    if(msgqid < 0)
    {
        printf("【主进程】创建消息队列失败\n");
        return NULL;
    }
    while(g_run)
    {   
        char text[ADS_MSG_TEXT_LEN];
        int ret = msgq_recv(msgqid,MSG_TYPE_CONFIG,text,sizeof(text),1);
        if(ret != 0)
        {
            printf("【主进程】接收消息失败\n");
            if(g_run)sleep(1);
            continue;
        }
        printf("【主进程】接收到消息：%s\n",text);
        send_config_request(text);
    }
    printf("【主进程】注册线程退出...\n");
    return NULL;
}

int main(int argc, char const *argv[])
{

    (void)argc;
    (void)argv;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    connect_mqtt();
    signal(SIGINT, on_sigint);
    pthread_t tid;
    if(pthread_create(&tid,NULL,handle_register_request,NULL)){
        perror("[main 进程]pthread_create failed !");
        return 1;
    }
    pthread_detach(tid);
    while (g_run)
    {
        send_heart_request();
        sleep(HEART_BEAT_INTERVAL);
    }
    printf("[main 进程]结束运行...,队列保留在内核，可用 ipcs -q 查看\n");
    return 0;
}
