#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>

#define PCKT_SIZE 64

unsigned short checksum(void *b, int len)
{
	unsigned short	*addr = (unsigned short *)b;
	register long	sum = 0;

	while( len > 1 )  {
		sum += *addr++;
		len -= 2;
	}
	if ( len > 0 )
		sum += *addr;
	while (sum>>16)
		sum = (sum & 0xffff) + (sum >> 16);
	return (~sum);
}

typedef struct s_pckt {
	struct icmphdr			header;
	char 					data[PCKT_SIZE - sizeof(struct icmphdr)];
} t_pckt;


int 	get_socket(int *sockfd, struct addrinfo *current)
{
	if ((*sockfd = socket(current->ai_family, current->ai_socktype, current->ai_protocol)) == -1
	|| setsockopt(*sockfd, ))
	{

	}
}

void 	create_ping4(char *host, struct addrinfo *current)
{
	char					name_addr[INET_ADDRSTRLEN];
	struct sockaddr_in		client;
	t_pckt					pckt;
	int 					msg_count = 0;
	int 					sockfd = -1;


	if (get_socket(&socket, current)
		&& !inet_ntop(current->ai_family, (const void *)&(((struct sockaddr_in *)(current->ai_addr))->sin_addr), name_addr, INET_ADDRSTRLEN))
	{
		if (sockfd < 0)
			close(sockfd);
		printf("error => %d\n", sockfd);
		exit(1);
	}
	pckt.header.type = ICMP_ECHO;
	pckt.header.code = 0;
	pckt.header.un.echo.id = getpid();
	while (msg_count < 1)
	{
		pckt.header.un.echo.sequence = msg_count++;
		pckt.header.checksum = checksum(&pckt, sizeof(pckt));
		int send = sendto(sockfd, &pckt, sizeof(pckt), 0, current->ai_addr, current->ai_addrlen);
		printf("send %d\n", send);
	}
}


int	main(int argc, char **argv)
{


	int 					status;
	struct addrinfo			hint;
	struct addrinfo			*res;

	memset(&hint, 0, sizeof (struct addrinfo));
	hint.ai_flags = AI_ADDRCONFIG;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_RAW;
	hint.ai_protocol = IPPROTO_ICMP;


	if ((status = getaddrinfo("google.com", NULL, &hint, &res)) != 0)
	{
		printf("[error]\t%s\n", gai_strerror(status));
		return (1);
	}
	for (struct addrinfo *current = res; current != NULL; current = current->ai_next)
	{
		if (current->ai_family == AF_INET)
			create_ping4(argv[0], current);

	}
	freeaddrinfo(res);
	return (0);
}