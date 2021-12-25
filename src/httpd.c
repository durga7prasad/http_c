/*
 * Author - VenkataDurgaPrasad.B
 */

#include "httpd.h"
#include "socket.h"
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

static void httpd_handler(int cfd);
static void handle_request(char *request, int req_len, int cfd);
static int IsSupported_Method(char *method);
static int send_response_header(int cfd, int resp, int type, int len);

int httpd_init(void)
{
	int status = 0;

	/* Create a socket */
	status = Socket_Create(&gsfd, AF_INET, SOCK_STREAM, 0);
	if (status != 0)
	{
		/* Failed to create socket */
		return status;
	}
	status = Socket_Bind(gsfd, gsport);
	if (status != 0)
	{
		/* Failed to bind socket */
		Socket_Close (gsfd);
		return status;
	}
	status = Socket_Listen(gsfd, HTTPD_BACKLOG);
	if (status != 0)
	{
		/* Listen failed */
		Socket_Close (gsfd);
		return status;
	}
	printf ("Socket created (%d) successfully\n", gsfd);
	return status;
}

int httpd_server()
{
	int status = 0;
	struct sockaddr_in caddr;
	int caddrlen;
	char *pclient = NULL;

	caddrlen = sizeof(caddr);
	/*Loop forever */
	while (1)
	{
		/* wait for client connection */
		printf ("Waiting for client to connect...\n");
		status = Socket_Accept(&gcfd, gsfd, (struct sockaddr *)&caddr, &caddrlen);
		if (status != 0)
		{
			/* accept failed */
			return status;
		}
		pclient = inet_ntoa(caddr.sin_addr);
		printf ("Client: %s\n", pclient);
		/* Handle the client requests till disconnects */
		httpd_handler(gcfd);
	}
	return status;
}

void dump_data(unsigned char *data, int len)
{
	int loop;

	printf ("\n============ DUMP START ==========\n");
	for (loop = 0; loop < len; loop++)
	{
		printf ("%c", data[loop]);
	}
	printf ("\n============ DUMP END ============\n");

	return;
}

void httpd_handler(int cfd)
{
	int status = 0;
	int readlen = 0;

	/* Loop forever */
	while (1)
	{
		printf ("waiting for client request...\n");
		readlen = recv (cfd, grx_data, MAX_BUFF_LEN, 0);
		if (readlen < 0)
		{
			/* read failed */
			perror ("recv Err:");
			break;
		}
		else if (readlen == 0)
		{
			/* Client got disconnected */
			printf ("OOps!! No data received\n");
			status = 1;
			break;
		}
		/* We receive data */
		printf ("Got %d Bytes of data\n", readlen);
		dump_data (grx_data, readlen);
		/* Handle http request */
		handle_request(grx_data, readlen, cfd);
	}
	Socket_Close (cfd);
	return;
}

static void handle_request(char *request, int req_len, int cfd)
{
	char method[128], uri[128], version[128];
	int status = 0, len = 0;
	char buf[1024];
	int ResponseCode, ContentType, DataLen;
	struct stat file_info;
	int fd;

	/* Extract method, uri & version */
	sscanf(grx_data, "%s %s %s\n", method, uri, version);
	/* Is method supported? */
	status = IsSupported_Method(method);
	if (status != TRUE)
	{
		/* Method not supported */
		return;
	}
	/* TODO: Handle uri */
	ResponseCode = HTTP_200;
	ContentType = HTTP_TEXT_HTML;

	/* Get file length */
	fd = open("res/index.html", S_IRUSR);
	if (fd == -1)
	{
		printf ("File open failed\n");
		perror ("open");
		/* TODO: Set 404 and send */
		return;
	}
	status = fstat(fd, &file_info);
	if (status == -1)
	{
		printf ("Unable to get file details\n");
		perror ("fstat");
		return;
	}
	DataLen = file_info.st_size;
	/*
	 * Send response
	 * -> Send response headers
	 * -> Send data
	 */
	status = send_response_header (cfd, ResponseCode, ContentType, DataLen);
	if (status == FAIL)
	{
		/* Header not sent!! don't send data */
		return;
	}
	/* Send data of DataLen bytes */
	while (DataLen > 0)
	{
		/* Read data */
		status = read (fd, buf, 1024);
		/* send to client (readbytes) */
		send (cfd, buf, status, 0);
		DataLen -= status;
	}
	return;
}

static int IsSupported_Method(char *method)
{
	int status = 0;

	status = strcmp (method, "GET");
	if (status == 0)
	{
		return TRUE;
	}
	/* Unsupported method */

	return FALSE;
}

static int send_response_header (int cfd, int resp, int type, int len)
{
	char buf[1024];
	int offset = 0, ret = 0;

	switch (resp)
	{
		case HTTP_200:
			ret = sprintf (buf+offset, "%s", HTTP_SUCCESS);
			offset += ret;
			break;
		case HTTP_404:
			break;
		default:
			/* Unsupported */
			return FAIL;
	}
	ret = sprintf (buf+offset, "%s", SERVER_NAME);
	offset += ret;
	ret = sprintf (buf+offset, "%s", ACCEPT_RANGES_BYTES);
	offset += ret;
	ret = sprintf (buf+offset, "%s", CONTENT_LENGTH);
	offset += ret;
	ret = sprintf (buf+offset, "%d\n", len);
	offset += ret;
	switch (type)
	{
		case HTTP_TEXT_HTML:
			ret = sprintf (buf+offset, "%s", CONTENT_TYPE_HTML);
			offset += ret;
			break;
		case HTTP_TEXT_PLAIN:
			break;
		default:
			/* Unsupported */
			return FAIL;
	}
	ret = sprintf (buf+offset, "%s", CONNECTION_ALIVE);
	offset += ret;
	ret = sprintf (buf+offset, "%s", HTTP_NEW_LINE);;
	offset += ret;
	/* Send response */
	send (cfd, buf, offset, 0);
	printf ("response %d bytes sent\n", offset);
	dump_data (buf, offset);
}
