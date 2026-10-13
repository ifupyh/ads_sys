#include <stdio.h>
#include <string.h>
#include "api_handler.h"
#include "http_response.h"

void handle_get_login(struct mg_connection *c,struct mg_http_message *hm)
{
    char username[64]={0};
    char password[64]={0};
    //数据源从 地址栏提取请求参数,&hm->query
    mg_http_get_var(&hm->query,"username",username,sizeof(username));
    mg_http_get_var(&hm->query,"password",password,sizeof(password));
    printf("[GET] username=%s,password=%s\n",username,password);
    //处理登录业务
    if(!strcmp(username,"admin") && !strcmp(password,"123456")){
        send_json(c,200,"{\"code\":200,\"message\":\"success\"}");
    }
    else
    {
        send_json(c,401,"{\"code\":401,\"message\":\"unauthorized\"}");
    }
}



void handle_post_login(struct mg_connection *c,struct mg_http_message *hm)
{
    char username[64]={0};
    char password[64]={0};
    //数据源从 地址栏提取请求参数,&hm->query
    mg_http_get_var(&hm->body,"username",username,sizeof(username));
    mg_http_get_var(&hm->body,"password",password,sizeof(password));
    printf("[POST] username=%s,password=%s\n",username,password);
    //处理登录业务
    if(!strcmp(username,"admin") && !strcmp(password,"123456")){
        send_json(c,200,"{\"code\":200,\"message\":\"success\"}");
    }
    else
    {
        send_json(c,401,"{\"code\":401,\"message\":\"unauthorized\"}");
    }
}

