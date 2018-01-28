
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_LIST_H_INCLUDED_
#define _NGX_LIST_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct ngx_list_part_s  ngx_list_part_t;

struct ngx_list_part_s {
    void             *elts;  // 指向數組的起始地址
    ngx_uint_t        nelts; // 表示數組中已經使用兩多少元素。當然，nelts必須小於ngx_list_t結構體中的nalloc。
    ngx_list_part_t  *next; // 下一個鏈表元素ngx_list_part_t的地址
};


typedef struct {
    ngx_list_part_t  *last; // 指向鏈表的最後一個數組元素
    ngx_list_part_t   part; // 鏈表的首個數組元素
    size_t            size; // 用戶要存儲的一個數據所佔用的字節數必須小於或等於size
    ngx_uint_t        nalloc; // 鏈表的數組元素一旦分配後是不可更改的。nalloc表示每個ngx_list_part_t數組容量，即做多可存儲多少個數據
    ngx_pool_t       *pool; // 鏈表中管理內存分配的內存池對象。用戶要存放的數據佔用的內存都是由pool分配的。
} ngx_list_t;


ngx_list_t *ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size);

static ngx_inline ngx_int_t
ngx_list_init(ngx_list_t *list, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    list->part.elts = ngx_palloc(pool, n * size);
    if (list->part.elts == NULL) {
        return NGX_ERROR;
    }

    list->part.nelts = 0;
    list->part.next = NULL;
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;

    return NGX_OK;
}


/*
 *
 *  the iteration through the list:
 *
 *  part = &list.part;
 *  data = part->elts;
 *
 *  for (i = 0 ;; i++) {
 *
 *      if (i >= part->nelts) {
 *          if (part->next == NULL) {
 *              break;
 *          }
 *
 *          part = part->next;
 *          data = part->elts;
 *          i = 0;
 *      }
 *
 *      ...  data[i] ...
 *
 *  }
 */


void *ngx_list_push(ngx_list_t *list);


#endif /* _NGX_LIST_H_INCLUDED_ */
