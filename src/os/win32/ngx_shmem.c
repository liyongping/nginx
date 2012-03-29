
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


ngx_int_t
ngx_shm_alloc(ngx_shm_t *shm)
{
    u_char    *name;
    uint64_t   size;
    // 申请一块空间存放共享内存名称
    name = ngx_alloc(shm->name.len + 2 + NGX_INT32_LEN, shm->log);
    if (name == NULL) {
        return NGX_ERROR;
    }

    (void) ngx_sprintf(name, "%V_%s%Z", &shm->name, ngx_unique);

    ngx_set_errno(0);

    size = shm->size;
    // 创建一个新的文件映射内核对象
    shm->handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                                    (u_long) (size >> 32),
                                    (u_long) (size & 0xffffffff),
                                    (char *) name);

    if (shm->handle == NULL) {
        ngx_log_error(NGX_LOG_ALERT, shm->log, ngx_errno,
                      "CreateFileMapping(%uz, %s) failed",
                      shm->size, name);
        ngx_free(name);

        return NGX_ERROR;
    }

    ngx_free(name);

    if (ngx_errno == ERROR_ALREADY_EXISTS) {
        shm->exists = 1;
    }
    // 将一个文件映射对象映射到当前应用程序的地址空间
    shm->addr = MapViewOfFile(shm->handle, FILE_MAP_WRITE, 0, 0, 0);

    if (shm->addr != NULL) {
        return NGX_OK;
    }

    ngx_log_error(NGX_LOG_ALERT, shm->log, ngx_errno,
                  "MapViewOfFile(%uz) of file mapping \"%V\" failed",
                  shm->size, &shm->name);
    //对象映射失败，关闭文件映射对象
    if (CloseHandle(shm->handle) == 0) {
        ngx_log_error(NGX_LOG_ALERT, shm->log, ngx_errno,
                      "CloseHandle() of file mapping \"%V\" failed",
                      &shm->name);
    }

    return NGX_ERROR;
}


void
ngx_shm_free(ngx_shm_t *shm)
{
    // 在当前应用程序的内存地址空间解除对一个文件映射对象的映射
    if (UnmapViewOfFile(shm->addr) == 0) {
        ngx_log_error(NGX_LOG_ALERT, shm->log, ngx_errno,
                      "UnmapViewOfFile(%p) of file mapping \"%V\" failed",
                      shm->addr, &shm->name);
    }
    // 关闭文件映射对象
    if (CloseHandle(shm->handle) == 0) {
        ngx_log_error(NGX_LOG_ALERT, shm->log, ngx_errno,
                      "CloseHandle() of file mapping \"%V\" failed",
                      &shm->name);
    }
}
