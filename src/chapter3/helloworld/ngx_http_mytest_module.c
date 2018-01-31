#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static char *
ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r);



static ngx_command_t  ngx_http_mytest_commands[] =
{

    {
        ngx_string("mytest"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_HTTP_LMT_CONF | NGX_CONF_NOARGS,
        ngx_http_mytest,  // 滿足配置後，調用的函數
        NGX_HTTP_LOC_CONF_OFFSET,  // 解析配置
        0,
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t  ngx_http_mytest_module_ctx =
{
    NULL,                               /* preconfiguration */
    NULL,                  		        /* postconfiguration */

    NULL,                               /* create main configuration */
    NULL,                               /* init main configuration */

    NULL,                               /* create server configuration */
    NULL,                               /* merge server configuration */

    NULL,       			            /* create location configuration */
    NULL         			            /* merge location configuration */
};

ngx_module_t  ngx_http_mytest_module =
{
    NGX_MODULE_V1,
    &ngx_http_mytest_module_ctx,           /* module context */
    ngx_http_mytest_commands,              /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static char *
ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;

    // 首先找到mytest配置顯所屬的配置塊，clcf看上去像是location塊內的數據結構，其實不然，它可以是main，
    // srv或者loc級別配置顯，也就是說，在每個http和server內也都有一個ngx_http_core_loc_conf_t結構體
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    // HTTP框架在處理用戶請求進行到NGX_HTTP_CONTENT_PHASE階段時，如果請求的主機域名，uri於mytest配置顯所在的配置塊
    // 想匹配，就將調用我們實現的ngx_http_mytest_handler方法處理這個請求
    clcf->handler = ngx_http_mytest_handler;

    return NGX_CONF_OK;
}


static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)
{
    //必須是GET或者HEAD方法，否則返回405 Not Allowed
    if (!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD)))
    {
        return NGX_HTTP_NOT_ALLOWED;
    }

    //丟棄請求中的包體
    ngx_int_t rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK)
    {
        return rc;
    }

    //設置返回的Conytent-Type。注意，ngx_str_t有一個很方便的初始化宏ngx_string，它可以把ngx_str_t的data和len成員都設置好
    ngx_str_t type = ngx_string("text/plain");
    //返回包體的內容
    ngx_str_t response = ngx_string("Hello World!");
    //設置返回的狀態碼
    r->headers_out.status = NGX_HTTP_OK;
    //響應包是有包體內容的，需要設置Content-Length長度
    r->headers_out.content_length_n = response.len;
    //設置Content-Type
    r->headers_out.content_type = type;

    //發送HTTP頭部
    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only)
    {
        return rc;
    }

    //構造ngx_buf_t結構體準備發送包體
    ngx_buf_t                 *b;
    b = ngx_create_temp_buf(r->pool, response.len);
    if (b == NULL)
    {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    //將hello world複製到ngx_buf_t指向的內存中
    ngx_memcpy(b->pos, response.data, response.len);
    //注意，一定要設置好last指針
    b->last = b->pos + response.len;
    //聲明最好一塊緩衝區
    b->last_buf = 1;

    //構造發送時的ngx_chain_t結構體
    ngx_chain_t		out;
    //賦值ngx_buf_t
    out.buf = b;
    //設置next位NULL
    out.next = NULL;

    //最後一步發送包體，發送結束後HTTP框架會調用ngx_http_fionalize_request方法結束請求
    return ngx_http_output_filter(r, &out);
}


