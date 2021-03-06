
/*
 * Copyright (C) Igor Sysoev
 */


#ifndef _NGX_CONNECTION_H_INCLUDED_
#define _NGX_CONNECTION_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>

//typedef unsigned short int sa_family_t;
//#define	__SOCKADDR_COMMON(sa_prefix)  sa_family_t sa_prefix##family  
//struct sockaddr{
//  __SOCKADDR_COMMON (sa_);	/* Common data: address family and length.  */
//  char sa_data[14];		    /* Address data.  */
//};


///* Internet address. */
//struct in_addr {
//	__be32	s_addr;
//};

///* Structure describing an Internet (IP) socket address. */
//#define __SOCK_SIZE__	16		/* sizeof(struct sockaddr)	*/
//struct sockaddr_in {
//  sa_family_t		sin_family;	/* Address family		*/
//  __be16		    sin_port;	/* Port number			*/
//  struct in_addr	sin_addr;	/* Internet address		*/
//
//  /* Pad to size of `struct sockaddr'. */
//  unsigned char		__pad[__SOCK_SIZE__ - sizeof(short int) - sizeof(unsigned short int) - sizeof(struct in_addr)];
//};

typedef struct ngx_listening_s  ngx_listening_t;

struct ngx_listening_s {
    ngx_socket_t        fd;     //typedef int  ngx_socket_t;

    struct sockaddr    *sockaddr;
    socklen_t           socklen;    /* size of sockaddr */
    size_t              addr;       /* offset to address in sockaddr */
    size_t              addr_text_max_len;
    ngx_str_t           addr_text;      //IP:PORT 192.168.1.1:8989

    int                 family;
    int                 type;

    int                 backlog;
    int                 rcvbuf;         //接收缓冲区的长度
    int                 sndbuf;         //发送缓冲区的长度

    /* handler of accepted connection */
    ngx_connection_handler_pt   handler;        //这是一个回调函数 typedef void (*ngx_connection_handler_pt)(ngx_connection_t *c);

    void               *servers;  /* array of ngx_http_in_addr_t, for example */

    ngx_log_t           log;

    size_t              pool_size;
    /* should be here because of the AcceptEx() preread */
    size_t              post_accept_buffer_size;
    /* should be here because of the deferred accept */
    ngx_msec_t          post_accept_timeout;

    ngx_listening_t    *previous;
    ngx_connection_t   *connection;

    unsigned            open:1;
    unsigned            remain:1;
    unsigned            ignore:1;       //该监听对象被忽略

    unsigned            bound:1;       /* already bound */
    unsigned            inherited:1;   /* inherited from previous process */
    unsigned            nonblocking_accept:1;
    unsigned            listen:1;       //端口调用listen成功后，设置为1，表示该socket已进入监听状态
    unsigned            nonblocking:1;
    unsigned            shared:1;    /* shared between threads or processes */
    unsigned            addr_ntop:1;

#if (NGX_HAVE_DEFERRED_ACCEPT)
    unsigned            deferred_accept:1;
    unsigned            delete_deferred:1;
    unsigned            add_deferred:1;
#ifdef SO_ACCEPTFILTER
    char               *accept_filter;
#endif
#endif

};


typedef enum {
     NGX_ERROR_CRIT = 0,
     NGX_ERROR_ERR,
     NGX_ERROR_INFO,
     NGX_ERROR_IGNORE_ECONNRESET
} ngx_connection_log_error_e;


typedef enum {
     NGX_TCP_NODELAY_UNSET = 0,
     NGX_TCP_NODELAY_SET,
     NGX_TCP_NODELAY_DISABLED
} ngx_connection_tcp_nodelay_e;


typedef enum {
     NGX_TCP_NOPUSH_UNSET = 0,
     NGX_TCP_NOPUSH_SET,
     NGX_TCP_NOPUSH_DISABLED
} ngx_connection_tcp_nopush_e;


#define NGX_LOWLEVEL_BUFFERED  0x0f
#define NGX_SSL_BUFFERED       0x01


struct ngx_connection_s {
    void               *data;
    ngx_event_t        *read;
    ngx_event_t        *write;

    ngx_socket_t        fd;

    ngx_recv_pt         recv;
    ngx_send_pt         send;
    ngx_recv_chain_pt   recv_chain;
    ngx_send_chain_pt   send_chain;

    ngx_listening_t    *listening;

    off_t               sent;

    ngx_log_t          *log;

    ngx_pool_t         *pool;

    struct sockaddr    *sockaddr;
    socklen_t           socklen;
    ngx_str_t           addr_text;

#if (NGX_SSL)
    ngx_ssl_connection_t  *ssl;
#endif

#if (NGX_HAVE_IOCP)
    struct sockaddr    *local_sockaddr;
    socklen_t           local_socklen;
#endif

    ngx_buf_t          *buffer;

    ngx_atomic_uint_t   number;

    unsigned            buffered:8;

    unsigned            log_error:2;     /* ngx_connection_log_error_e */

    unsigned            single_connection:1;
    unsigned            unexpected_eof:1;
    unsigned            timedout:1;
    unsigned            error:1;
    unsigned            destroyed:1;

    unsigned            idle:1;
    unsigned            close:1;

    unsigned            sendfile:1;
    unsigned            sndlowat:1;
    unsigned            tcp_nodelay:2;   /* ngx_connection_tcp_nodelay_e */
    unsigned            tcp_nopush:2;    /* ngx_connection_tcp_nopush_e */

#if (NGX_HAVE_IOCP)
    unsigned            accept_context_updated:1;
#endif

#if (NGX_THREADS)
    ngx_atomic_t        lock;
#endif
};


#ifndef ngx_ssl_set_nosendshut
#define ngx_ssl_set_nosendshut(ssl)
#endif


ngx_listening_t *ngx_listening_inet_stream_socket(ngx_conf_t *cf,
    in_addr_t addr, in_port_t port);
ngx_int_t ngx_set_inherited_sockets(ngx_cycle_t *cycle);
ngx_int_t ngx_open_listening_sockets(ngx_cycle_t *cycle);
void ngx_configure_listening_socket(ngx_cycle_t *cycle);
void ngx_close_listening_sockets(ngx_cycle_t *cycle);
void ngx_close_connection(ngx_connection_t *c);
ngx_int_t ngx_connection_error(ngx_connection_t *c, ngx_err_t err, char *text);

ngx_connection_t *ngx_get_connection(ngx_socket_t s, ngx_log_t *log);
void ngx_free_connection(ngx_connection_t *c);


#endif /* _NGX_CONNECTION_H_INCLUDED_ */
