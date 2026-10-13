#include <stdio.h>
#include <string.h>
#include "mongoose.h"
#include "http_response.h"
#include "config_logic.h"
#include "wifi_utils.h"
#include "glib.h"
#include "msgqueue.h"
const char*config_get_path(void)
{
    static char s_config_path[512]={0};
    if (s_config_path[0]=='\0')
    {
        char *p=g_build_filename(g_get_home_dir(),".config","ads_config.ini",NULL);
        snprintf(s_config_path,sizeof(s_config_path),"%s",p);
        g_free(p);
        printf("[config]的配置文件路径为：%s",s_config_path);
    }
    return s_config_path;
}

static int config_store_save(const devic_config_t* cfg)
{
    GKeyFile*kf=g_key_file_new();
    g_key_file_set_string(kf,"server","backendUrl",cfg->backendUrl);
    g_key_file_set_string(kf,"server","mqttUrl",cfg->mqttUrl);
    g_key_file_set_string(kf,"wifi","wifiName",cfg->wifiName);
    g_key_file_set_string(kf,"wifi","wifiPassword",cfg->wifiPassword);
    g_key_file_set_string(kf,"device","area",cfg->area);
    g_key_file_set_string(kf,"device","detailAddress",cfg->detailAddress);

    const char*path=config_get_path();
    char *dir=g_path_get_dirname(path);
    g_mkdir_with_parents(dir,0755);
    g_free(dir);

    GError *err=NULL;
    int ret=0;
    if (!g_key_file_save_to_file(kf,path,&err))
    {
        fprintf(stderr,"save ini failed:%s\n",err->message);
        g_error_free(err);
        ret=1;    
    }
    else
    {
        printf("[config]saved -> %s\n",path);
    }
    g_key_file_free(kf);
    return ret;
}



void handle_post_configsubmit(struct mg_connection *c,struct mg_http_message *hm)
{
    devic_config_t cfg;
    memset(&cfg,0,sizeof(cfg));
    mg_http_get_var(&hm->body,"backendUrl",cfg.backendUrl,sizeof(cfg.backendUrl));
    mg_http_get_var(&hm->body,"area",cfg.area,sizeof(cfg.area));
    mg_http_get_var(&hm->body,"detailAddress",cfg.detailAddress,sizeof(cfg.detailAddress));
    mg_http_get_var(&hm->body,"mqttUrl",cfg.mqttUrl,sizeof(cfg.mqttUrl));
    mg_http_get_var(&hm->body,"wifiName",cfg.wifiName,sizeof(cfg.wifiName));
    mg_http_get_var(&hm->body,"wifiPassword",cfg.wifiPassword,sizeof(cfg.wifiPassword));

    int wifi_ret=connect_wifi(cfg.wifiName,cfg.wifiPassword);
    printf("wifi %d(%s)\n",wifi_ret,wifi_strerror(wifi_ret));
    if (config_store_save(&cfg))
    {
        send_error(c,500,"save config failed\n");
        return;
    }
    
    int msgid=msgq_create();
    if (msgid>=0)
    {
        if( !msgq_send(msgid,MSG_TYPE_CONFIG,config_get_path()) )
        {
            printf("[config] send msg ok\n");
        }
        else
        {
            printf("[config] send msg failed,消息队列不可以用,跳过通知!\n");
        }
    }

    /* ---------- 4 拼 JSON，交给 send_json 统一出口 ---------- */
    char body[1024];
    snprintf(body, sizeof(body),
        "{\"code\":200,\"message\":\"config received\",\"data\":{"
        "\"backendUrl\":\"%s\",\"mqttUrl\":\"%s\","
        "\"wifiName\":\"%s\",\"wifiPassword\":\"%s\","
        "\"area\":\"%s\",\"detailAddress\":\"%s\","
        "\"wifiConnected\":%d}}",
        cfg.backendUrl, cfg.mqttUrl, cfg.wifiName, cfg.wifiPassword,
        cfg.area, cfg.detailAddress,
        wifi_ret == WIFI_OK ? 1 : 0);
        send_json(c, 200, body);
    }



