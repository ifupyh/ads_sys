/* wifi_utils.c —— WiFi 连接工具实现
 *
 * Linux 内核没有"连上某个 WiFi"这种系统调用：
 *   内核只管 802.11 的数据通道，认证/密钥协商这些协议层的事在用户态做。
 * 所以通用做法就是 C 程序去调系统命令：
 *   wpa_supplicant 做 关联 + 4 次握手，dhclient/udhcpc 负责拿 IP。
 */
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <sys/stat.h>
 
 /* WIFEXITED / WEXITSTATUS 定义在 sys/wait.h 里 —— Linux 上漏了这个头文件会编不过。
  * （MinGW 没装这个头，所以只在非 Windows 平台包含；Windows 分支给个等价定义，
  *   目的是让这份代码在 Windows 的 Cursor 里也能"看得过眼"，真跑必须上 Linux） */
 #if defined(_WIN32)
   #ifndef WIFEXITED
     #define WIFEXITED(s)   ((s) == 0)
     #define WEXITSTATUS(s) (s)
   #endif
 #else
   #include <sys/wait.h>
 #endif
 
 #include "wifi_utils.h"
 
 #define WIFI_IFACE "wlan0"                        /* 网卡名，用 ip addr 查看 */
 #define WPA_CONF   "/tmp/wpa_supplicant.conf"     /* 临时配置 */
 #define WPA_LOG    "/tmp/wpa_supplicant.log"       /* supplicant 日志 */
 
 /* MinGW 下 chmod 签名不同；非 Linux 直接不做（也连不了 WiFi） */
 #if defined(_WIN32)
   #define WIFI_CHMOD(p, m) ((void) (p), (void) (m))
 #else
   #define WIFI_CHMOD(p, m) chmod((p), (m))
 #endif
 
 /* 跑一条 shell 命令；需要读输出就传 output。返回命令退出码，异常退出返回 -1 */
 static int exec_cmd(const char *cmd, char *output, int out_size)
 {
     FILE *fp = popen(cmd, "r");     /* popen 会起一个 /bin/sh 来跑这条命令 */
     int status;
 
     if (fp == NULL) return -1;
 
     if (output != NULL && out_size > 0) {
         size_t n;
         memset(output, 0, (size_t) out_size);
         n = fread(output, 1, (size_t) out_size - 1, fp);   /* 留 1 字节给结尾 '\0' */
         (void) n;                                          /* 命令没输出也正常 */
     }
 
     status = pclose(fp);            /* 等命令结束并取回"等待状态" */
     return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
 }
 
 int wifi_iface_exists(void)
 {
 #if defined(_WIN32)
     return 0;                       /* Windows 上没有这套，直接当没有 */
 #else
     char cmd[128];
     char out[256];
 
     /* sysfs 最快，Linux 上都有 */
     if (access("/sys/class/net/" WIFI_IFACE, F_OK) == 0) return 1;
 
     /* 退一步用命令探测（有些精简系统 /sys 没挂） */
     snprintf(cmd, sizeof(cmd), "ip link show %s 2>/dev/null", WIFI_IFACE);
     return exec_cmd(cmd, out, sizeof(out)) == 0 ? 1 : 0;
 #endif
 }
 
 const char *wifi_strerror(int code)
 {
     switch (code) {
         case WIFI_OK:              return "ok";
         case WIFI_ERR_PARAM:       return "SSID 为空";
         case WIFI_ERR_NO_IFACE:    return "没找到无线网卡（虚拟机/台式机属正常现象）";
         case WIFI_ERR_CONFIG:      return "生成 wpa_supplicant.conf 失败";
         case WIFI_ERR_SUPPLICANT:  return "wpa_supplicant 启动失败";
         case WIFI_ERR_DHCP:        return "DHCP 没拿到 IP";
         case WIFI_ERR_TIMEOUT:     return "轮询超时，没能连上";
         default:                   return "未知错误";
     }
 }
 
 /* 生成 wpa_supplicant 配置 */
 static int write_wpa_conf(const char *ssid, const char *psk)
 {
     FILE *fp = fopen(WPA_CONF, "w");
     if (fp == NULL) return -1;
 
     fprintf(fp, "ctrl_interface=/var/run/wpa_supplicant\n");
     fprintf(fp, "update_config=1\n");
     fprintf(fp, "country=CN\n");          /* 国家码决定可用信道与功率，写错可能连不上 */
     fprintf(fp, "network={\n");
     fprintf(fp, "    ssid=\"%s\"\n", ssid);
     fprintf(fp, "    psk=\"%s\"\n", psk);
     /* key_mgmt 的合法取值是 WPA-PSK（覆盖 WPA/WPA2）和 SAE（WPA3）；
      * 写成 WPA2-PSK / WPA3-PSK 是无效的，会报 Unsupported key_mgmt。
      * 两个都写 = Transition 模式，WPA2/WPA3 热点都能连。 */
     fprintf(fp, "    key_mgmt=WPA-PSK SAE\n");
     fprintf(fp, "    scan_ssid=1\n");     /* 隐藏 SSID 的热点必须加 */
     fprintf(fp, "}\n");
     fclose(fp);
 
     WIFI_CHMOD(WPA_CONF, 0600);           /* 里面有明文密码，收紧权限 */
     return 0;
 }
 
 /* 检查是否连上：iw/iwconfig 里能看到 SSID，且网卡拿到了 IP */
 static int check_wifi_status(const char *ssid)
 {
     char out[512];
 
     /* 新系统推荐 iw；很多开发板只装了 wireless-tools，所以退回 iwconfig */
     exec_cmd("iw dev " WIFI_IFACE " link 2>/dev/null", out, sizeof(out));
     if (strstr(out, ssid) == NULL) {
         exec_cmd("iwconfig " WIFI_IFACE " 2>/dev/null", out, sizeof(out));
         if (strstr(out, ssid) == NULL) return -1;
     }
 
     /* 链路通了还不算完，得看有没有 IP */
     exec_cmd("ip addr show " WIFI_IFACE " 2>/dev/null", out, sizeof(out));
     return strstr(out, "inet ") != NULL ? 0 : -1;
 }
 
 /* 清理残留进程与临时文件 */
 static void clean_wifi(void)
 {
     /* killall 来自 psmisc 包，精简系统上可能没有；失败无所谓 */
     exec_cmd("killall -9 wpa_supplicant 2>/dev/null", NULL, 0);
     exec_cmd("killall -9 dhclient 2>/dev/null", NULL, 0);
     remove(WPA_CONF);
     remove(WPA_LOG);
 }
 
 int connect_wifi(const char *ssid, const char *psk)
 {
     char cmd[512];
     char out[512];
 
     if (ssid == NULL || *ssid == '\0') return WIFI_ERR_PARAM;
     if (psk == NULL) psk = "";
 
     printf("[wifi] 开始连接 SSID=\"%s\"\n", ssid);
 
     /* ---- 1 先看有没有无线网卡 ----
      * 虚拟机 / 台式机没有 wlan0，这一步直接判掉，不用白等 5 秒 */
     if (!wifi_iface_exists()) {
         printf("[wifi] 没有无线网卡 %s，跳过真实连接\n", WIFI_IFACE);
         return WIFI_ERR_NO_IFACE;
     }
 
     /* ---- 2 清掉上一次的残留 ---- */
     clean_wifi();
 
     /* ---- 3 生成配置 ---- */
     if (write_wpa_conf(ssid, psk) != 0) return WIFI_ERR_CONFIG;
 
     /* ---- 4 后台启动 wpa_supplicant（-B 后台、-f 打日志）---- */
     snprintf(cmd, sizeof(cmd), "wpa_supplicant -i %s -c %s -B -f %s",
              WIFI_IFACE, WPA_CONF, WPA_LOG);
     if (exec_cmd(cmd, out, sizeof(out)) != 0) {
         fprintf(stderr, "[wifi] wpa_supplicant 启动失败: %s\n", out);
         clean_wifi();
         return WIFI_ERR_SUPPLICANT;
     }
 
     sleep(2);   /* 等 4 次握手完成（这里就是"阻塞回调"的元凶，正式版要丢线程） */
 
     /* ---- 5 DHCP 拿 IP：先 dhclient，没有就退 udhcpc（busybox 精简系统）---- */
     snprintf(cmd, sizeof(cmd), "dhclient -r %s 2>/dev/null; dhclient %s -v 2>/dev/null",
              WIFI_IFACE, WIFI_IFACE);
     if (exec_cmd(cmd, out, sizeof(out)) != 0) {
         snprintf(cmd, sizeof(cmd), "udhcpc -i %s -q 2>/dev/null", WIFI_IFACE);
         if (exec_cmd(cmd, out, sizeof(out)) != 0) {
             clean_wifi();
             return WIFI_ERR_DHCP;
         }
     }
 
     /* ---- 6 轮询确认，最多 3 次 ---- */
     for (int i = 0; i < 3; i++) {
         if (check_wifi_status(ssid) == 0) {
             printf("[wifi] 连接成功 SSID=\"%s\"\n", ssid);
             return WIFI_OK;
         }
         sleep(1);
     }
 
     clean_wifi();
     return WIFI_ERR_TIMEOUT;
 }
 