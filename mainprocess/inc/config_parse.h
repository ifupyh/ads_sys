#ifndef CONFIG_PARSE_H
#define CONFIG_PARSE_H

/* 配置项：6 个字段和 configprocess 存进 ini 的内容一一对应
 *（数组长度和 configprocess 的 device_config_t 保持一致，比它小会被截断） */
typedef struct {
    char backendUrl[128]; /* [server] backendUrl    后端地址，只填 IP:端口 */
    char mqttUrl[128]; /* [server] mqttUrl       MQTT 服务器地址，主进程连 broker 用 */
    char wifiName[128]; /* [wifi]   wifiName      备用 */
    char wifiPassword[128]; /* [wifi]   wifiPassword  备用 */
    int  area; /* [device] area          ini 里存的是字符串，读进来转 int */
    char detailAddress[128]; /* [device] detailAddress 片区 + 详细地址 */
} config_t;
/* 读 ini 填进 cfg：
 *   ini_path = 配置进程消息里带来的路径；传 NULL 则用 $HOME/.config/ads_config.ini
 * 返回 0 成功，-1 失败（文件打不开） */
int config_parse_parse(config_t *cfg, const char *ini_path);
#endif /* CONFIG_PARSE_H */