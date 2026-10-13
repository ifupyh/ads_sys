#ifndef MQTT_UTIL_H
#define MQTT_UTIL_H

/* paho.mqtt.c 的核心头文件，源码安装后在 /usr/local/include/MQTTClient.h */
#include <MQTTClient.h>

/* 连上 broker：uri 填 "IP:端口"（如 192.168.31.94:1883）或写全
 * "tcp://IP:端口" 都行，工具内部会统一整理成 paho 要的格式
 * client_id 在同一个 broker 上必须唯一，重复会把前面那个踢下线
 * user / pass 传 NULL 表示匿名登录；成功返回 0，失败返回 -1 */
int mqtt_util_connect(const char *uri, const char *client_id,
                      const char *user, const char *pass);

/* 订阅主题：topic 支持通配符，+ 匹配一层、# 匹配多层（如 ads/cmd/#）
 * qos：0 最多一次 / 1 至少一次 / 2 恰好一次
 * 收到的消息由本工具内部的回调直接打印，调用方不用管 */
int mqtt_util_subscribe(const char *topic, int qos);

/* 发布一条消息：payload 是正文（长度自动算，不用自己数）
 * 成功返回 0，失败返回 -1 */
int mqtt_util_publish(const char *topic, const char *payload, int qos);

/* 断开并销毁句柄 */
void mqtt_util_disconnect(void);

#endif /* MQTT_UTIL_H */