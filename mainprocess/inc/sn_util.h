#ifndef SN_UTIL_H
#define SN_UTIL_H


#include <stdio.h>
#include <stdlib.h>

/**
 * @brief 获取设备SN，内部静态缓冲区，无需手动free
 * @return 字符串指针，下次调用本函数内容会被覆盖
 */
char* get_device_sn();
#endif /* SN_UTIL_H */