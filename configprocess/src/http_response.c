#include "http_response.h"

void send_json(struct mg_connection *c, int code, const char *body) {
    mg_http_reply(c, code,
                  "Content-Type: application/json; charset=utf-8\r\n",
                  "%s", body);
}

void send_html(struct mg_connection *c, int code, const char *body) {
    mg_http_reply(c, code,
                  "Content-Type: text/html; charset=utf-8\r\n",
                  "%s", body);
}

void send_text(struct mg_connection *c, int code, const char *body) {
    mg_http_reply(c, code,
                  "Content-Type: text/plain; charset=utf-8\r\n",
                  "%s", body);
}

/* 统一错误响应：自动构造 {"code":N,"message":"..."}
   用 MG_ESC 做 JSON 转义，msg 里有 " 或 \ 也不会破格式 */
void send_error(struct mg_connection *c, int code, const char *msg) {
    mg_http_reply(c, code,
                  "Content-Type: application/json; charset=utf-8\r\n",
                  "{%m:%d,%m:%m}",
                  MG_ESC("code"),    code,
                  MG_ESC("message"), MG_ESC(msg));
}