#include <string.h>
#include "router.h"
#include "http_response.h"
#include "api_handler.h"    /* 引入各 API handler 的声明 */
#include "config_logic.h"
/* ------------------------------------------------------------------
 * 路由表：以后加接口只改这一张表，别处不用动
 * ------------------------------------------------------------------
 * 匹配顺序自上而下，先匹配到的先执行。
 * 通配符说明：
 *   ?  匹配 1 个字符
 *   *  匹配一段（不跨 /）
 *   #  匹配任意（可跨 /）
 * 更具体的模式请写在更前面。
 */
static const struct route s_routes[] = {
    /* method  pattern                handler             */
    { "GET",    "/api/login",         handle_get_login  },
    { "POST",   "/api/login",         handle_post_login },
    { "POST",   "/api/configsubmit",  handle_post_configsubmit },

    /* 示例：以后要加接口，只需往这里加一行 */
    /* { "GET",  "/api/status",       handle_get_status }, */
    /* { "GET",  "/api/user/*",       handle_get_user   }, */

    { NULL, NULL, NULL }   /* 哨兵：表尾 */
};

void router_dispatch(struct mg_connection *c, struct mg_http_message *hm) {
    for (const struct route *r = s_routes; r->pattern != NULL; r++) {
        /* 方法过滤：r->method 为 NULL 表示任意方法 */
        if (r->method != NULL &&
            !mg_match(hm->method, mg_str(r->method), NULL)) {
            continue;
        }
        /* URI 匹配 */
        if (mg_match(hm->uri, mg_str(r->pattern), NULL)) {
            r->handler(c, hm);
            return;
        }
    }
    /* 未命中 */
    send_error(c, 404, "not found");
}