/* wifi_utils.h —— WiFi 连接工具（wpa_supplicant + dhclient 方案） */
#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

/* connect_wifi 的返回值：0 成功，负数是错误码 */
#define WIFI_OK               0
#define WIFI_ERR_PARAM       -1   /* SSID 为空 */
#define WIFI_ERR_NO_IFACE    -2   /* 找不到无线网卡（虚拟机 / 台式机就是这一条） */
#define WIFI_ERR_CONFIG      -3   /* 生成 wpa_supplicant.conf 失败 */
#define WIFI_ERR_SUPPLICANT  -4   /* wpa_supplicant 启动失败 */
#define WIFI_ERR_DHCP        -5   /* 没拿到 IP */
#define WIFI_ERR_TIMEOUT     -6   /* 轮询超时，没能连上 */

/* 把错误码翻译成人话，便于直接 printf("%s", wifi_strerror(ret)) */
const char *wifi_strerror(int code);

/* 先探一下有没有无线网卡，方便业务决定要不要真的去连 */
int wifi_iface_exists(void);

/*
 * 用 wpa_supplicant + dhclient 连接 WiFi，成功返回 WIFI_OK。
 *
 * 注意：这是**同步阻塞**调用，最长会等 5 秒左右。
 *       mongoose 是单线程事件循环，正式产品要把这段丢到独立线程里做，
 *       别卡在 HTTP 回调里（课堂为了主线清晰才同步调用）。
 */
int connect_wifi(const char *ssid, const char *psk);

#endif /* WIFI_UTILS_H */
