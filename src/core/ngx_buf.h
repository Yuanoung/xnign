
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_BUF_H_INCLUDED_
#define _NGX_BUF_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef void *            ngx_buf_tag_t;

typedef struct ngx_buf_s  ngx_buf_t;

struct ngx_buf_s {
    // pos通常用來告訴使用者本次應該從這個位置開始處理內存中的數據，這樣設置是因爲同一個ngx_buf_t可能被多次反覆處理。
    // 當然，pos的含義是由使用它的模塊定義的
    u_char          *pos;
    // last通常表示有效的內容到此爲止，注意，post和last之間的內存是希望nginx處理的內容
    u_char          *last;
    // 處理文件時，file_pos於file_last的含義於處理內存的pos於last相同，file_pos表示將要處理的文件位置，file_last表示截止的文件位置
    off_t            file_pos;
    off_t            file_last;

    // 如果ngx_buf_t緩衝區用於內存，那麼start指向這段內存的起始地址
    u_char          *start;         /* start of buffer */
    // 於start成員對立，指向緩衝區內存的末尾
    u_char          *end;           /* end of buffer */
    // 表示當前緩衝區的類型，例如由哪個模塊使用就是指向這個模塊ngx_module_t變量的地址
    ngx_buf_tag_t    tag;
    // 引用文件
    ngx_file_t      *file;
    // 當前緩衝區的影子緩衝區，該成員很少使用，僅僅在使用緩衝區轉發上游服務器的響應時才使用來shadow成員，這是因爲Nginx太節約內存來，分配一塊內存并使用ngx_buf_t
    // 表示接收到的上游服務器響應後，在向下游客服端轉發時可能會把這塊內存存儲到文件中，也可能直接向下游發送，此時Nginx絕不會重新複製一份內存用於新的目的，而是再次
    // 建立一個ngx_buf_t結構體指向原內存，這樣多個ngx_buf_t結構體指向來統一塊內存，它們之間的關係就通過shadow成員來應用的。這種設置過於複雜，通常不建議使用
    ngx_buf_t       *shadow;


    // 臨時內存標誌位，为1時表示數據在內存中且這段內存可以修改
    unsigned         temporary:1;

    // 標誌位，爲1時表示這段數據在內存中且這段內存不可修改
    unsigned         memory:1;

    // 標誌位，爲1時表示這段內存是使用mmap系統調用映射過來的，不可以修改
    unsigned         mmap:1;

    // 標誌位，爲1時表示可回收
    unsigned         recycled:1;
    // 標誌位，爲1時表示這段緩衝區處理的是文件而不是內存
    unsigned         in_file:1;
    // 標誌位，爲1時表示需要執行flush操作
    unsigned         flush:1;
    // 標誌位，對於操作這塊緩衝區是否使用同步方式，需慎重考慮，這肯能會阻塞Nginx進程，Nginx中所有操作幾乎都是異步的，
    // 這是它支持高併發的關鍵。這些框架代碼在sync爲1時可能會阻塞的方式運行I/O操作，它的意義視使用它的Nginx模塊而定
    unsigned         sync:1;
    //標誌位，表示是否最後一塊緩衝區，因爲ngx_buf_t可以由ngx_chain_t鏈表串聯起來，因此，當last_buf爲1時，表示當前是最後一塊待處理的緩衝區
    unsigned         last_buf:1;
    // 標誌位，表示是否是ngx_chain_t中的最後一塊緩衝區
    unsigned         last_in_chain:1;
    // 標誌位，表示是否是最後一個影子緩衝區，而shadow域配合使用。通常不建議使用它。
    unsigned         last_shadow:1;
    // 標誌位，表示當前緩衝區是否屬於臨時文件
    unsigned         temp_file:1;

    /* STUB */ int   num;
};


struct ngx_chain_s {
    ngx_buf_t    *buf;
    ngx_chain_t  *next;
};


typedef struct {
    ngx_int_t    num;
    size_t       size;
} ngx_bufs_t;


typedef struct ngx_output_chain_ctx_s  ngx_output_chain_ctx_t;

typedef ngx_int_t (*ngx_output_chain_filter_pt)(void *ctx, ngx_chain_t *in);

#if (NGX_HAVE_FILE_AIO)
typedef void (*ngx_output_chain_aio_pt)(ngx_output_chain_ctx_t *ctx,
    ngx_file_t *file);
#endif

struct ngx_output_chain_ctx_s {
    ngx_buf_t                   *buf;
    ngx_chain_t                 *in;
    ngx_chain_t                 *free;
    ngx_chain_t                 *busy;

    unsigned                     sendfile:1;
    unsigned                     directio:1;
#if (NGX_HAVE_ALIGNED_DIRECTIO)
    unsigned                     unaligned:1;
#endif
    unsigned                     need_in_memory:1;
    unsigned                     need_in_temp:1;
#if (NGX_HAVE_FILE_AIO)
    unsigned                     aio:1;

    ngx_output_chain_aio_pt      aio_handler;
#endif

    off_t                        alignment;

    ngx_pool_t                  *pool;
    ngx_int_t                    allocated;
    ngx_bufs_t                   bufs;
    ngx_buf_tag_t                tag;

    ngx_output_chain_filter_pt   output_filter;
    void                        *filter_ctx;
};


typedef struct {
    ngx_chain_t                 *out;
    ngx_chain_t                **last;
    ngx_connection_t            *connection;
    ngx_pool_t                  *pool;
    off_t                        limit;
} ngx_chain_writer_ctx_t;


#define NGX_CHAIN_ERROR     (ngx_chain_t *) NGX_ERROR


#define ngx_buf_in_memory(b)        (b->temporary || b->memory || b->mmap)
#define ngx_buf_in_memory_only(b)   (ngx_buf_in_memory(b) && !b->in_file)

#define ngx_buf_special(b)                                                   \
    ((b->flush || b->last_buf || b->sync)                                    \
     && !ngx_buf_in_memory(b) && !b->in_file)

#define ngx_buf_sync_only(b)                                                 \
    (b->sync                                                                 \
     && !ngx_buf_in_memory(b) && !b->in_file && !b->flush && !b->last_buf)

#define ngx_buf_size(b)                                                      \
    (ngx_buf_in_memory(b) ? (off_t) (b->last - b->pos):                      \
                            (b->file_last - b->file_pos))

ngx_buf_t *ngx_create_temp_buf(ngx_pool_t *pool, size_t size);
ngx_chain_t *ngx_create_chain_of_bufs(ngx_pool_t *pool, ngx_bufs_t *bufs);


#define ngx_alloc_buf(pool)  ngx_palloc(pool, sizeof(ngx_buf_t))
#define ngx_calloc_buf(pool) ngx_pcalloc(pool, sizeof(ngx_buf_t))

ngx_chain_t *ngx_alloc_chain_link(ngx_pool_t *pool);
#define ngx_free_chain(pool, cl)                                             \
    cl->next = pool->chain;                                                  \
    pool->chain = cl



ngx_int_t ngx_output_chain(ngx_output_chain_ctx_t *ctx, ngx_chain_t *in);
ngx_int_t ngx_chain_writer(void *ctx, ngx_chain_t *in);

ngx_int_t ngx_chain_add_copy(ngx_pool_t *pool, ngx_chain_t **chain,
    ngx_chain_t *in);
ngx_chain_t *ngx_chain_get_free_buf(ngx_pool_t *p, ngx_chain_t **free);
void ngx_chain_update_chains(ngx_chain_t **free, ngx_chain_t **busy,
    ngx_chain_t **out, ngx_buf_tag_t tag);


#endif /* _NGX_BUF_H_INCLUDED_ */
