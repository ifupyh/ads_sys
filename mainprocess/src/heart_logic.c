//心跳处理，设备每隔一段时间都要跟后端说一句“我还活着”，顺便把自己的状态带上，MQTT是长连接，后端顺着这条连接把指令(换广告、重载配置、重启...)退下来
#include <stdio.h>
#include "heart_logic.h"
#include "mqtt_util.h"
#include "sn_util.h"
#include "cjson/cJSON.h"

/*
主题约定，和后端说好就行
"ads/heart"  设备->后端(MQTT),我还在线，这是我的状态
"ads/cmd/#"  后端(MQTT)->设备  请执行命令指令
*/
#define TOPIC_HEART "ads/heart"
#define TOPIC_CMD "ads/cmd/#"

//发一次心跳，把状态拼成json，发送到MQTT的在线状态主题，返回0成功，-1失败(通常是没有脸上broker)
int send_heart_request(void){
    //用cJSON凭借消息体
    cJSON *root = cJSON_CreateObject();
    if(root == NULL)
        return -1;
    cJSON_AddStringToObject(root, "sn", get_device_sn());
    cJSON_AddNumberToObject(root, "status", 1);//1在线，0离线
    //转换成json字符串，压成一行(节省流量)
    char *json = cJSON_PrintUnformatted(root);
    if(json == NULL){
        cJSON_Delete(root);
        return -1;
    }
    printf("发送心跳:\n");
    int ret = mqtt_util_publish(TOPIC_HEART, json, 0);//Qos0:心跳丢一条无所谓
    cJSON_free(json);
    cJSON_Delete(root);
    return ret;
}

//连上broker之后，订阅后端下发的指令主题
int heart_subscribe_cmd(void){
    return mqtt_util_subscribe(TOPIC_CMD, 1);//Qos1:指令不能丢
}