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
#define CONNECTION_CLOSED	"Connection: closed\n"

/*
 * content type
 */

#define CONTENT_TYPE_HTML	"Content-Type: text/html\n"
#define CONTENT_TYPE_PLAIN	"Content-Type: text/plain\n"
#define CONTENT_TYPE_IMAGE	"Content-Type: image/jpg\n"

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
#define	HTTP_NOT_FOUND		"HTTP/1.1 404 NotFound\n"

/* http response codes */
typedef enum HTTP_RESP_CODE
{
	HTTP_200 = 0,
	HTTP_404,

} HTTP_RESP_CODE_T;

/* http content type */

typedef enum HTTP_CONTENT_TYPE
{
	HTTP_TEXT_HTML = 0,
	HTTP_TEXT_PLAIN,
	HTTP_IMAGE,

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


int httpd_init(void);

int httpd_server(void);

#endif	/* _HTTPD_H_ */
