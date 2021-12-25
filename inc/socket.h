/*
 * Author - VenkataDurgaPrasad.B
 */

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdio.h>
#include <unistd.h>

/* Socket headers */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Creates a socket and returns fd */

int Socket_Create(int *sfd, int domain, int type, int protocol);

/* Binds the socket fd to the given port */

int Socket_Bind(int sfd, int port);

int Socket_Listen(int sfd, int backlog);

int Socket_Close (int sfd);

int Socket_Accept(int *cfd, int sfd, struct sockaddr *caddr, socklen_t *caddrlen);

#endif	/* _SOCKET_H_ */
