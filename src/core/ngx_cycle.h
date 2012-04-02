
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CYCLE_H_INCLUDED_
#define _NGX_CYCLE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


#ifndef NGX_CYCLE_POOL_SIZE
#define NGX_CYCLE_POOL_SIZE     16384
#endif


#define NGX_DEBUG_POINTS_STOP   1
#define NGX_DEBUG_POINTS_ABORT  2


typedef struct ngx_shm_zone_s  ngx_shm_zone_t;

typedef ngx_int_t (*ngx_shm_zone_init_pt) (ngx_shm_zone_t *zone, void *data);

struct ngx_shm_zone_s {
    void                     *data;
    ngx_shm_t                 shm;
    ngx_shm_zone_init_pt      init;
    void                     *tag;
};


struct ngx_cycle_s {
    void                  ****conf_ctx;             // 上下文
    ngx_pool_t               *pool;                 // 对应内存池

    ngx_log_t                *log;                  // 日志
    ngx_log_t                 new_log;

    ngx_connection_t        **files;
    ngx_connection_t         *free_connections;     // 空闲连接
    ngx_uint_t                free_connection_n;    // 空闲连接数

    ngx_queue_t               reusable_connections_queue;   // 再利用连接队列

    ngx_array_t               listening;            // 监听数组，存放监听的套接字
    ngx_array_t               pathes;
    ngx_list_t                open_files;           // 需要打开的文件链表，如access.log，error.log
    ngx_list_t                shared_memory;        // 共享内存链表

    ngx_uint_t                connection_n;         // 连接个数, 由配置文件中的worker_connections设置
    ngx_uint_t                files_n;              // 文件打开数

    ngx_connection_t         *connections;          // 连接
    ngx_event_t              *read_events;          // 读事件
    ngx_event_t              *write_events;         // 写事件

    ngx_cycle_t              *old_cycle;            // 指向old_cycle

    ngx_str_t                 conf_file;            // 配置文件
    ngx_str_t                 conf_param;           // 参数
    ngx_str_t                 conf_prefix;          // 配置文件前缀
    ngx_str_t                 prefix;
    ngx_str_t                 lock_file;            // 锁文件
    ngx_str_t                 hostname;             // 主机名
};


typedef struct {
     ngx_flag_t               daemon;   // 标记是否为开启daemon进程，默认为1,表示开启，对应：daemon on;
     ngx_flag_t               master;   // 标记是否为开启master进程，默认为1,表示开启，对应：master_process on;

     ngx_msec_t               timer_resolution;

     ngx_int_t                worker_processes;
     ngx_int_t                debug_points;

     ngx_int_t                rlimit_nofile;
     ngx_int_t                rlimit_sigpending;
     off_t                    rlimit_core;

     int                      priority;

     ngx_uint_t               cpu_affinity_n;
     u_long                  *cpu_affinity;

     char                    *username; // 用户名
     ngx_uid_t                user;     // 用户ID
     ngx_gid_t                group;    // 组ID

     ngx_str_t                working_directory;
     ngx_str_t                lock_file;

     ngx_str_t                pid;
     ngx_str_t                oldpid;

     ngx_array_t              env;
     char                   **environment;

#if (NGX_THREADS)
     ngx_int_t                worker_threads;
     size_t                   thread_stack_size;
#endif

} ngx_core_conf_t;


typedef struct {
     ngx_pool_t              *pool;   /* pcre's malloc() pool */
} ngx_core_tls_t;


#define ngx_is_init_cycle(cycle)  (cycle->conf_ctx == NULL)


ngx_cycle_t *ngx_init_cycle(ngx_cycle_t *old_cycle);
ngx_int_t ngx_create_pidfile(ngx_str_t *name, ngx_log_t *log);
void ngx_delete_pidfile(ngx_cycle_t *cycle);
ngx_int_t ngx_signal_process(ngx_cycle_t *cycle, char *sig);
void ngx_reopen_files(ngx_cycle_t *cycle, ngx_uid_t user);
char **ngx_set_environment(ngx_cycle_t *cycle, ngx_uint_t *last);
ngx_pid_t ngx_exec_new_binary(ngx_cycle_t *cycle, char *const *argv);
u_long ngx_get_cpu_affinity(ngx_uint_t n);
ngx_shm_zone_t *ngx_shared_memory_add(ngx_conf_t *cf, ngx_str_t *name,
    size_t size, void *tag);


extern volatile ngx_cycle_t  *ngx_cycle;
extern ngx_array_t            ngx_old_cycles;
extern ngx_module_t           ngx_core_module;
extern ngx_uint_t             ngx_test_config;
extern ngx_uint_t             ngx_quiet_mode;
#if (NGX_THREADS)
extern ngx_tls_key_t          ngx_core_tls_key;
#endif


#endif /* _NGX_CYCLE_H_INCLUDED_ */
