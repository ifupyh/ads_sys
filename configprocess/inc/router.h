#ifndef ROUTER_H
#define ROUTER_H

#include "mongoose.h"

/* 路由处理函数签名：和 handle_api 里各个 handler 一致 */
typedef void (*route_fn)(struct mg_connection *c,
                         struct mg_http_message *hm);

/*
 * 一条路由：
 *   method  —— "GET" / "POST" / NULL（NULL 表示任意方法）
 *   pattern —— mg_match 模式，如 "/api/login"、"/api/user/*"
 *   handler —— 命中后调用
 */
struct route {
    const char *method;
    const char *pattern;
    route_fn    handler;
};

/*
 * 遍历路由表分发请求。命中则调用 handler 并返回；
 * 未命中则回 404。
 */
void router_dispatch(struct mg_connection *c, struct mg_http_message *hm);

#endif