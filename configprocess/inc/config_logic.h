#ifndef CONFIG_LOGIC_H
#define CONFIG_LOGIC_H
#include "mongoose.h"


typedef struct 
{
    char backendUrl[128];
    char mqttUrl[128];
    char wifiName[128];
    char wifiPassword[128];
    char area[8];
    char detailAddress[128];
}devic_config_t;
void handle_post_configsubmit(struct mg_connection *c,struct mg_http_message *hm);
const char*config_get_path(void);
#endif