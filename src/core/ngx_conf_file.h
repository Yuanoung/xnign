
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HTTP_CONF_FILE_H_INCLUDED_
#define _NGX_HTTP_CONF_FILE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/*
 *        AAAA  number of agruments
 *      FF      command flags
 *    TT        command type, i.e. HTTP "location" or "server" command
 */

#define NGX_CONF_NOARGS      0x00000001
#define NGX_CONF_TAKE1       0x00000002
#define NGX_CONF_TAKE2       0x00000004
#define NGX_CONF_TAKE3       0x00000008
#define NGX_CONF_TAKE4       0x00000010
#define NGX_CONF_TAKE5       0x00000020
#define NGX_CONF_TAKE6       0x00000040
#define NGX_CONF_TAKE7       0x00000080

#define NGX_CONF_MAX_ARGS    8

#define NGX_CONF_TAKE12      (NGX_CONF_TAKE1|NGX_CONF_TAKE2)
#define NGX_CONF_TAKE13      (NGX_CONF_TAKE1|NGX_CONF_TAKE3)

#define NGX_CONF_TAKE23      (NGX_CONF_TAKE2|NGX_CONF_TAKE3)

#define NGX_CONF_TAKE123     (NGX_CONF_TAKE1|NGX_CONF_TAKE2|NGX_CONF_TAKE3)
#define NGX_CONF_TAKE1234    (NGX_CONF_TAKE1|NGX_CONF_TAKE2|NGX_CONF_TAKE3   \
                              |NGX_CONF_TAKE4)

#define NGX_CONF_ARGS_NUMBER 0x000000ff
#define NGX_CONF_BLOCK       0x00000100
#define NGX_CONF_FLAG        0x00000200
#define NGX_CONF_ANY         0x00000400
#define NGX_CONF_1MORE       0x00000800
#define NGX_CONF_2MORE       0x00001000
#define NGX_CONF_MULTI       0x00002000

#define NGX_DIRECT_CONF      0x00010000

#define NGX_MAIN_CONF        0x01000000
#define NGX_ANY_CONF         0x0F000000



#define NGX_CONF_UNSET       -1
#define NGX_CONF_UNSET_UINT  (ngx_uint_t) -1
#define NGX_CONF_UNSET_PTR   (void *) -1
#define NGX_CONF_UNSET_SIZE  (size_t) -1
#define NGX_CONF_UNSET_MSEC  (ngx_msec_t) -1


#define NGX_CONF_OK          NULL
#define NGX_CONF_ERROR       (void *) -1

#define NGX_CONF_BLOCK_START 1
#define NGX_CONF_BLOCK_DONE  2
#define NGX_CONF_FILE_DONE   3

#define NGX_CORE_MODULE      0x45524F43  /* "CORE" */
#define NGX_CONF_MODULE      0x464E4F43  /* "CONF" */


#define NGX_MAX_CONF_ERRSTR  1024


struct ngx_command_s {
    // 配置顯名稱，如“gizp”
    ngx_str_t             name;
    // 配置顯類型，type將指定配置顯可以出現的位置。例如，出現在server或者location中，以及它可以攜帶的參數個數
    ngx_uint_t            type;
    // 出現來name中指定的配置顯後，將會調用set方法處理配置顯的參數
    char               *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
    // 通常用於使用預設的解析方法解析配置顯，這是配置模塊的一個優秀設計。它需要於conf配合使用        
    ngx_uint_t            conf;
    // 在配置文件中的偏移量
    ngx_uint_t            offset;
    // 配置顯讀取後的處理方法，必須是ngx_conf_post_t結構的指針
    void                 *post;
};

#define ngx_null_command  { ngx_null_string, 0, NULL, 0, 0, NULL }  // 空的ngx_command_t


struct ngx_open_file_s {
    ngx_fd_t              fd;
    ngx_str_t             name;

    u_char               *buffer;
    u_char               *pos;
    u_char               *last;

#if 0
    /* e.g. append mode, error_log */
    ngx_uint_t            flags;
    /* e.g. reopen db file */
    ngx_uint_t          (*handler)(void *data, ngx_open_file_t *file);
    void                 *data;
#endif
};


#define NGX_MODULE_V1          0, 0, 0, 0, 0, 0, 1
#define NGX_MODULE_V1_PADDING  0, 0, 0, 0, 0, 0, 0, 0

struct ngx_module_s {
    // 下面的ctx_index，index，spare0， spare1, spare2, spare3, version變量不需要在定義時賦值，
    // 可以用Nginx準備好的宏NGX_MODULE_V1來定義，它已經定義好來這7個值。

    //      #define NGX_MODULE_V1	0,0,0,0,0,0,1

    // 對於一類模塊來（由下面的type成員決定類別）而言，ctx_index表示當前模塊在這類模塊中的序號。這個成
    // 員常常是由管理這類模塊的一個Nginx核心模塊設置的，對於所有的HTTP模塊而言，ctx_index是由核心模塊
    // ngx_http_module設置的。ctx_index非常重要，Nginx的模塊化設計非常依賴於各個模塊的順序，它們既
    // 用於表達式優先級，也用於表明每個模塊的位置，借以幫助Nginx框架快速獲得某個模塊的數據
    ngx_uint_t            ctx_index;

    // index表示當前模塊在nginx_module數組中的序號。注意，ctx_index表示當前模塊在一類模塊中的序號，而index
    // 表示當前模塊在所有模塊中的序號，它同樣重要。Nginx啓動時會根據ngx_modules數組設置個模塊的index值。例如：
	// ngx_max_module = 0;
	// for (i = 0; ngx_modules[i]; i++) {   該數組的最後一個的元素的值爲NULL
	// 	ngx_modules[i]->index = ngx_max_module++;
	// }
    ngx_uint_t            index;

    // spare系列的保留變量，暫未使用
    ngx_uint_t            spare0;
    ngx_uint_t            spare1;
    ngx_uint_t            spare2;
    ngx_uint_t            spare3;

    // 模塊的版本，便於將來的擴展。目前只有一種，默認爲1
    ngx_uint_t            version;

    // ctx用於指向一類模塊的上下文結構體，那爲什麼需要ctx呢？因爲前面說過，Nginx模塊有許多種類，不同類模塊之前的
    // 功能差別很大。例如，事件類型的模塊主要處理I/O事件相關功能，HTTP類型的模塊主要處理HTTP應用層的功能。這樣，
    // 每個模塊都有來自己的特性，兒ctx將會指向特定類型模塊的公共接口。例如，在HTTP模塊中，ctx需要指向ngx_http_module_t結構體。
    void                 *ctx;

    // commands將處理nginx.conf中的配置顯
    ngx_command_t        *commands;

    // type表示該模塊的類型，它於ctx指針是緊密相關的。在官方Nginx中，它的取值範圍有一下5中：
    // NGX_HTTP_MODULE, NGX_CORE_MUDULE, NGX_CONF_MODULE, NGX_EVENT_MODULE, NGX_MALL_MODULE。
    // 實際上，還可以自定義新的模塊類型。
    ngx_uint_t            type;

    // 在Nginx的啓動，停止過程中，一下7個函數指針表示7個執行點會分別調用7中方法。對於任一個方法而言，如果不需要Nginx在某
    // 個時刻執行它，那麼簡單地把它設爲NULL指針即可
    // 雖然從字面上理解應當在master進程啓動時回調init_master，但到目前爲止，框架代碼從來不會調用它，因此，可將init_master設爲NULL
    ngx_int_t           (*init_master)(ngx_log_t *log);

    // init_module回調方法在初始化所有模塊時被調用。在master/worker模式下，這個階段將在啓動worker子進程前完成。
    ngx_int_t           (*init_module)(ngx_cycle_t *cycle);

    // init_process回調方法在正常服務前被調用。在master/worker模式下，多個我人客人子進程已經產生，在每個worker進程
    // 的初始化過程中會調用所有模塊的init_process函數
    ngx_int_t           (*init_process)(ngx_cycle_t *cycle);

    // 由於Nginx暫時不支持多線程模式， 所有init_thread在框架中沒有被調用過，設爲NULL
    ngx_int_t           (*init_thread)(ngx_cycle_t *cycle);
    // 同上，exit_thread也不支持，設爲NULL
    void                (*exit_thread)(ngx_cycle_t *cycle);
    // exit_process回調方法在服務停止前調用。在master/worker模式下，worker進程會在退出前面調用它
    void                (*exit_process)(ngx_cycle_t *cycle);
    // exit_master回調方法將在master退出前被調用
    void                (*exit_master)(ngx_cycle_t *cycle);

    // 以下8個spare_hook變量也是保留字段，目前沒有使用，但是可用Nginx提供的NGX_MODULE_V1_PADDING宏來填充。看一下該宏的定義：
	// #define NGX_MODULE_V1_PADDING	0, 0, 0, 0, 0, 0, 0, 0
    uintptr_t             spare_hook0;
    uintptr_t             spare_hook1;
    uintptr_t             spare_hook2;
    uintptr_t             spare_hook3;
    uintptr_t             spare_hook4;
    uintptr_t             spare_hook5;
    uintptr_t             spare_hook6;
    uintptr_t             spare_hook7;
};


typedef struct {
    ngx_str_t             name;
    void               *(*create_conf)(ngx_cycle_t *cycle);
    char               *(*init_conf)(ngx_cycle_t *cycle, void *conf);
} ngx_core_module_t;


typedef struct {
    ngx_file_t            file;
    ngx_buf_t            *buffer;
    ngx_uint_t            line;
} ngx_conf_file_t;


typedef char *(*ngx_conf_handler_pt)(ngx_conf_t *cf,
    ngx_command_t *dummy, void *conf);


struct ngx_conf_s {
    char                 *name;
    ngx_array_t          *args;

    ngx_cycle_t          *cycle;
    ngx_pool_t           *pool;
    ngx_pool_t           *temp_pool;
    ngx_conf_file_t      *conf_file;
    ngx_log_t            *log;

    void                 *ctx;
    ngx_uint_t            module_type;
    ngx_uint_t            cmd_type;

    ngx_conf_handler_pt   handler;
    char                 *handler_conf;
};


typedef char *(*ngx_conf_post_handler_pt) (ngx_conf_t *cf,
    void *data, void *conf);

typedef struct {
    ngx_conf_post_handler_pt  post_handler;
} ngx_conf_post_t;


typedef struct {
    ngx_conf_post_handler_pt  post_handler;
    char                     *old_name;
    char                     *new_name;
} ngx_conf_deprecated_t;


typedef struct {
    ngx_conf_post_handler_pt  post_handler;
    ngx_int_t                 low;
    ngx_int_t                 high;
} ngx_conf_num_bounds_t;


typedef struct {
    ngx_str_t                 name;
    ngx_uint_t                value;
} ngx_conf_enum_t;


#define NGX_CONF_BITMASK_SET  1

typedef struct {
    ngx_str_t                 name;
    ngx_uint_t                mask;
} ngx_conf_bitmask_t;



char * ngx_conf_deprecated(ngx_conf_t *cf, void *post, void *data);
char *ngx_conf_check_num_bounds(ngx_conf_t *cf, void *post, void *data);


#define ngx_get_conf(conf_ctx, module)  conf_ctx[module.index]



#define ngx_conf_init_value(conf, default)                                   \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = default;                                                      \
    }

#define ngx_conf_init_ptr_value(conf, default)                               \
    if (conf == NGX_CONF_UNSET_PTR) {                                        \
        conf = default;                                                      \
    }

#define ngx_conf_init_uint_value(conf, default)                              \
    if (conf == NGX_CONF_UNSET_UINT) {                                       \
        conf = default;                                                      \
    }

#define ngx_conf_init_size_value(conf, default)                              \
    if (conf == NGX_CONF_UNSET_SIZE) {                                       \
        conf = default;                                                      \
    }

#define ngx_conf_init_msec_value(conf, default)                              \
    if (conf == NGX_CONF_UNSET_MSEC) {                                       \
        conf = default;                                                      \
    }

#define ngx_conf_merge_value(conf, prev, default)                            \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = (prev == NGX_CONF_UNSET) ? default : prev;                    \
    }

#define ngx_conf_merge_ptr_value(conf, prev, default)                        \
    if (conf == NGX_CONF_UNSET_PTR) {                                        \
        conf = (prev == NGX_CONF_UNSET_PTR) ? default : prev;                \
    }

#define ngx_conf_merge_uint_value(conf, prev, default)                       \
    if (conf == NGX_CONF_UNSET_UINT) {                                       \
        conf = (prev == NGX_CONF_UNSET_UINT) ? default : prev;               \
    }

#define ngx_conf_merge_msec_value(conf, prev, default)                       \
    if (conf == NGX_CONF_UNSET_MSEC) {                                       \
        conf = (prev == NGX_CONF_UNSET_MSEC) ? default : prev;               \
    }

#define ngx_conf_merge_sec_value(conf, prev, default)                        \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = (prev == NGX_CONF_UNSET) ? default : prev;                    \
    }

#define ngx_conf_merge_size_value(conf, prev, default)                       \
    if (conf == NGX_CONF_UNSET_SIZE) {                                       \
        conf = (prev == NGX_CONF_UNSET_SIZE) ? default : prev;               \
    }

#define ngx_conf_merge_off_value(conf, prev, default)                        \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = (prev == NGX_CONF_UNSET) ? default : prev;                    \
    }

#define ngx_conf_merge_str_value(conf, prev, default)                        \
    if (conf.data == NULL) {                                                 \
        if (prev.data) {                                                     \
            conf.len = prev.len;                                             \
            conf.data = prev.data;                                           \
        } else {                                                             \
            conf.len = sizeof(default) - 1;                                  \
            conf.data = (u_char *) default;                                  \
        }                                                                    \
    }

#define ngx_conf_merge_bufs_value(conf, prev, default_num, default_size)     \
    if (conf.num == 0) {                                                     \
        if (prev.num) {                                                      \
            conf.num = prev.num;                                             \
            conf.size = prev.size;                                           \
        } else {                                                             \
            conf.num = default_num;                                          \
            conf.size = default_size;                                        \
        }                                                                    \
    }

#define ngx_conf_merge_bitmask_value(conf, prev, default)                    \
    if (conf == 0) {                                                         \
        conf = (prev == 0) ? default : prev;                                 \
    }


char *ngx_conf_param(ngx_conf_t *cf);
char *ngx_conf_parse(ngx_conf_t *cf, ngx_str_t *filename);


ngx_int_t ngx_conf_full_name(ngx_cycle_t *cycle, ngx_str_t *name,
    ngx_uint_t conf_prefix);
ngx_open_file_t *ngx_conf_open_file(ngx_cycle_t *cycle, ngx_str_t *name);
void ngx_cdecl ngx_conf_log_error(ngx_uint_t level, ngx_conf_t *cf,
    ngx_err_t err, const char *fmt, ...);


char *ngx_conf_set_flag_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_str_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_str_array_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
char *ngx_conf_set_keyval_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_num_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_size_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_off_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_msec_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_sec_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_bufs_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_enum_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_bitmask_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);


extern ngx_uint_t     ngx_max_module;
extern ngx_module_t  *ngx_modules[];


#endif /* _NGX_HTTP_CONF_FILE_H_INCLUDED_ */
