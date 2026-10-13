#include <stdio.h>
#include "curl_util.h"

/* 发 GET 请求。libcurl 固定四步：init -> setopt -> perform -> cleanup
 * 响应不在这里处理，交给 fn（调用方传进来的写回调） */
void sendGetRequest(char *url, size_t (*fn)(void *, size_t, size_t, void *)){
    /* 1 拿句柄 */
    CURL *curl = curl_easy_init();          
    if (!curl) return;
    /* 2 请求地址 */
    curl_easy_setopt(curl, CURLOPT_URL, url);    
    /*   响应到了调谁 */       
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fn);  
    /* 3 发请求，阻塞到响应收完 */
    CURLcode code = curl_easy_perform(curl);            
    /* 4 只看网络通不通；业务成功与否看响应里的 success */
    if (code != CURLE_OK) {                             
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(code));
    }
    /*   释放句柄 */
    curl_easy_cleanup(curl);                            
}

/* 发 POST 请求，和 GET 只差一行：设置请求体 CURLOPT_POSTFIELDS */
void sendPostRequest(char *url, char *parmStr, size_t (*fn)(void *, size_t, size_t, void *)){
    CURL *curl = curl_easy_init();
    if (!curl) return;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    /* 请求体 */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, parmStr);   
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fn);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(code));
    }
    curl_easy_cleanup(curl);
}