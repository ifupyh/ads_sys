/* config_parse.c —— 用 glib 的 GKeyFile 读 ini
 * 组名/键名按你自己的 configprocess 存的那份来：
 *   [server] backendUrl / mqttUrl
 *   [wifi]   wifiName / wifiPassword
 *   [device] area（字符串）/ detailAddress          */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include "config_parse.h"

/* 取一个键的值填进 dest，一个函数同时办两件事：
 *   1) 键不存在时 g_key_file_get_string 返回 NULL，必须兜底（否则 strcpy(NULL) 直接段错误）
 *   2) 它返回的是堆内存，用完要 g_free                                  */
static void get_into(GKeyFile *kf, const char *group, const char *key,
                     char *dest, int dest_size)
{
    char *v = g_key_file_get_string(kf, group, key, NULL);
    snprintf(dest, dest_size, "%s", v ? v : "");
    g_free(v);
}

int config_parse_parse(config_t *cfg, const char *ini_path)
{
    memset(cfg, 0, sizeof(*cfg));

    /* 1 ini 路径：消息里带来的优先；没带就用 $HOME/.config/ads_config.ini
     *   不能写成 "~/.config/..."，glib 不展开 ~，会当成当前目录下叫 "~" 的文件夹 */
    char *path = ini_path ? g_strdup(ini_path)
                          : g_build_filename(g_get_home_dir(), ".config",
                                             "ads_config.ini", NULL);

    /* 2 打开文件 */
    GKeyFile *kf  = g_key_file_new();
    GError   *err = NULL;
    if (!g_key_file_load_from_file(kf, path, G_KEY_FILE_NONE, &err)) {
        fprintf(stderr, "打开 ini 失败 %s: %s\n", path, err->message);
        g_error_free(err);
        g_key_file_free(kf);
        g_free(path);
        return -1;
    }
    printf("configPath = %s\n", path);

    /* 3 取 6 项填进结构体 */
    get_into(kf, "server", "backendUrl",    cfg->backendUrl,    sizeof(cfg->backendUrl));
    get_into(kf, "server", "mqttUrl",       cfg->mqttUrl,       sizeof(cfg->mqttUrl));
    get_into(kf, "wifi",   "wifiName",      cfg->wifiName,      sizeof(cfg->wifiName));
    get_into(kf, "wifi",   "wifiPassword",  cfg->wifiPassword,  sizeof(cfg->wifiPassword));
    get_into(kf, "device", "detailAddress", cfg->detailAddress, sizeof(cfg->detailAddress));

    /* area 在 ini 里存的是字符串 "2"，结构体里是 int，要转一下 */
    char area[16];
    get_into(kf, "device", "area", area, sizeof(area));
    cfg->area = atoi(area);

    g_key_file_free(kf);
    g_free(path);
    return 0;
}