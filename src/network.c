/* netToe Version 1.5.1
 *
 * Copyright 2000,2001 Gabriele Giorgetti <ggdev@users.sourceforge.net>
 *           2009-2014 Mats Erik Andersson <meand@users.sourceforge.net>
 *		
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <netdb.h>

#include "network.h"

/* Has network partner reported EOF? */
extern int has_given_eof;

/* Choice of INET address family. */
extern int addrfamily;


int establish_listening_socket(unsigned short port_number, char *peer_ip_address, int len)
{
	int sockfd, new_fd;
	int reuse = 1;
	socklen_t sin_size;
	int BACKLOG = 2;
	struct sockaddr_storage their_addr;
	struct addrinfo hints, *ai, *aiptr;
	char portstr[10];
	int status;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = addrfamily;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
#ifdef AI_ADDRCONFIG
	hints.ai_flags |= AI_ADDRCONFIG;
#endif

	sprintf(portstr, "%u", port_number);

	if ( (status = getaddrinfo(NULL, portstr, &hints, &aiptr)) != 0 ) {
		perror("getaddrinfo");
		fprintf(stderr, "Address failure. Quitting...\n\n");
		return -1;
	}

	/* Prefer IPv6 over IPv4. */
	ai = aiptr;
	while (ai->ai_next && (ai->ai_family == AF_INET))
		ai = ai->ai_next;

	if ( (sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol))
			== -1 )
	{
		perror("socket");
		fprintf(stderr, "Peer error: (socket). Quitting...\n\n");
		exit(EXIT_FAILURE);
	}

	/* avoid "Address already in use" errors when restarting */
	if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
				(void *) &reuse, sizeof (reuse)) == -1 )
	{
		perror("SO_REUSEADDR");
		fprintf(stderr, "Peer error: (SO_REUSEADDR). Quitting...\n\n");
		exit(EXIT_FAILURE);
	}

	/*
	 * When an unspecified family was asked for, and an IPv6
	 * result address resulted, the attempt a dual stacked
	 * listening socket. Otherwise a lock down to a single
	 * address family is sought.
	 */
	if (addrfamily == AF_UNSPEC && ai->ai_family == AF_INET6)
	{
		int data = 0;
		setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY,
			   &data, sizeof(data));
	}
	else if (ai->ai_family == AF_INET6)
	{
		int data = 1;
		setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY,
			   &data, sizeof(data));
	}

	if ( bind(sockfd, ai->ai_addr, ai->ai_addrlen) == -1)
	{
		printf("\n   Network error: Address is in use,");
		printf("or permission was insufficient.\n\n");
		close(sockfd);
		return -1;
	}

	freeaddrinfo(aiptr);

	if ( listen(sockfd, BACKLOG) == -1 )
	{
		perror("listen");
		fprintf(stderr, "Peer error: (bind: address already in use or "
						"permission is denied).\nQuitting...\n\n");
		close(sockfd);
		return -1;
	}

	sin_size = sizeof(their_addr);

	if ( (new_fd = accept(sockfd, (struct sockaddr *) &their_addr,
							&sin_size)) == -1)
	{
		perror("accept");
		fprintf(stderr, "Peer error: (accept). continuing:\n\n");
	}
 
	close(sockfd);

	memset(peer_ip_address, 0, len);

	getnameinfo((struct sockaddr *) &their_addr, sin_size,
				peer_ip_address, len,
				NULL, 0, NI_NUMERICHOST);

	return new_fd;
} /* establish_listening_socket(unsigned short, char *, int) */


int connect_to_socket(char *host_ip_number, unsigned short port_number)
{
	int sockfd = -1, status;
	char portstr[20];
	struct addrinfo hints, *ai, *aiptr;

	//printf("\n   Calling %s.\n\n", host_ip_number);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = addrfamily;
	hints.ai_socktype = SOCK_STREAM;
#ifdef AI_ADDRCONFIG
	hints.ai_flags = AI_ADDRCONFIG;
#endif

	sprintf(portstr, "%u", port_number);

	if ( (status = getaddrinfo(host_ip_number, portstr, &hints, &ai)) )
	{
		/* Failed host lookup. */
		printf("failed\n\n");
		printf("   Incorrect IP address, or failing host name.\n");
		printf("   Try again with a usable destination!\n\n");
		return -1;
	}

	for ( aiptr = ai; aiptr; aiptr = aiptr->ai_next)
	{
		if ((sockfd = socket (aiptr->ai_family, SOCK_STREAM, 0)) == -1)
			continue;

		if ( connect(sockfd, aiptr->ai_addr, aiptr->ai_addrlen) == 0 )
			break;
		
		close(sockfd);
		sockfd = -1;
	} /* for "aiptr" */

	freeaddrinfo(ai);

	if ( aiptr == NULL ) 
	{
		printf("failed\n\n");
		printf("   No game found at: %s\n\n", host_ip_number);
		printf("   Try again if the host was typed correctly.\n");
		printf("   Maybe the host is not ready yet.\n\n");
		return -1;
	}

	return sockfd;
} /* connect_to_socket(char *, unsigned short) */


int read_from_socket(int connected_socket, char *buffer, int bytes)
{
	int bytescount;
	int bytesread;

	bytescount = 0;
	bytesread  = 0;

	while (bytescount < bytes)
	{
		if ( (bytesread = read(connected_socket, buffer, bytes-bytescount))
				> 0 )
		{
			bytescount += bytesread;
			buffer += bytesread;
			continue;
		}
		else if (bytesread < 0)
			return -1;

		/* EOF from other end. */
		has_given_eof = 1;
		break;
	}

	return bytescount;
} /* read_from_socket( int, char *, int) */


int write_to_socket(int connected_socket, char *buffer, int bytes)
{
	int bytescount;
	int byteswritten;

	bytescount = 0;
	byteswritten  = 0;

	while (bytescount < bytes)
	{
		if ( (byteswritten = write(connected_socket, buffer,
									bytes - bytescount))
				> 0)
		{
			bytescount += byteswritten;
			buffer += byteswritten;
		}
		else if ( (byteswritten == 0) && (errno == EPIPE) )
		{
			has_given_eof = 1;
			break;
		}
		else if (byteswritten < 0)
			return -1;
	}

	return bytescount;
} /* write_to_socket(int, char *, int) */


void give_local_IP(char *local_ip_address, int len)
{
	char hostname[100];
	struct addrinfo hints, *ai;
	int status;

	gethostname(hostname, sizeof(hostname));

	local_ip_address[0] = '\0';

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = addrfamily;
	hints.ai_socktype = SOCK_STREAM;
#ifdef AI_ADDRCONFIG
	hints.ai_flags = AI_ADDRCONFIG;
#endif

	if ( (status = getaddrinfo(hostname, NULL, &hints, &ai)) ) {
		printf("Address lookup failed: %s\n", gai_strerror(status));
	} else {
		getnameinfo(ai->ai_addr, ai->ai_addrlen,
					local_ip_address, len,
					NULL, 0, NI_NUMERICHOST);
	}
} /* give_local_IP(char *) */


/* NOT USED */
/* ==================================
const char *give_local_hostname( void )
 {
	char hostname[100];
	struct hostent *h;

	gethostname ( hostname, 100);

	if ((h=gethostbyname(hostname)) == NULL) {
	          herror("gethostbyname");
	      }
	      strcpy(hostname, h->h_name );
	      return ( hostname );
}
*/
