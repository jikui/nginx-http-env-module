
/*
 * Copyright (C) Jikui Pei
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static ngx_int_t ngx_http_env_add_var(ngx_conf_t *cf);
static ngx_int_t
ngx_http_variable_env(ngx_http_request_t *r, ngx_http_variable_value_t *v,
                      uintptr_t data);

static ngx_http_variable_t  ngx_http_env_var[] = {

    {
        ngx_string("env_"), NULL, ngx_http_variable_env,
        0, NGX_HTTP_VAR_NOCACHEABLE | NGX_HTTP_VAR_PREFIX, 0
    },

    ngx_http_null_variable
};



static ngx_http_module_t  ngx_http_env_module_ctx = {
    ngx_http_env_add_var,          /* preconfiguration */
    NULL,                          /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,       /* create location configuration */
    NULL         /* merge location configuration */
};


ngx_module_t ngx_http_env_module = {
    NGX_MODULE_V1,
    &ngx_http_env_module_ctx,           /* module context */
    NULL,                               /* module directives */
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

ngx_int_t
ngx_http_env_add_var(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var, *v;

    for (v = ngx_http_env_var; v->name.len; v++) {
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == NULL) {
            return NGX_ERROR;
        }

        var->get_handler = v->get_handler;
        var->data = v->data;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_env(ngx_http_request_t *r, u_char *name, size_t len, ngx_str_t *value)
{
    char      **p;

    if (environ == NULL || *environ == NULL) {
        return NGX_DECLINED;
    }

    for (p = environ; *p; p ++) {

        if (ngx_strncasecmp((u_char *)*p, name, len) == 0
            && (*p)[len] == '=') {
            /*find the variable*/
            value->data = (u_char *)(*p + len + 1);
            value->len = ngx_strlen(*p) - len - 1;
            return NGX_OK;
        }
    }
    return NGX_DECLINED;
}

static ngx_int_t
ngx_http_variable_env(ngx_http_request_t *r, ngx_http_variable_value_t *v,
                      uintptr_t data)
{
    ngx_str_t *name = (ngx_str_t *) data;

    u_char     *arg;
    size_t      len;
    ngx_str_t   value;

    len = name->len - (sizeof("env_") - 1);
    arg = name->data + sizeof("env_") - 1;

    if (len == 0 || ngx_http_env(r, arg, len, &value) != NGX_OK) {
        v->not_found = 1;
        return NGX_OK;
    }

    v->data = value.data;
    v->len = value.len;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}
