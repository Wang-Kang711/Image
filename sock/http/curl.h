struct Curl_easy {
  /* First a simple identifier to easier detect if a user mix up this easy
     handle with a multi handle. Set this to CURLEASY_MAGIC_NUMBER */
  unsigned int magic;

  /* first, two fields for the linked list of these */
  struct Curl_easy *next;
  struct Curl_easy *prev;

  struct connectdata *conn;
  struct Curl_llist_element connect_queue;
  struct Curl_llist_element conn_queue; /* list per connectdata */

  CURLMstate mstate;  /* the handle's state */
  CURLcode result;   /* previous result */

  struct Curl_message msg; /* A single posted message. */

  /* Array with the plain socket numbers this handle takes care of, in no
     particular order. Note that all sockets are added to the sockhash, where
     the state etc are also kept. This array is mostly used to detect when a
     socket is to be removed from the hash. See singlesocket(). */
  curl_socket_t sockets[MAX_SOCKSPEREASYHANDLE];
  unsigned char actions[MAX_SOCKSPEREASYHANDLE]; /* action for each socket in
                                                    sockets[] */
  int numsocks;

  struct Names dns;
  struct Curl_multi *multi;    /* if non-NULL, points to the multi handle
                                  struct to which this "belongs" when used by
                                  the multi interface */
  struct Curl_multi *multi_easy; /* if non-NULL, points to the multi handle
                                    struct to which this "belongs" when used
                                    by the easy interface */
  struct Curl_share *share;    /* Share, handles global variable mutexing */
#ifdef USE_LIBPSL
  struct PslCache *psl;        /* The associated PSL cache. */
#endif
  struct SingleRequest req;    /* Request-specific data */
  struct UserDefined set;      /* values set by the libcurl user */
#ifndef CURL_DISABLE_COOKIES
  struct CookieInfo *cookies;  /* the cookies, read from files and servers.
                                  NOTE that the 'cookie' field in the
                                  UserDefined struct defines if the "engine"
                                  is to be used or not. */
#endif
#ifndef CURL_DISABLE_HSTS
  struct hsts *hsts;
#endif
#ifndef CURL_DISABLE_ALTSVC
  struct altsvcinfo *asi;      /* the alt-svc cache */
#endif
  struct Progress progress;    /* for all the progress meter data */
  struct UrlState state;       /* struct for fields used for state info and
                                  other dynamic purposes */
#ifndef CURL_DISABLE_FTP
  struct WildcardData wildcard; /* wildcard download state info */
#endif
  struct PureInfo info;        /* stats, reports and info data */
  struct curl_tlssessioninfo tsi; /* Information about the TLS session, only
                                     valid after a client has asked for it */
#ifdef USE_HYPER
  struct hyptransfer hyp;
#endif
};


/*
 * HTTP连接封装信息
 * The connectdata struct contains all fields and variables that should be
 * unique for an entire connection.
 */
struct connectdata {
  struct connstate cnnct;
  struct Curl_llist_element bundle_node; /* conncache */

  /* chunk is for HTTP chunked encoding, but is in the general connectdata
     struct only because we can do just about any protocol through a HTTP proxy
     and a HTTP proxy may in fact respond using chunked encoding */
  struct Curl_chunker chunk;

  curl_closesocket_callback fclosesocket; /* function closing the socket(s) */
  void *closesocket_client;

  /* This is used by the connection cache logic. If this returns TRUE, this
     handle is still used by one or more easy handles and can only used by any
     other easy handle without careful consideration (== only for
     multiplexing) and it cannot be used by another multi handle! */
#define CONN_INUSE(c) ((c)->easyq.size)

  /**** Fields set when inited and not modified again */
  long connection_id; /* Contains a unique number to make it easier to
                         track the connections in the log output */

  /* 'dns_entry' is the particular host we use. This points to an entry in the
     DNS cache and it will not get pruned while locked. It gets unlocked in
     multi_done(). This entry will be NULL if the connection is re-used as then
     there is no name resolve done. */
  struct Curl_dns_entry *dns_entry;

  /* 'ip_addr' is the particular IP we connected to. It points to a struct
     within the DNS cache, so this pointer is only valid as long as the DNS
     cache entry remains locked. It gets unlocked in multi_done() */
  struct Curl_addrinfo *ip_addr;
  struct Curl_addrinfo *tempaddr[2]; /* for happy eyeballs */

#ifdef ENABLE_QUIC
  struct quicsocket hequic[2]; /* two, for happy eyeballs! */
  struct quicsocket *quic;
#endif

  struct hostname host;
  char *hostname_resolve; /* host name to resolve to address, allocated */
  char *secondaryhostname; /* secondary socket host name (ftp) */
  struct hostname conn_to_host; /* the host to connect to. valid only if
                                   bits.conn_to_host is set */
#ifndef CURL_DISABLE_PROXY
  struct proxy_info socks_proxy;
  struct proxy_info http_proxy;
#endif
  /* 'primary_ip' and 'primary_port' get filled with peer's numerical
     ip address and port number whenever an outgoing connection is
     *attempted* from the primary socket to a remote address. When more
     than one address is tried for a connection these will hold data
     for the last attempt. When the connection is actually established
     these are updated with data which comes directly from the socket. */

  char primary_ip[MAX_IPADR_LEN];
  char *user;    /* user name string, allocated */
  char *passwd;  /* password string, allocated */
  char *options; /* options string, allocated */
  char *sasl_authzid;     /* authorization identity string, allocated */
  char *oauth_bearer; /* OAUTH2 bearer, allocated */
  struct curltime now;     /* "current" time */
  struct curltime created; /* creation time */
  struct curltime lastused; /* when returned to the connection cache */
  curl_socket_t sock[2]; /* two sockets, the second is used for the data
                            transfer when doing FTP */
  curl_socket_t tempsock[2]; /* temporary sockets for happy eyeballs */
  int tempfamily[2]; /* family used for the temp sockets */
  Curl_recv *recv[2];
  Curl_send *send[2];

#ifdef USE_RECV_BEFORE_SEND_WORKAROUND
  struct postponed_data postponed[2]; /* two buffers for two sockets */
#endif /* USE_RECV_BEFORE_SEND_WORKAROUND */
  struct ssl_connect_data ssl[2]; /* this is for ssl-stuff */
#ifndef CURL_DISABLE_PROXY
  struct ssl_connect_data proxy_ssl[2]; /* this is for proxy ssl-stuff */
#endif
#ifdef USE_SSL
  void *ssl_extra; /* separately allocated backend-specific data */
#endif
  struct ssl_primary_config ssl_config;
#ifndef CURL_DISABLE_PROXY
  struct ssl_primary_config proxy_ssl_config;
#endif
  struct ConnectBits bits;    /* various state-flags for this connection */

 /* connecttime: when connect() is called on the current IP address. Used to
    be able to track when to move on to try next IP - but only when the multi
    interface is used. */
  struct curltime connecttime;

  /* The field below gets set in Curl_connecthost */
  /* how long time in milliseconds to spend on trying to connect to each IP
     address, per family */
  timediff_t timeoutms_per_addr[2];

  const struct Curl_handler *handler; /* Connection's protocol handler */
  const struct Curl_handler *given;   /* The protocol first given */

  /* Protocols can use a custom keepalive mechanism to keep connections alive.
     This allows those protocols to track the last time the keepalive mechanism
     was used on this connection. */
  struct curltime keepalive;

  /**** curl_get() phase fields */

  curl_socket_t sockfd;   /* socket to read from or CURL_SOCKET_BAD */
  curl_socket_t writesockfd; /* socket to write to, it may very
                                well be the same we read from.
                                CURL_SOCKET_BAD disables */

#ifdef HAVE_GSSAPI
  BIT(sec_complete); /* if Kerberos is enabled for this connection */
  unsigned char command_prot; /* enum protection_level */
  unsigned char data_prot; /* enum protection_level */
  unsigned char request_data_prot; /* enum protection_level */
  size_t buffer_size;
  struct krb5buffer in_buffer;
  void *app_data;
  const struct Curl_sec_client_mech *mech;
  struct sockaddr_in local_addr;
#endif

#if defined(USE_KERBEROS5)    /* Consider moving some of the above GSS-API */
  struct kerberos5data krb5;  /* variables into the structure definition, */
#endif                        /* however, some of them are ftp specific. */

  struct Curl_llist easyq;    /* List of easy handles using this connection */
  curl_seek_callback seek_func; /* function that seeks the input */
  void *seek_client;            /* pointer to pass to the seek() above */

  /*************** Request - specific items ************/
#if defined(USE_WINDOWS_SSPI) && defined(SECPKG_ATTR_ENDPOINT_BINDINGS)
  CtxtHandle *sslContext;
#endif

#ifdef USE_GSASL
  struct gsasldata gsasl;
#endif

#if defined(USE_NTLM)
  curlntlm http_ntlm_state;
  curlntlm proxy_ntlm_state;

  struct ntlmdata ntlm;     /* NTLM differs from other authentication schemes
                               because it authenticates connections, not
                               single requests! */
  struct ntlmdata proxyntlm; /* NTLM data for proxy */
#endif

#ifdef USE_SPNEGO
  curlnegotiate http_negotiate_state;
  curlnegotiate proxy_negotiate_state;

  struct negotiatedata negotiate; /* state data for host Negotiate auth */
  struct negotiatedata proxyneg; /* state data for proxy Negotiate auth */
#endif

  /* for chunked-encoded trailer */
  struct dynbuf trailer;

  union {
#ifndef CURL_DISABLE_FTP
    struct ftp_conn ftpc;
#endif
#ifndef CURL_DISABLE_HTTP
    struct http_conn httpc;
#endif
#ifdef USE_SSH
    struct ssh_conn sshc;
#endif
#ifndef CURL_DISABLE_TFTP
    struct tftp_state_data *tftpc;
#endif
#ifndef CURL_DISABLE_IMAP
    struct imap_conn imapc;
#endif
#ifndef CURL_DISABLE_POP3
    struct pop3_conn pop3c;
#endif
#ifndef CURL_DISABLE_SMTP
    struct smtp_conn smtpc;
#endif
#ifndef CURL_DISABLE_RTSP
    struct rtsp_conn rtspc;
#endif
#ifndef CURL_DISABLE_SMB
    struct smb_conn smbc;
#endif
    void *rtmp;
    struct ldapconninfo *ldapc;
#ifndef CURL_DISABLE_MQTT
    struct mqtt_conn mqtt;
#endif
  } proto;

  struct http_connect_state *connect_state; /* for HTTP CONNECT */
  struct connectbundle *bundle; /* The bundle we are member of */
#ifdef USE_UNIX_SOCKETS
  char *unix_domain_socket;
#endif
#ifdef USE_HYPER
  /* if set, an alternative data transfer function */
  Curl_datastream datastream;
#endif
  /* When this connection is created, store the conditions for the local end
     bind. This is stored before the actual bind and before any connection is
     made and will serve the purpose of being used for comparison reasons so
     that subsequent bound-requested connections aren't accidentally re-using
     wrong connections. */
  char *localdev;
  int localportrange;
  int cselect_bits; /* bitmask of socket events */
  int waitfor;      /* current READ/WRITE bits to wait for */
#if defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  int socks5_gssapi_enctype;
#endif
  /* The field below gets set in Curl_connecthost */
  int num_addr; /* number of addresses to try to connect to */
  int port;        /* which port to use locally - to connect to */
  int remote_port; /* the remote port, not the proxy port! */
  int conn_to_port; /* the remote port to connect to. valid only if
                       bits.conn_to_port is set */
#ifdef ENABLE_IPV6
  unsigned int scope_id;  /* Scope id for IPv6 */
#endif
  unsigned short localport;
  unsigned short secondary_port; /* secondary socket remote port to connect to
                                    (ftp) */
  unsigned char alpn; /* APLN TLS negotiated protocol, a CURL_HTTP_VERSION*
                         value */
  unsigned char transport; /* one of the TRNSPRT_* defines */
  unsigned char ip_version; /* copied from the Curl_easy at creation time */
  unsigned char httpversion; /* the HTTP version*10 reported by the server */
  unsigned char connect_only;
};


/*
 * Struct to keep statistical and informational data.
 * All variables in this struct must be initialized/reset in Curl_initinfo().
 */
struct PureInfo {
  int httpcode;  /* Recent HTTP, FTP, RTSP or SMTP response code */
  int httpproxycode; /* response code from proxy when received separate */
  int httpversion; /* the http version number X.Y = X*10+Y */
  time_t filetime; /* If requested, this is might get set. Set to -1 if the
                      time was unretrievable. */
  curl_off_t header_size;  /* size of read header(s) in bytes */
  curl_off_t request_size; /* the amount of bytes sent in the request(s) */
  unsigned long proxyauthavail; /* what proxy auth types were announced */
  unsigned long httpauthavail;  /* what host auth types were announced */
  long numconnects; /* how many new connection did libcurl created */
  char *contenttype; /* the content type of the object */
  char *wouldredirect; /* URL this would've been redirected to if asked to */
  curl_off_t retry_after; /* info from Retry-After: header */

  /* PureInfo members 'conn_primary_ip', 'conn_primary_port', 'conn_local_ip'
     and, 'conn_local_port' are copied over from the connectdata struct in
     order to allow curl_easy_getinfo() to return this information even when
     the session handle is no longer associated with a connection, and also
     allow curl_easy_reset() to clear this information from the session handle
     without disturbing information which is still alive, and that might be
     reused, in the connection cache. */

  char conn_primary_ip[MAX_IPADR_LEN];
  int conn_primary_port; /* this is the destination port to the connection,
                            which might have been a proxy */
  int conn_remote_port;  /* this is the "remote port", which is the port
                            number of the used URL, independent of proxy or
                            not */
  char conn_local_ip[MAX_IPADR_LEN];
  int conn_local_port;
  const char *conn_scheme;
  unsigned int conn_protocol;
  struct curl_certinfo certs; /* info about the certs, only populated in
                                 OpenSSL, GnuTLS, Schannel, NSS and GSKit
                                 builds. Asked for with CURLOPT_CERTINFO
                                 / CURLINFO_CERTINFO */
  CURLproxycode pxcode;
  BIT(timecond);  /* set to TRUE if the time condition didn't match, which
                     thus made the document NOT get fetched */
};
