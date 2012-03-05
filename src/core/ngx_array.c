
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


ngx_array_t *
ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size)
{
    ngx_array_t *a;
    // 申请数组头
    a = ngx_palloc(p, sizeof(ngx_array_t));
    if (a == NULL) {
        return NULL;
    }
    // 从内存池中申请大小为size，长度为n的空间
    a->elts = ngx_palloc(p, n * size);
    if (a->elts == NULL) {
        return NULL;
    }

    a->nelts = 0;
    a->size = size;
    a->nalloc = n;
    a->pool = p;

    return a;
}


void
ngx_array_destroy(ngx_array_t *a)
{
    ngx_pool_t  *p;

    p = a->pool;
    // 内存池回收空间
    if ((u_char *) a->elts + a->size * a->nalloc == p->d.last) {
        p->d.last -= a->size * a->nalloc;
    }

    if ((u_char *) a + sizeof(ngx_array_t) == p->d.last) {
        p->d.last = (u_char *) a;
    }
}


void *
ngx_array_push(ngx_array_t *a)
{
    void        *elt, *new;
    size_t       size;
    ngx_pool_t  *p;

    if (a->nelts == a->nalloc) {

        /* the array is full */
        // 空间已满
        size = a->size * a->nalloc;     // 计算当前数组的大小

        p = a->pool;
        // 检查当前内存池上是否还有足够的空间
        if ((u_char *) a->elts + size == p->d.last
            && p->d.last + a->size <= p->d.end)
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += a->size;
            a->nalloc++;

        } else {
            /* allocate a new array */
            // 内存池空间不够，进行*2的扩容
            new = ngx_palloc(p, 2 * size);
            if (new == NULL) {
                return NULL;
            }
            // 原来的数据拷贝到扩容后的空间头部
            ngx_memcpy(new, a->elts, size);
            a->elts = new;
            a->nalloc *= 2;     //数组总大小*2
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts++;

    return elt;     // 返回可用的长度为1*size的空间地址给用户
}


void *
ngx_array_push_n(ngx_array_t *a, ngx_uint_t n)
{
    void        *elt, *new;
    size_t       size;
    ngx_uint_t   nalloc;
    ngx_pool_t  *p;

    size = n * a->size;
    // 检查当前数组空间是否足够
    if (a->nelts + n > a->nalloc) {

        /* the array is full */

        p = a->pool;

        if ((u_char *) a->elts + a->size * a->nalloc == p->d.last
            && p->d.last + size <= p->d.end)
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += size;
            a->nalloc += n;

        } else {
            /* allocate a new array */
            // 当前内存池空间不够，进行扩容，基数在原来数组大小和需要的空间n中取大的决定
            nalloc = 2 * ((n >= a->nalloc) ? n : a->nalloc);

            new = ngx_palloc(p, nalloc * a->size);
            if (new == NULL) {
                return NULL;
            }
            // 原来的数据拷贝到新的空间中
            ngx_memcpy(new, a->elts, a->nelts * a->size);
            a->elts = new;
            a->nalloc = nalloc;
        }
    }
    // 数组空出n个位置来
    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts += n;

    return elt;     // 返回可用的长度为n*size的空间地址给用户
}
