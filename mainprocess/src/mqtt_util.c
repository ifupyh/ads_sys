/* mqtt_util.c —— 把 paho.mqtt.c 包一层皮的工具文件
 * 定位和 sn_util 一样：整体复制用，不逐行讲。对外就四件事：
 *   连（connect）→ 订（subscribe）/ 发（publish）→ 断（disconnect）
 *
 * 编译要链 paho-mqtt3c，源码安装后在 /usr/local/lib/libpaho-mqtt3c.so */
#include <stdio.h>
#include <string.h>
#include "mqtt_util.h"

/* paho 的客户端句柄：整个进程共用一个连接就够了，放静态全局 */
static MQTTClient g_client;

/* 把配置页里填的 mqtt 地址整理成 paho 必须的 tcp://IP:端口 格式：
 *   "192.168.38.1"           -> "tcp://192.168.38.1:1883"
 *   "tcp://192.168.38.1:1883" -> 原样
 *配置页 placeholder 是 "localhost:1883" 这种 IP:端口 写法，
 * 而 paho 的 MQTTClient_create 不认没有 tcp:// 开头的地址，所以要补这一手 */
static void make_uri(const char *raw, char *out, int out_size){
    if (!raw || !raw[0]) { /*如果本机有MQTT服务器，没填就退回本机 broker */
        snprintf(out, out_size, "tcp://127.0.0.1:1883");
        return;
    }
    if (strstr(raw, "://")) { /* 已经写全了，原样用 */
        snprintf(out, out_size, "%s", raw);
        return;
    }
    if (strchr(raw, ':'))  /* IP:端口，只补协议头 */
        snprintf(out, out_size, "tcp://%s", raw);
    else  /* 裸 IP，连默认端口一起补 */
        snprintf(out, out_size, "tcp://%s:1883", raw);
}

/* 收到订阅主题的消息时被调用（paho 在它自己的线程里调，别做耗时的事）
 * topic_name 主题名，message 正文 */
static int on_message_arrived(void *context, char *topic_name, int topic_len,
                              MQTTClient_message *message)
{
    (void) context;
    (void) topic_len;

    /* payload 不保证以 '\0' 结尾，必须按 payloadlen 定长打印
     *（和 curl 写回调里那个坑是同一个道理） */
    printf("[mqtt] 收到消息 主题:%s 内容:%.*s\n",
           topic_name, message->payloadlen, (char *) message->payload);

    /* 这两行必须成对调用，否则每收一条消息就漏一块库分配的内存 */
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topic_name);
    return 1;   /* 返回 1 表示已处理完，paho 不会再重发这条 */
}

int mqtt_util_connect(const char *uri_in, const char *client_id,
                      const char *user, const char *pass)
{
    /* 0 地址标准化：配置页填 IP 或 IP:端口 都能认 */
    char uri[192];
    make_uri(uri_in, uri, sizeof(uri));

    /* 1 创建客户端句柄。PERSISTENCE_NONE = 不做本地落盘持久化 */
    int rc = MQTTClient_create(&g_client, uri, client_id,
                               MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("[mqtt] 创建客户端失败, rc=%d\n", rc);
        return -1;
    }

    /* 2 注册收消息回调（断线、送达两个回调用不上，传 NULL 即可）
     *   必须在 connect 之前调，连上之后再注册会漏掉早期的消息 */
    rc = MQTTClient_setCallbacks(g_client, NULL, NULL,
                                 on_message_arrived, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("[mqtt] 注册回调失败, rc=%d\n", rc);
        MQTTClient_destroy(&g_client);
        return -1;
    }

    /* 3 连接 broker。连接参数必须用 initializer 初始化，否则字段是随机值 */
    MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
    opts.keepAliveInterval = 20;   /* 20 秒没数据就发个 PING，证明链路还活着 */
    opts.cleansession      = 1;    /* 1 = 每次连接都清会话，不保留离线消息 */
    if (user && user[0]) {         /* 匿名 broker 就不用填用户名密码 */
        opts.username = user;
        opts.password = pass;
    }

    rc = MQTTClient_connect(g_client, &opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("[mqtt] 连接 broker 失败, rc=%d(broker 起了吗？地址端口对吗？）\n", rc);
        MQTTClient_destroy(&g_client);
        return -1;
    }

    printf("[mqtt] 已连接 broker: %s(客户端ID: %s)\n", uri, client_id);
    return 0;
}

int mqtt_util_subscribe(const char *topic, int qos)
{
    int rc = MQTTClient_subscribe(g_client, topic, qos);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("[mqtt] 订阅失败, rc=%d\n", rc);
        return -1;
    }
    printf("[mqtt] 已订阅: %s\n", topic);
    return 0;
}

int mqtt_util_publish(const char *topic, const char *payload, int qos)
{
    /* 消息结构体同样必须用 initializer 初始化 */
    MQTTClient_message msg = MQTTClient_message_initializer;
    msg.payload    = (void *) payload;
    msg.payloadlen = (int) strlen(payload);  /* 长度不含结尾的 '\0'，别写成 +1 */
    msg.qos        = qos;
    msg.retained   = 0;  /* 不保留：新订阅者收不到历史消息 */

    MQTTClient_deliveryToken token;
    int rc = MQTTClient_publishMessage(g_client, topic, &msg, &token);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("[mqtt] 发布失败, rc=%d\n", rc);
        return -1;
    }
    /* 只有 QoS > 0 才需要等 broker 确认；QoS = 0 是"发了就算"，不等 */
    if (qos > 0) MQTTClient_waitForCompletion(g_client, token, 3000);
    return 0;
}

void mqtt_util_disconnect(void)
{
    if (!g_client) return;
    MQTTClient_disconnect(g_client, 1000);
    MQTTClient_destroy(&g_client);
    g_client = NULL;
}