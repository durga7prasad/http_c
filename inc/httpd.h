/*
 * Author - VenkataDurgaPrasad.B
 */

#ifndef _HTTPD_H_
#define _HTTPD_H_

#include <stdio.h>

#define MAX_BUFF_LEN 	(8*1024)

extern int gsport;
extern int gsfd;
extern int gcfd;
extern unsigned char grx_data[MAX_BUFF_LEN];

#define HTTPD_BACKLOG	10

/*
 * Server name
 */

#define SERVER_NAME			"Server: VDP_HTTPD\n"

/*
 * Connection type
 */

#define	CONNECTION_ALIVE	"Connection: Keep-Alive\n"
#define CONNECTION_CLOSED	"Connection: Closed\n"

/*
 * content type
 */

#define CONTENT_TYPE_HTML	"Content-Type: text/html\n"
#define CONTENT_TYPE_PLAIN	"Content-Type: text/plain\n"
#define CONTENT_TYPE_IMAGE	"Content-Type: image/jpg\n"
#define CONTENT_TYPE_TRACE	"Content-Type: message/http\n"

/*
 * Accept ranges
 */

#define ACCEPT_RANGES_BYTES	"Accept-Ranges: bytes\n"

/*
 * Content length
 */

#define CONTENT_LENGTH		"Content-Length: "

/*
 * New line
 */

#define HTTP_NEW_LINE		"\n"

/*
 * Response status codes
 */

#define HTTP_SUCCESS		"HTTP/1.1 200 OK\n"
#define	HTTP_NOT_FOUND		"HTTP/1.1 404 Not Found\n"
#define	HTTP_SERVER_ERR		"HTTP/1.1 500 Internal Server Error\n"
#define	HTTP_NOT_IMPLEMENT	"HTTP/1.1 501 Not Implemented\n"

/* Error responses */
#define HTTP_ERR_RESPONSE	"<html>\n<head>\n\t<title>%d %s</title>\n</head>\n</html>"

/* http response codes */
typedef enum HTTP_RESP_CODE
{
	/* 1xx - information */
	HTTP_100 = 0,	/* Continue */
	HTTP_101,		/* Switching Protocols */
	/* 2xx - success */
	HTTP_200,		/* OK */
	HTTP_201,		/* Created */
	HTTP_202,		/* Accepted */
	HTTP_203,		/* Non Authoritative Information */
	HTTP_204,		/* No Content */
	HTTP_205,		/* Reset Content */
	HTTP_206,		/* Partital Content */
	/* 3xx - Redirection */
	HTTP_300,		/* Multiple Choices */
	HTTP_301,		/* Moved Permanently */
	HTTP_302,		/* Found */
	HTTP_303,		/* See Other */
	HTTP_304,		/* Not Modified */
	HTTP_305,		/* Use Proxy */
	HTTP_306,		/* Unused */
	HTTP_307,		/* Temporary Redirect */
	/* 4xx - Client error */
	HTTP_400,		/* Bad Request */
	HTTP_401,		/* Unauthorized */
	HTTP_402,		/* Payment Required */
	HTTP_403,		/* Forbidden */
	HTTP_404,		/* Not Found */
	HTTP_405,		/* Method Not Allowed */
	HTTP_406,		/* Not Acceptable */
	HTTP_407,		/* Proxy Authentication Required */
	HTTP_408,		/* Request Timeout */
	HTTP_409,		/* Conflict */
	HTTP_410,		/* Gone */
	HTTP_411,		/* Length Required */
	HTTP_412,		/* Precondition Failed */
	HTTP_413,		/* Request Entity Too Large */
	HTTP_414,		/* Request-url Too Long */
	HTTP_415,		/* Unsupported Media Type */
	HTTP_416,		/* Requested Range Not Satisfiable */
	HTTP_417,		/* Expectation Failed */
	/* 5xx - Server error */
	HTTP_500,		/* Internal Server Error */
	HTTP_501,		/* Not Implemented */
	HTTP_502,		/* Bad Gateway */
	HTTP_503,		/* Service Unavailable */
	HTTP_504,		/* Gateway Timeout */
	HTTP_505,		/* HTTP Version Not Supported */

} HTTP_RESP_CODE_T;

/* http content type */

typedef enum HTTP_CONTENT_TYPE
{
	HTTP_TEXT_HTML = 0,
	HTTP_TEXT_PLAIN,
	HTTP_IMAGE,
	HTTP_TRACE,

} HTTP_CONTENT_TYPE_T;

typedef enum BOOLEAN
{
	FALSE = 0,
	TRUE,

} BOOLEAN_T;

typedef enum STATUS
{
	FAIL = 0,
	SUCCESS,

} STATUS_T;

typedef enum HTTP_METHODS
{
	METHOD_UNSUPPORTED = 0,
	METHOD_GET,
	METHOD_POST,
	METHOD_HEAD,
	METHOD_PUT,
	METHOD_DELETE,
	METHOD_OPTIONS,
	METHOD_TRACE,

} HTTP_METHODS_T;

int httpd_init(void);

int httpd_server(void);

#endif	/* _HTTPD_H_ */
