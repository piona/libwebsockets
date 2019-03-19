/*
 * libwebsockets - small server side websockets and web server implementation
 *
 * Copyright (C) 2010 - 2019 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 *
 *  This is included from core/private.h if LWS_WITH_TLS
 */

struct lws_context_per_thread;
struct lws_tls_ops {
	int (*fake_POLLIN_for_buffered)(struct lws_context_per_thread *pt);
	int (*periodic_housekeeping)(struct lws_context *context, time_t now);
};

struct lws_context_tls {
	char alpn_discovered[32];
	const char *alpn_default;
	time_t last_cert_check_s;
	struct lws_dll cc_head;
	int count_client_contexts;
};

struct lws_pt_tls {
	struct lws_dll dll_pending_tls_head;
};

struct lws_tls_ss_pieces;

struct alpn_ctx {
	uint8_t data[23];
	uint8_t len;
};

struct lws_vhost_tls {
	lws_tls_ctx *ssl_ctx;
	lws_tls_ctx *ssl_client_ctx;
	const char *alpn;
	struct lws_tls_ss_pieces *ss; /* for acme tls certs */
	char *alloc_cert_path;
	char *key_path;
#if defined(LWS_WITH_MBEDTLS)
	lws_tls_x509 *x509_client_CA;
#endif
	char ecdh_curve[16];
	struct alpn_ctx alpn_ctx;

	int use_ssl;
	int allow_non_ssl_on_ssl_port;
	int ssl_info_event_mask;

	unsigned int user_supplied_ssl_ctx:1;
	unsigned int skipped_certs:1;
};

struct lws_lws_tls {
	lws_tls_conn *ssl;
	lws_tls_bio *client_bio;
	struct lws_dll dll_pending_tls;
	unsigned int use_ssl;
	unsigned int redirect_to_https:1;
};


LWS_EXTERN void
lws_context_init_alpn(struct lws_vhost *vhost);
LWS_EXTERN int LWS_WARN_UNUSED_RESULT
lws_ssl_capable_read(struct lws *wsi, unsigned char *buf, int len);
LWS_EXTERN int LWS_WARN_UNUSED_RESULT
lws_ssl_capable_write(struct lws *wsi, unsigned char *buf, int len);
LWS_EXTERN int LWS_WARN_UNUSED_RESULT
lws_ssl_pending(struct lws *wsi);
LWS_EXTERN int LWS_WARN_UNUSED_RESULT
lws_server_socket_service_ssl(struct lws *new_wsi, lws_sockfd_type accept_fd);
LWS_EXTERN int
lws_ssl_close(struct lws *wsi);
LWS_EXTERN void
lws_ssl_SSL_CTX_destroy(struct lws_vhost *vhost);
LWS_EXTERN void
lws_ssl_context_destroy(struct lws_context *context);
void
__lws_ssl_remove_wsi_from_buffered_list(struct lws *wsi);
LWS_VISIBLE void
lws_ssl_remove_wsi_from_buffered_list(struct lws *wsi);
LWS_EXTERN int
lws_ssl_client_bio_create(struct lws *wsi);
LWS_EXTERN int
lws_ssl_client_connect1(struct lws *wsi);
LWS_EXTERN int
lws_ssl_client_connect2(struct lws *wsi, char *errbuf, int len);
LWS_EXTERN int
lws_tls_fake_POLLIN_for_buffered(struct lws_context_per_thread *pt);
LWS_EXTERN int
lws_gate_accepts(struct lws_context *context, int on);
LWS_EXTERN void
lws_ssl_bind_passphrase(lws_tls_ctx *ssl_ctx,
			const struct lws_context_creation_info *info);
LWS_EXTERN void
lws_ssl_info_callback(const lws_tls_conn *ssl, int where, int ret);
LWS_EXTERN int
lws_tls_server_certs_load(struct lws_vhost *vhost, struct lws *wsi,
			  const char *cert, const char *private_key,
			  const char *mem_cert, size_t len_mem_cert,
			  const char *mem_privkey, size_t mem_privkey_len);
LWS_EXTERN enum lws_tls_extant
lws_tls_generic_cert_checks(struct lws_vhost *vhost, const char *cert,
			    const char *private_key);
#if !defined(LWS_NO_SERVER)
 LWS_EXTERN int
 lws_context_init_server_ssl(const struct lws_context_creation_info *info,
			     struct lws_vhost *vhost);
 void
 lws_tls_acme_sni_cert_destroy(struct lws_vhost *vhost);
#else
 #define lws_context_init_server_ssl(_a, _b) (0)
 #define lws_tls_acme_sni_cert_destroy(_a)
#endif

LWS_EXTERN void
lws_ssl_destroy(struct lws_vhost *vhost);

/*
* lws_tls_ abstract backend implementations
*/

LWS_EXTERN int
lws_tls_server_client_cert_verify_config(struct lws_vhost *vh);
LWS_EXTERN int
lws_tls_server_vhost_backend_init(const struct lws_context_creation_info *info,
			  struct lws_vhost *vhost, struct lws *wsi);
LWS_EXTERN int
lws_tls_server_new_nonblocking(struct lws *wsi, lws_sockfd_type accept_fd);

LWS_EXTERN enum lws_ssl_capable_status
lws_tls_server_accept(struct lws *wsi);

LWS_EXTERN enum lws_ssl_capable_status
lws_tls_server_abort_connection(struct lws *wsi);

LWS_EXTERN enum lws_ssl_capable_status
__lws_tls_shutdown(struct lws *wsi);

LWS_EXTERN enum lws_ssl_capable_status
lws_tls_client_connect(struct lws *wsi);
LWS_EXTERN int
lws_tls_client_confirm_peer_cert(struct lws *wsi, char *ebuf, int ebuf_len);
LWS_EXTERN int
lws_tls_client_create_vhost_context(struct lws_vhost *vh,
			    const struct lws_context_creation_info *info,
			    const char *cipher_list,
			    const char *ca_filepath,
			    const void *ca_mem,
			    unsigned int ca_mem_len,
			    const char *cert_filepath,
			    const void *cert_mem,
			    unsigned int cert_mem_len,
			    const char *private_key_filepath);

LWS_EXTERN lws_tls_ctx *
lws_tls_ctx_from_wsi(struct lws *wsi);
LWS_EXTERN int
lws_ssl_get_error(struct lws *wsi, int n);

LWS_EXTERN int
lws_context_init_client_ssl(const struct lws_context_creation_info *info,
		    struct lws_vhost *vhost);

LWS_EXTERN void
lws_ssl_info_callback(const lws_tls_conn *ssl, int where, int ret);

int
lws_tls_fake_POLLIN_for_buffered(struct lws_context_per_thread *pt);





