
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_SHMEM_H_INCLUDED_
#define _NGX_SHMEM_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct {
    u_char      *addr;      //ָ�����ڴ��ָ��
    size_t       size;      //�����ڴ��С
    ngx_str_t    name;      //�����ڴ�����
    HANDLE       handle;    //�����ڴ���
    ngx_log_t   *log;
    ngx_uint_t   exists;   /* unsigned  exists:1;  */
} ngx_shm_t;

// ���빲���ڴ�
ngx_int_t ngx_shm_alloc(ngx_shm_t *shm);
// �ͷŹ����ڴ�
void ngx_shm_free(ngx_shm_t *shm);


#endif /* _NGX_SHMEM_H_INCLUDED_ */
