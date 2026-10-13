#include <string.h>
#include "config_parse.h"
#include "curl_util.h"
#include "cjson/cJSON.h"
#include "config_logic.h"
#include "sn_util.h"
static int g_ok;



size_t config_request_callback(void *data,size_t size, size_t nmemb,void* userp)
{
    (void)userp;
    size_t realsize =size*nmemb;
    char buf[4096];
    size_t n=(realsize<sizeof(buf)-1?realsize:sizeof(buf)-1);
    memcpy(buf,data,n);
    buf[n]='\0';
    cJSON *root =cJSON_Parse(buf);
    if (root)
    {
        char *pretty=cJSON_Print(root);
        printf("配置请求的响应结果%s\n",pretty?pretty:buf);
        cJSON_free(pretty);
        cJSON* success=cJSON_GetObjectItemCaseSensitive(root,"success");
        g_ok=cJSON_IsTrue(success);
        cJSON_Delete(root);
    }
    return realsize;
    
}

void send_config_request(char * ini_path)
{
    config_t cfg;
    if (config_parse_parse(&cfg,ini_path)!=0)
    {
        return;
    }
    

   //2.设备的sn(每一台设备的sn是唯一的)，先写死
    char *sn=get_device_sn();

    char url[512];

    snprintf(url,sizeof(url),"http://%s/config.json?area=%d&sn=%s&detailAddress=%s",
    cfg.backendUrl,
    cfg.area,sn,cfg.detailAddress);
    printf("配置请求的url:%s\n",url);
    g_ok=0;
    sendGetRequest(url,config_request_callback);
    printf("配置请求结果:%s\n",g_ok?"成功":"失败");
 
}




