﻿
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_SHMEM_H_INCLUDED_
#define _NGX_SHMEM_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct {
    u_char      *addr;      //指向共享内存的指针
    size_t       size;      //共享内存大小
    ngx_str_t    name;      //共享内存名称
    HANDLE       handle;    //共享内存句柄
    ngx_log_t   *log;
    ngx_uint_t   exists;   /* unsigned  exists:1;  */
} ngx_shm_t;

// 申请共享内存
ngx_int_t ngx_shm_alloc(ngx_shm_t *shm);
// 释放共享内存
void ngx_shm_free(ngx_shm_t *shm);


#endif /* _NGX_SHMEM_H_INCLUDED_ */
