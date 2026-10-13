#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "mongoose.h"

void handle_get_login(struct mg_connection *c,struct mg_http_message *hm);
void handle_post_login(struct mg_connection *c,struct mg_http_message *hm);




#endif