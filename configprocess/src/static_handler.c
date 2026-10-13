#include "static_handler.h"
#include "http_config.h"

void handle_static(struct mg_connection* c,struct mg_http_message* hm)
{
    struct mg_http_serve_opts ops=
    {
        .root_dir=STATIC_ROOT,
        .ssi_pattern=NULL,
        .extra_headers=NULL,
        .mime_types=STAITC_MIME,
        .page404=STAITC_404,
        .fs=NULL,
    };
    mg_http_serve_dir(c,hm,&ops);



    
}