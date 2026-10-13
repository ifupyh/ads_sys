#ifndef CURL_REQUEST_H
#define CURL_REQUEST_H

#include <curl/curl.h>

/* 发 GET：url 是拼好的完整地址，fn 是响应写回调（由调用方提供） */
void sendGetRequest(char *url, size_t (*fn)(void *, size_t, size_t, void *));

/* 发 POST：parmStr 是 "a=1&b=2" 形式的请求体（配置请求用 GET，心跳上报会用到 POST） */
void sendPostRequest(char *url, char *parmStr, size_t (*fn)(void *, size_t, size_t, void *));

#endif /* CURL_REQUEST_H */