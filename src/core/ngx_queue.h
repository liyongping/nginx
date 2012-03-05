
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


#ifndef _NGX_QUEUE_H_INCLUDED_
#define _NGX_QUEUE_H_INCLUDED_


typedef struct ngx_queue_s  ngx_queue_t;

struct ngx_queue_s {
    ngx_queue_t  *prev;     // 指向前一个节点
    ngx_queue_t  *next;     // 指向下一个节点
};

// 初始化或构造一个queue
#define ngx_queue_init(q)                                                     \
    (q)->prev = q;                                                            \
    (q)->next = q

// 判断队列是否为null
#define ngx_queue_empty(h)                                                    \
    (h == (h)->prev)

// 在节点x前面插入head：h
#define ngx_queue_insert_head(h, x)                                           \
    (x)->next = (h)->next;                                                    \
    (x)->next->prev = x;                                                      \
    (x)->prev = h;                                                            \
    (h)->next = x


#define ngx_queue_insert_after   ngx_queue_insert_head

// 在节点x后面插入h
#define ngx_queue_insert_tail(h, x)                                           \
    (x)->prev = (h)->prev;                                                    \
    (x)->prev->next = x;                                                      \
    (x)->next = h;                                                            \
    (h)->prev = x


#define ngx_queue_head(h)                                                     \
    (h)->next


#define ngx_queue_last(h)                                                     \
    (h)->prev


#define ngx_queue_sentinel(h)                                                 \
    (h)


#define ngx_queue_next(q)                                                     \
    (q)->next


#define ngx_queue_prev(q)                                                     \
    (q)->prev


#if (NGX_DEBUG)

#define ngx_queue_remove(x)                                                   \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next;                                              \
    (x)->prev = NULL;                                                         \
    (x)->next = NULL

#else
// 移除节点x
#define ngx_queue_remove(x)                                                   \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next

#endif

/*
描述：（n为新加入的节点，q为原来h为头部的队列中的一个节点）
   在h为队列Q中，以q为界限把Q分割为两个队列QL（队列左）,QR（队列右），同事把n插入到QR，并且作为QR的head节点，
   q作为n的下一个节点
*/
#define ngx_queue_split(h, q, n)                                              \
    (n)->prev = (h)->prev;                                                    \
    (n)->prev->next = n;                                                      \
    (n)->next = q;                                                            \
    (h)->prev = (q)->prev;                                                    \
    (h)->prev->next = h;                                                      \
    (q)->prev = n;

// 把以n为头结点的队列N，连接到以h为头结点的队列H中，同时删除n节点...好像头部节点没有包含实际数据，所以删除
#define ngx_queue_add(h, n)                                                   \
    (h)->prev->next = (n)->next;                                              \
    (n)->next->prev = (h)->prev;                                              \
    (h)->prev = (n)->prev;                                                    \
    (h)->prev->next = h;

/*
ngx_queue_data: 获取节点数据
注意：
    nginx的队列操作只对链表指针进行简单的修改指向操作，并不负责节点数据空间的分配。因此，用户在使用nginx队列
    时，要自己定义数据结构并分配空间，且在其中包含一个ngx_queue_t的指针或者对象，如：
    typedef struct
    {
        int point;
        ngx_queue_t queue;
    } my_queue_t;
*/
#define ngx_queue_data(q, type, link)                                         \
    (type *) ((u_char *) q - offsetof(type, link))

// 获取队列的中间节点
ngx_queue_t *ngx_queue_middle(ngx_queue_t *queue);
// 排序-插入排序
void ngx_queue_sort(ngx_queue_t *queue,
    ngx_int_t (*cmp)(const ngx_queue_t *, const ngx_queue_t *));


#endif /* _NGX_QUEUE_H_INCLUDED_ */
