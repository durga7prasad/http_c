#include "httpd.h"
#include "socket.h"
#include <string.h>

static void httpd_handler(int cfd);

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
	char buf[1024] = {0};
	char method[1024], uri[1024], version[1024];
	FILE *fp;

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
		sscanf(grx_data, "%s %s %s\n", method, uri, version);
		strcpy (buf, version);
		strcat (buf, " 200 OK\n");
		strcat (buf, "Server: VDP_HTTPD\n");
		strcat (buf, "Accept-Ranges: bytes\n");
		strcat (buf, "Content-Length: 5\n");
		strcat (buf, "Content-Type: text/html\n");
		strcat (buf, "Connection: Keep-Alive\n\n");
		send (cfd, buf, 121, 0);
		printf ("response sent\n");
#if 1
		send (cfd, "Hello",5,0);
		dump_data (buf, 1024);
#else
		fp = fopen ("dp.jpg", "rb");
		if (fp == NULL)
		{
			perror ("fopen Err:");
			break;
		}
		int len = 47894;
		while (len)
		{
			readlen = fread(buf, 1, 1024, fp);
			//send the read bytes
			send (cfd, buf, readlen, 0);
			//printf ("Sent: %d (%d)\n", readlen, len);
			len -= readlen;
		}
#endif
	}
	Socket_Close (cfd);
	return;
}
