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
		/*
		 * Client socket closed in handler
		 * So clearing descriptor
		 */
		gcfd = 0;
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
		break;
	}
	Socket_Close (cfd);
	return;
}

static void handle_request(char *request, int req_len, int cfd)
{
	char method[128], uri[128], version[128];
	int status = 0, len = 0;
	char buf[1024];
	int ResponseCode, ContentType, DataLen, method_type;
	struct stat file_info;
	int fd;
	char file_path[128];

	/* Extract method, uri & version */
	sscanf(grx_data, "%s %s %s\n", method, uri, version);
	/* Is method supported? */
	method_type = IsSupported_Method(method);
	if (method_type == METHOD_UNSUPPORTED)
	{
		/* Method not supported */
		/* Set 501 and send */
		ResponseCode = HTTP_501;
		ContentType = HTTP_TEXT_HTML;
		DataLen = sprintf(buf, HTTP_ERR_RESPONSE, 501, "Not Implemented");
		status = send_response_header (cfd, ResponseCode, ContentType, DataLen);
		if (status == FAIL)
		{
			/* Header not sent!! don't send data */
			return;
		}
		send (cfd, buf, DataLen, 0);
		return;
	}
	if (method_type == METHOD_TRACE)
	{
		ResponseCode = HTTP_200;
		ContentType = HTTP_TRACE;
		DataLen = req_len;
		status = send_response_header (cfd, ResponseCode, ContentType, DataLen);
		if (status == FAIL)
		{
			/* Header not sent!! don't send data */
			return;
		}
		/* Send data of DataLen bytes */
		while (DataLen > 0)
		{
			status = (DataLen > 1024) ? 1024: DataLen;
			/* send to client (readbytes) */
			send (cfd, grx_data, status, 0);
			DataLen -= status;
		}
		return;
	}
	/* TODO: Handle uri */
	if (strcmp (uri, "/") == 0)
	{
		strcpy (file_path, "res/Index.html");
	}
	else
	{
		char *temp;
		/* Is it query? */
		temp = strchr(uri, '?');
		if (temp)
		{
			/* removing '?' to get file path */
			*temp = '\0';
		}
		sprintf (file_path, "res/%s", uri);
	}
	ResponseCode = HTTP_200;
	ContentType = HTTP_TEXT_HTML;

	/* Get file length */
	fd = open(file_path, S_IRUSR);
	if (fd == -1)
	{
		printf ("File (%s) open failed\n", file_path);
		perror ("open");
		/* Set 404 and send */
		ResponseCode = HTTP_404;
		ContentType = HTTP_TEXT_HTML;
		DataLen = sprintf(buf, HTTP_ERR_RESPONSE, 404, "Not Found");
		status = send_response_header (cfd, ResponseCode, ContentType, DataLen);
		if (status == FAIL)
		{
			/* Header not sent!! don't send data */
			return;
		}
		send (cfd, buf, DataLen, 0);
		return;
	}
	status = fstat(fd, &file_info);
	if (status == -1)
	{
		printf ("Unable to get file details\n");
		perror ("fstat");
		/* Set 500 and send */
		ResponseCode = HTTP_500;
		ContentType = HTTP_TEXT_HTML;
		DataLen = sprintf(buf, HTTP_ERR_RESPONSE, 500, "Internal Server Error");
		status = send_response_header (cfd, ResponseCode, ContentType, DataLen);
		if (status == FAIL)
		{
			/* Header not sent!! don't send data */
			close (fd);
			return;
		}
		send (cfd, buf, DataLen, 0);

		close (fd);
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
		close (fd);
		return;
	}
	/* If method is HEAD, then don't send data */
	if (method_type != METHOD_HEAD)
	{
		/* Send data of DataLen bytes */
		while (DataLen > 0)
		{
			/* Read data */
			status = read (fd, buf, 1024);
			/* send to client (readbytes) */
			send (cfd, buf, status, 0);
			DataLen -= status;
		}
	}

	close(fd);
	return;
}

static int IsSupported_Method(char *method)
{
	int status = 0;

	status = strcmp (method, "GET");
	if (status == 0)
	{
		return METHOD_GET;
	}
	status = strcmp (method, "POST");
	if (status == 0)
	{
		return METHOD_UNSUPPORTED;
	}
	status = strcmp (method, "HEAD");
	if (status == 0)
	{
		return METHOD_POST;
	}
	status = strcmp (method, "PUT");
	if (status == 0)
	{
		return METHOD_UNSUPPORTED;
	}
	status = strcmp (method, "DELETE");
	if (status == 0)
	{
		return METHOD_UNSUPPORTED;
	}
	status = strcmp (method, "OPTIONS");
	if (status == 0)
	{
		return METHOD_UNSUPPORTED;
	}
	status = strcmp (method, "TRACE");
	if (status == 0)
	{
		return METHOD_TRACE;
	}
	/* Unsupported method */

	return METHOD_UNSUPPORTED;
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
			ret = sprintf (buf+offset, "%s", HTTP_NOT_FOUND);
			offset += ret;
			break;
		case HTTP_500:
			ret = sprintf (buf+offset, "%s", HTTP_SERVER_ERR);
			offset += ret;
			break;
		case HTTP_501:
			ret = sprintf (buf+offset, "%s", HTTP_NOT_IMPLEMENT);
			offset += ret;
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
			ret = sprintf (buf+offset, "%s", CONTENT_TYPE_HTML);
			offset += ret;
			break;
		case HTTP_TRACE:
			ret = sprintf (buf+offset, "%s", CONTENT_TYPE_TRACE);
			offset += ret;
			break;
		default:
			/* Unsupported */
			return FAIL;
	}
	ret = sprintf (buf+offset, "%s", CONNECTION_CLOSED);
	offset += ret;
	ret = sprintf (buf+offset, "%s", HTTP_NEW_LINE);;
	offset += ret;
	/* Send response */
	send (cfd, buf, offset, 0);
	printf ("response %d bytes sent\n", offset);
	dump_data (buf, offset);
}
