#include <stdio.h>
#include <curl/curl.h>
#include <cJSON.h>





size_t get_requset_fn(void *ptr,size_t size,size_t nmemb,void *stream)
{
    char *josnString =(char *)ptr;
    printf("%s",josnString);
    cJSON* root=cJSON_Parse(josnString);
    if (root)
    {
        cJSON *success=cJSON_GetObjectItemCaseSensitive(root,"success");
        cJSON *code=cJSON_GetObjectItemCaseSensitive(root,"code");
        cJSON *message=cJSON_GetObjectItemCaseSensitive(root,"message");
        if (cJSON_IsTrue(success))
        {
            printf("success:%d\n",success->valueint);
        }
        else
        {
            printf("code:%d\n",code->valueint);
            printf("message:%s\n",message->valuestring);
        }
        cJSON_Delete(root);
    }
    return size*nmemb;
}
void send_get_request()
{
    curl_global_init(CURL_REDIR_GET_ALL);
    CURL *curl=curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl,CURLOPT_URL,"http://192.168.150.31/config.json?xxx=xx");
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_requset_fn);
        CURLcode res=curl_easy_perform(curl);
        if (res!=CURLE_OK)
        {
            printf("curl_easy_perform failed: %s\n",curl_easy_strerror(res));
            /* code */
        }
       
        curl_easy_cleanup(curl);
    }
     curl_global_cleanup();
}


int main(int argc, char const *argv[])
{
    send_get_request();
    return 0;
}
