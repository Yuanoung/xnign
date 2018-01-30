#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>



static void*
ngx_http_mytest_create_loc_conf(ngx_conf_t *cf);

static char*
ngx_conf_set_myconfig(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static char *
ngx_http_mytest_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

ngx_int_t
ngx_http_mytest_post_conf(ngx_conf_t *cf);


typedef struct
{
    ngx_str_t   	my_str;
    ngx_int_t   	my_num;
    ngx_flag_t   	my_flag;
    size_t		    my_size;
    ngx_array_t*  	my_str_array;
    ngx_array_t*  	my_keyval;
    off_t   	    my_off;
    ngx_msec_t   	my_msec;
    time_t   	    my_sec;
    ngx_bufs_t   	my_bufs;
    ngx_uint_t   	my_enum_seq;
    ngx_uint_t	    my_bitmask;
    ngx_uint_t   	my_access;
    ngx_path_t*	    my_path;

    ngx_str_t		my_config_str;
    ngx_int_t		my_config_num;
} ngx_http_mytest_conf_t;

static ngx_conf_enum_t  test_enums[] =
{
    { ngx_string("apple"), 1 },
    { ngx_string("banana"), 2 },
    { ngx_string("orange"), 3 },
    { ngx_null_string, 0 }
};

static ngx_conf_bitmask_t  test_bitmasks[] =
{
    { ngx_string("good"), 0x0002 },
    { ngx_string("better"), 0x0004 },
    { ngx_string("best"), 0x0008 },
    { ngx_null_string, 0 }
};


static ngx_command_t  ngx_http_mytest_commands[] =
{

    {
        ngx_string("test_flag"),
        NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_flag),
        NULL
    },

    {
        ngx_string("test_str"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_str),
        NULL
    },

    {
        ngx_string("test_str_array"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_str_array_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_str_array),
        NULL
    },

    {
        ngx_string("test_keyval"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE2,
        ngx_conf_set_keyval_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_keyval),
        NULL
    },

    {
        ngx_string("test_num"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_num_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_num),
        NULL
    },

    {
        ngx_string("test_size"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_size_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_size),
        NULL
    },

    {
        ngx_string("test_off"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_off_slot, NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_off),
        NULL
    },

    {
        ngx_string("test_msec"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_msec_slot, NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_msec),
        NULL
    },

    {
        ngx_string("test_sec"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_sec_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_sec),
        NULL
    },

    {
        ngx_string("test_bufs"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE2,
        ngx_conf_set_bufs_slot, NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_bufs),
        NULL
    },

    {
        ngx_string("test_enum"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_enum_slot, NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_enum_seq),
        test_enums
    },

    {
        ngx_string("test_bitmask"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_bitmask_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_bitmask),
        test_bitmasks
    },

    {
        ngx_string("test_access"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE123,
        ngx_conf_set_access_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_access),
        NULL
    },

    {
        ngx_string("test_path"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1234,
        ngx_conf_set_path_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_path),
        NULL
    },

    {
        ngx_string("test_myconfig"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE12,
        ngx_conf_set_myconfig,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },


    ngx_null_command
};

static ngx_http_module_t  ngx_http_mytest_module_ctx =
{
    NULL,                               /* preconfiguration */
    ngx_http_mytest_post_conf,          /* postconfiguration */

    NULL,                               /* create main configuration */
    NULL,                               /* init main configuration */

    NULL,                               /* create server configuration */
    NULL,                               /* merge server configuration */

    ngx_http_mytest_create_loc_conf,    /* create location configuration */
    ngx_http_mytest_merge_loc_conf      /* merge location configuration */
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
ngx_http_mytest_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_mytest_conf_t *prev = (ngx_http_mytest_conf_t *)parent;
    ngx_http_mytest_conf_t *conf = (ngx_http_mytest_conf_t *)child;
    ngx_conf_merge_str_value(conf->my_str,
                             prev->my_str, "defaultstr");
    ngx_conf_merge_value(conf->my_flag, prev->my_flag, 0);


    return NGX_CONF_OK;
}


static char* ngx_conf_set_myconfig(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    //ע�⣬����conf����http��ܴ������ǵģ���ngx_http_mytest_create_loc_conf
    //�ص������з���Ľṹ��ngx_http_mytest_conf_t
    ngx_http_mytest_conf_t  *mycf = conf;

    // cf->args��1��ngx_array_t���У����ĳ�Ա����ngx_str_t�ṹ��
    //������valueָ��ngx_array_t��elts���ݣ�����value[1]���ǵ�1
    //��������ͬ��value[2]�ǵ�2������
    ngx_str_t* value = cf->args->elts;

    //ngx_array_t��nelts��ʾ�����ĸ���
    if (cf->args->nelts > 1)
    {
        //ֱ�Ӹ�ֵ���ɣ� ngx_str_t�ṹֻ��ָ��Ĵ���
        mycf->my_config_str = value[1];
    }
    if (cf->args->nelts > 2)
    {
        //���ַ�����ʽ�ĵ�2������תΪ����
        mycf->my_config_num = ngx_atoi(value[2].data, value[2].len);
        //����ַ���ת������ʧ�ܣ�����"invalid number"����
    //nginx��ʧ��
        if (mycf->my_config_num == NGX_ERROR)
        {
            return "invalid number";
        }
    }

    //���سɹ�
    return NGX_CONF_OK;
}



static void* ngx_http_mytest_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_mytest_conf_t  *mycf;

    mycf = (ngx_http_mytest_conf_t  *)ngx_pcalloc(cf->pool, sizeof(ngx_http_mytest_conf_t));
    if (mycf == NULL)
    {
        return NULL;
    }

    mycf->my_flag = NGX_CONF_UNSET;
    mycf->my_num = NGX_CONF_UNSET;
    mycf->my_str_array = NGX_CONF_UNSET_PTR;
    mycf->my_keyval = NULL;
    mycf->my_off = NGX_CONF_UNSET;
    mycf->my_msec = NGX_CONF_UNSET_MSEC;
    mycf->my_sec = NGX_CONF_UNSET;
    mycf->my_size = NGX_CONF_UNSET_SIZE;

    return mycf;
}

//for test
extern ngx_module_t  ngx_http_module;
extern ngx_module_t  ngx_http_core_module;

//�����ڱ���location�ڵ�test_str�ֶ�
void traversal(ngx_conf_t *cf, ngx_http_location_tree_node_t* node)
{
    if (node != NULL)
    {
        traversal(cf, node->left);
        ngx_http_core_loc_conf_t* loc = NULL;
        if (node->exact != NULL)
        {
            loc = (ngx_http_core_loc_conf_t*)node->exact;
        }
        else if (node->inclusive != NULL)
        {
            loc = (ngx_http_core_loc_conf_t*)node->inclusive;
        }

        if (loc != NULL)
        {
            ngx_http_mytest_conf_t  *mycf = (ngx_http_mytest_conf_t  *)loc->loc_conf[ngx_http_mytest_module.ctx_index];
            ngx_log_error(NGX_LOG_ALERT, cf->log, 0, "in location[name=%V]{} test_str=%V",
                          &loc->name, &mycf->my_str);
        }
        else
        {
            ngx_log_error(NGX_LOG_ALERT, cf->log, 0, "wrong location tree");
        }

        traversal(cf, node->right);
    }
}

//������test_strΪ������Ļ�������ȡ�������в�ͬ���ֵ
ngx_int_t
ngx_http_mytest_post_conf(ngx_conf_t *cf)
{
    ngx_uint_t i = 0;
    ngx_http_conf_ctx_t* http_root_conf = (ngx_http_conf_ctx_t*)ngx_get_conf(cf->cycle->conf_ctx, ngx_http_module);

    ngx_http_mytest_conf_t  *mycf;
    mycf = (ngx_http_mytest_conf_t  *)http_root_conf->loc_conf[ngx_http_mytest_module.ctx_index];
    ngx_log_error(NGX_LOG_ALERT, cf->log, 0, "in http{} test_str=%V", &mycf->my_str);

    ngx_http_core_main_conf_t* core_main_conf = (ngx_http_core_main_conf_t*)
                                                http_root_conf->main_conf[ngx_http_core_module.ctx_index];

    for (i = 0; i < core_main_conf->servers.nelts; i++)
    {
        ngx_http_core_srv_conf_t* tmpcoresrv = *((ngx_http_core_srv_conf_t**)
                                                 (core_main_conf->servers.elts) + i);
        mycf = (ngx_http_mytest_conf_t  *)tmpcoresrv->ctx->loc_conf[ngx_http_mytest_module.ctx_index];
        ngx_log_error(NGX_LOG_ALERT, cf->log, 0, "in server[name=%V]{} test_str=%V",
                      &tmpcoresrv->server_name, &mycf->my_str);

        ngx_http_core_loc_conf_t* tmpcoreloc = (ngx_http_core_loc_conf_t*)
                                               tmpcoresrv->ctx->loc_conf[ngx_http_core_module.ctx_index];

        traversal(cf, tmpcoreloc->static_locations);
    }

    return NGX_OK;
}


