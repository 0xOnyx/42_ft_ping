#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>

void	send_icmp()
{
	sendto
}

void	recv_icmp()
{
	recvfrom();
}

int	create_ping_4(struct addrinfo *addr)
{
	int 	sockfd;

	if ((sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) != 0
|| setsockopt(sockfd, IPPROTO_IP, IP))
	{
		printf("[error]\t%s\n", strerror(errno));
		return (1);
	}

	send_icmp();


	close();
	return (0);
}

int	main(int argc, char **argv)
{
	struct addrinfo		hint;
	struct addrinfo		*res;
	char 				*name;
	int 				status;


	memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_flags = AI_ADDRCONFIG;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_RAW;
	hint.ai_protocol = IPPROTO_ICMP;
	name = argv[1];
	if ((status = getaddrinfo(name, NULL, &hint, &res)) != 0)
	{
		printf("[error]\t%s\n", gai_strerror(status));
		return (1);
	}
	if (!res || create_ping_4(res))
		return (3);
	freeaddrinfo(res);
}