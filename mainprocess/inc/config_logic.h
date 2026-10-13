#ifndef CONFIG_LOGIC_H
#define CONFIG_LOGIC_H
#include <stdio.h>


size_t config_request_callback(void *data,size_t size, size_t nmemb,void* userp);

void send_config_request(char * ini_path);



#endif