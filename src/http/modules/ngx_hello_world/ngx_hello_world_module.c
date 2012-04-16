
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>

static char *ngx_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_hello_world_get_output(ngx_http_request_t *r, char *out_buf);
static ngx_int_t ngx_hello_world_handler(ngx_http_request_t *r);

/* Commands */
static ngx_command_t  ngx_hello_world_commands[] = {
    { ngx_string("ngx_hello_world"),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_hello_world,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

      ngx_null_command
};

static ngx_http_module_t  ngx_hello_world_module_ctx = {
    NULL,                                  /* preconfiguration */
    NULL,                                     /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};

/* hook */
ngx_module_t  ngx_hello_world_module = {
    NGX_MODULE_V1,
    &ngx_hello_world_module_ctx,              /* module context */
    ngx_hello_world_commands,                 /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,             /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,             /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_int_t ngx_hello_world_get_output(ngx_http_request_t *r, char *out_buf){

    sprintf(out_buf, "%s", "Hello World!");
    return NGX_OK;
}

static ngx_int_t
ngx_hello_world_handler(ngx_http_request_t *r)
{
    ngx_int_t     rc;
    ngx_buf_t    *b;
    ngx_chain_t   out;

    /* Http Output Buffer */
    char out_buf[20] = {0};

    if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD))) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    rc = ngx_http_discard_request_body(r);

    if (rc != NGX_OK && rc != NGX_AGAIN) {
        return rc;
    }
    //定制头信息
    r->headers_out.content_type.len = sizeof("text/html") - 1;
    r->headers_out.content_type.data = (u_char *) "text/html";

    if (r->method == NGX_HTTP_HEAD) {
        rc = ngx_http_send_header(r);

        if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
            return rc;
        }
    }

    //获取输出的body
    ngx_hello_world_get_output(r, out_buf);

    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    out.buf = b;
    out.next = NULL;

    b->pos = (u_char *)out_buf;
    b->last = (u_char *)out_buf + strlen(out_buf);
    b->memory = 1;
    b->last_buf = 1;
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = strlen(out_buf);

    rc = ngx_http_send_header(r);

    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    return ngx_http_output_filter(r, &out);
}

static char *
ngx_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    /* register hanlder */
    clcf->handler = ngx_hello_world_handler;

    return NGX_CONF_OK;
}

