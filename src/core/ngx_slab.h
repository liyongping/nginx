
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_SLAB_H_INCLUDED_
#define _NGX_SLAB_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct ngx_slab_page_s  ngx_slab_page_t;
// 页结构体
struct ngx_slab_page_s {
    uintptr_t         slab; // 保存当前页的一些信息
    ngx_slab_page_t  *next;
    uintptr_t         prev;
};


typedef struct {
    ngx_shmtx_sh_t    lock;     // 锁，因为slab在nginx中一般配合共享内存使用

    size_t            min_size; // 分配空间的最小值
    size_t            min_shift;// 该最小值对应的移位数

    ngx_slab_page_t  *pages;    // 页数组
    ngx_slab_page_t   free;     // 空闲的页

    u_char           *start;    // 分配地址开始地址
    u_char           *end;

    ngx_shmtx_t       mutex;

    u_char           *log_ctx;
    u_char            zero;

    void             *data;
    void             *addr;
} ngx_slab_pool_t;

// 初始化slab池
void ngx_slab_init(ngx_slab_pool_t *pool);

void *ngx_slab_alloc(ngx_slab_pool_t *pool, size_t size);
// 在调用前已加锁，分配指定大小空间
void *ngx_slab_alloc_locked(ngx_slab_pool_t *pool, size_t size);
void ngx_slab_free(ngx_slab_pool_t *pool, void *p);
// 释放空间
void ngx_slab_free_locked(ngx_slab_pool_t *pool, void *p);


#endif /* _NGX_SLAB_H_INCLUDED_ */
