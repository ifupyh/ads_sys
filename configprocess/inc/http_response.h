/* http_response.h */
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "mongoose.h"

/* 统一 HTTP 响应出口：所有模块都通过这里回包，保证约定一致 */

void send_json(struct mg_connection *c, int code, const char *body);

void send_html(struct mg_connection *c, int code, const char *body);

void send_text(struct mg_connection *c, int code, const char *body);

void send_error(struct mg_connection *c, int code, const char *msg);

#endif /* HTTP_RESPONSE_H */