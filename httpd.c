#include "httpd.h"
#include "socket.h"

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

	/* wait for client connection */
	printf ("Waiting for client to connect...\n");
	status = Socket_Accept(&gcfd, gsfd, NULL, NULL);
	if (status != 0)
	{
		/* accept failed */
		return status;
	}
}

int httpd_handler(int cfd)
{
	int status = 0;
	int readlen = 0;

	/* Loop forever */
	while (1)
	{
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
			status = 1;
			Socket_Close (cfd);
			break;
		}
		/* We receive data */
		printf ("Got %d Bytes of data\n", readlen);
		printf ("Data:\n%s\n", grx_data);
	}
	return status;
}
