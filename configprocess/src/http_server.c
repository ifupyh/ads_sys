#include <stdio.h>
#include <signal.h>
#include "mongoose.h"
#include "static_handler.h"
#include "http_server.h"
#include "http_config.h"
#include "api_handler.h"
#include "router.h"
static volatile int g_runing=1;

static void on_signal(int sig)
{
    (void)sig;
    g_runing=0;

}

static void on_http_event(struct mg_connection *c ,int ev,void *ev_data)
{
    if (ev!=MG_EV_HTTP_MSG) return;
    struct mg_http_message *hm=(struct mg_http_message*)ev_data;
    printf("url=%.*s\n",(int)hm->uri.len,hm->uri.buf);
    if (mg_match(hm->uri,mg_str("/api/#"),NULL))
    {
        router_dispatch(c,hm);
    }
    else
    {
        handle_static(c,hm);
    }
}



int http_server_run(void)
{   
    signal(SIGINT,on_signal);
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    struct mg_connection *c=mg_http_listen(&mgr,HTTP_ADDE,on_http_event,NULL);
    if (c==NULL)
    {
        printf("无法再以下位置创建监听器 %s\n",HTTP_ADDE);
        mg_mgr_free(&mgr);
        return 1;
    }
    printf("HTTP服务器已经启动,监听 %s\n",HTTP_ADDE);
    printf("Press Ctrl + c stop\n");
    while (g_runing)
    {
        mg_mgr_poll(&mgr,1000);
    }
    mg_mgr_free(&mgr);
    printf("Server stopped\n");
    return 0;
    
}