#include <stdio.h>
#include <stdlib.h>
#include "httpd.h"


int gsfd = 0;
int gcfd = 0;
int gsport = 0;
unsigned char grx_data[MAX_BUFF_LEN] = {0};

int main(int argc, char *argv[])
{
	int status = 0;

	/* If argc is more than 1, then parse the argument */
	if (argc > 1)
	{
		gsport = atoi(argv[1]);
	}
	else
	{
		/* No port configured, use default */
		gsport = 8000;
	}
	status = httpd_init();
	if (status != 0)
	{
		/* httpd init failed */
		printf ("httpd init failed\n");
		return status;
	}
	status = httpd_server();
	if (status != 0)
	{
		/* Server start failed */
		return status;
	}
	status = httpd_handler(gcfd);
	return 0;
}


