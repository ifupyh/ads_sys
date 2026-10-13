#ifndef CONFIG_H
#define CONFIG_H

#define HTTP_ADDE "http://0.0.0.0:8080"

//static
// 优先使用 CMake 注入的绝对路径（见 configprocess/CMakeLists.txt），
// 这样进程从任意工作目录启动都能找到资源；未注入时退回相对路径。
#ifndef STATIC_ROOT
#define STATIC_ROOT "./www"
#endif
#ifndef STAITC_404
#define STAITC_404 "./www/404.html"
#endif
#define STAITC_MIME "html=text/html,css=text/css,js=application/javascript,txt=text/plain"

//api
#define API_PREFIX "/api/"


#endif