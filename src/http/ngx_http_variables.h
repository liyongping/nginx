
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HTTP_VARIABLES_H_INCLUDED_
#define _NGX_HTTP_VARIABLES_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef ngx_variable_value_t  ngx_http_variable_value_t;

#define ngx_http_variable(v)     { sizeof(v) - 1, 1, 0, 0, 0, (u_char *) v }

typedef struct ngx_http_variable_s  ngx_http_variable_t;

typedef void (*ngx_http_set_variable_pt) (ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
typedef ngx_int_t (*ngx_http_get_variable_pt) (ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);

/*
NGX_HTTP_VAR_CHANGEABLE宏的时候意味着，该变量可以重复配置，一般后配置的会覆盖前面的配置。如：
set $file $1
set $file $2
那么结果就是file的最终值就是$2所代表的
 */
#define NGX_HTTP_VAR_CHANGEABLE   1
// NGX_HTTP_VAR_NOCACHEABLE宏所影响的正是ngx_variable_value_t结构中的no_cacheable，凡是有该标记的变量，都要通过get_handler来获取变量的值
// 这种变量往往是跟特定的请求紧密相关的，如host，uri，args之类的，处理时每次重新获取新值时必要的
#define NGX_HTTP_VAR_NOCACHEABLE  2
/*
NGX_HTTP_VAR_INDEXED和NGX_HTTP_VAR_NOHASH
这两个宏主要是在SSI相关处理中用到，而ssi的处理需要用到cmcf->variables_hash，这是hash表，可以高效的找到变量
 */
#define NGX_HTTP_VAR_INDEXED      4
#define NGX_HTTP_VAR_NOHASH       8


struct ngx_http_variable_s {
    ngx_str_t                     name;   /* 变量的名称, must be first to build the hash*/
    ngx_http_set_variable_pt      set_handler;  // 变量的设置函数
    ngx_http_get_variable_pt      get_handler;  // 变量的get函数
    uintptr_t                     data;         // 传给get与set_handler的值
    ngx_uint_t                    flags;        // 变量的标志
    ngx_uint_t                    index;        // 如果有索引，则是变量的索引号
};

// name: 即变量的名字
// flags: 如果同一个变量要多次添加，则flags应该设置 NGX_HTTP_VAR_CHANGEABLE
// 否则，多次添加将会提示重复
// flags表示可以是：NGX_HTTP_VAR_CHANGEABLE
//                 NGX_HTTP_VAR_NOCACHEABLE
//                 NGX_HTTP_VAR_INDEXED
//                 NGX_HTTP_VAR_NOHASH
ngx_http_variable_t *ngx_http_add_variable(ngx_conf_t *cf, ngx_str_t *name,
    ngx_uint_t flags);

// 通过ngx_http_get_variable_index来添加一个变量的索引
ngx_int_t ngx_http_get_variable_index(ngx_conf_t *cf, ngx_str_t *name);
ngx_http_variable_value_t *ngx_http_get_indexed_variable(ngx_http_request_t *r,
    ngx_uint_t index);
ngx_http_variable_value_t *ngx_http_get_flushed_variable(ngx_http_request_t *r,
    ngx_uint_t index);

// 没有索引过的变量，则只能通过ngx_http_get_variable函数来获取, key 由ngx_hash_strlow来计算, 所以变量名是没有大小写区分的
ngx_http_variable_value_t *ngx_http_get_variable(ngx_http_request_t *r,
    ngx_str_t *name, ngx_uint_t key);

ngx_int_t ngx_http_variable_unknown_header(ngx_http_variable_value_t *v,
    ngx_str_t *var, ngx_list_part_t *part, size_t prefix);


#define ngx_http_clear_variable(r, index) r->variables0[index].text.data = NULL;


#if (NGX_PCRE)

typedef struct {
    ngx_uint_t                    capture;
    ngx_int_t                     index;
} ngx_http_regex_variable_t;


typedef struct {
    ngx_regex_t                  *regex;
    ngx_uint_t                    ncaptures;
    ngx_http_regex_variable_t    *variables;
    ngx_uint_t                    nvariables;
    ngx_str_t                     name;
} ngx_http_regex_t;


typedef struct {
    ngx_http_regex_t             *regex;
    void                         *value;
} ngx_http_map_regex_t;


ngx_http_regex_t *ngx_http_regex_compile(ngx_conf_t *cf,
    ngx_regex_compile_t *rc);
ngx_int_t ngx_http_regex_exec(ngx_http_request_t *r, ngx_http_regex_t *re,
    ngx_str_t *s);

#endif


typedef struct {
    ngx_hash_combined_t           hash;
#if (NGX_PCRE)
    ngx_http_map_regex_t         *regex;
    ngx_uint_t                    nregex;
#endif
} ngx_http_map_t;


void *ngx_http_map_find(ngx_http_request_t *r, ngx_http_map_t *map,
    ngx_str_t *match);


ngx_int_t ngx_http_variables_add_core_vars(ngx_conf_t *cf);
ngx_int_t ngx_http_variables_init_vars(ngx_conf_t *cf);


extern ngx_http_variable_value_t  ngx_http_variable_null_value;
extern ngx_http_variable_value_t  ngx_http_variable_true_value;


#endif /* _NGX_HTTP_VARIABLES_H_INCLUDED_ */
