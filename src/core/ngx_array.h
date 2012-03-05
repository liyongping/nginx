
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_ARRAY_H_INCLUDED_
#define _NGX_ARRAY_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


struct ngx_array_s {
    void        *elts;          // 指向数据的内存空间
    ngx_uint_t   nelts;         // 当前数据已经使用的大小
    size_t       size;          // 数组元大小
    ngx_uint_t   nalloc;        // 已分配空间中可存放的元素个数，即数组大小
    ngx_pool_t  *pool;          // 存放数据的内存池
};


ngx_array_t *ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size);
// 销毁数组，实际内存空间没有释放，只是还给了内存池
void ngx_array_destroy(ngx_array_t *a);
// 在数组中申请一个单位的空间
void *ngx_array_push(ngx_array_t *a);
// 在数组中申请n个单位的空间
void *ngx_array_push_n(ngx_array_t *a, ngx_uint_t n);


static ngx_inline ngx_int_t
ngx_array_init(ngx_array_t *array, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    /*
     * set "array->nelts" before "array->elts", otherwise MSVC thinks
     * that "array->nelts" may be used without having been initialized
     */

    array->nelts = 0;
    array->size = size;
    array->nalloc = n;
    array->pool = pool;
    // 从内存池中申请大小为size，长度为n的空间
    array->elts = ngx_palloc(pool, n * size);
    if (array->elts == NULL) {
        return NGX_ERROR;
    }

    return NGX_OK;
}


#endif /* _NGX_ARRAY_H_INCLUDED_ */
