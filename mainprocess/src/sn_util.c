#include "sn_util.h"
#include <string.h>
#include <ctype.h>

static void trim_newline(char *str){
    if (!str) return;
    size_t len = strlen(str);
    if (len && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

static void remove_all_space(char *str)
{
    if (!str) return;
    int i = 0, j = 0;
    while (str[i])
    {
        if (str[i] != ' ') str[j++] = str[i];
        i++;
    }
    str[j] = '\0';
}

char* get_device_sn(){
    // 静态全局缓冲区，生命周期全程，不用malloc/free
    static char sn_buf[128] = {0};
    const char path[] = "/sys/class/dmi/id/product_serial";
    FILE *fp = fopen(path, "r");

    if (fp)
    {
        if (fgets(sn_buf, sizeof(sn_buf), fp))
        {
            trim_newline(sn_buf);
            remove_all_space(sn_buf);
        }
        fclose(fp);
    }
    else
    {
        // 读取失败填充默认SN
        strcpy(sn_buf, "linux-ifupyh");
    }
    return sn_buf;
}