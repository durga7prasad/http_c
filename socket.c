#include "socket.h"

/* Creates a socket and returns fd */

int Socket_Create(int *sfd, int domain, int type, int protocol)
{
	int status = 0;
	int option = 1;

	/* Create a socket */
	status = socket (domain, type, protocol);
	if (status == -1)
	{
		printf ("Failed to create socket\n");
		perror ("Socket Err:");
		return status;
	}
	else
	{
		*sfd = status;
	}
	status = setsockopt(*sfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if (status != 0)
	{
		printf ("Failed to set socket option\n");
		perror ("setsockopt Err:");
	}
	return status;
}

/* Binds the socket fd to the given port */

int Socket_Bind(int sfd, int port)
{
	int status = 0;
	struct sockaddr_in saddr;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	status = bind (sfd, (struct sockaddr *)&saddr, sizeof(saddr));
	if (status != 0)
	{
		printf ("bind failed with port: %d\n", port);
		perror ("Bind Err:");
	}
	printf ("Server bind with port:%d\n", port);
	return status;
}

int Socket_Listen(int sfd, int backlog)
{
	int status = 0;

	status = listen(sfd, backlog);
	if (status != 0)
	{
		printf ("listen failed with fd: %d\n", sfd);
		perror ("Listen Err:");
	}
	return status;
}

int Socket_Close (int sfd)
{
	int status = 0;

	status = close(sfd);
	if (status != 0)
	{
		printf ("socket (%d) close failed\n", sfd);
		perror ("Close Err:");
	}
	return status;
}

int Socket_Accept(int *cfd, int sfd, struct sockaddr *caddr, socklen_t *caddrlen)
{
	int status = 0;

	status = accept(sfd, caddr, caddrlen);
	if (status == -1)
	{
		/* Accept failed */
		printf ("Accept failed\n");
		perror ("Accept Err:");
		return status;
	}
	/* store the client fd */
	*cfd = status;
	printf ("Client got connected (%d)\n", status);
	return 0;
}
