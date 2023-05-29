#include "ft_ping.h"


int	create_ping_4(struct addrinfo *addr)
{
	int 	sockfd;

	if ((sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) != 0
		|| setsockopt(sockfd, IPPROTO_IP, IP))
	{
		printf("[error]\t%s\n", strerror(errno));
		return (1);
	}
	sig_alarm(SIGALRM);
	readloop();
	close(sockfd);
	return (0);
}


void	readloop(void)
{
	int size;

	size = 60 * 1024;
	if ((sockfd = socket(pr.sasend->sa_family, SOCK_RAW, pr.icmp_proto)) == -1
	|| setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size))) // set the socket receive buffer size to 61,440 bytes (60 x 1024) for  IPv4 broadcast address or a multicast address)
	{
		dprintf(2, "error with the socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	setuid(getuid()); //delete all permission and go to real user ID of calling process
	sig_alarm(SIGALRM);

	recvfrom();
}