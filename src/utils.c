#include "ft_ping.h"

struct addrinfo	*Host_serv(const char *host, const char *serv, int family, int socktype)
{
	int 			n;
	struct addrinfo	hint;
	struct addrinfo *res;

	memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_flags = AI_CANONNAME | AI_ADDRCONFIG;
	hint.ai_family = family;
	hint.ai_socktype = socktype;
	if ((n = getaddrinfo(host, serv, &hint, &res)) != 0)
	{
		dprintf(2, "host_server error for %s, %s: %s",
			   (host == NULL)? "(no hostname)": host,
			   (serv == NULL) ? "(no service name)": serv,
			   gai_strerror(n));
		exit(EXIT_FAILURE);


	}
	return (res);
}

const char		*Sock_ntop_host(const struct sockaddr *sa)
{
	static char str[INET6_ADDRSTRLEN];

	if (sa->sa_family == AF_INET)
	{
		struct sockaddr_in	*sin = (struct sockaddr_in *)sa;

		if (inet_ntop(AF_INET, (void *)&sin->sin_addr, str, sizeof(str)) == NULL)
			return (NULL);
		return (str);
	}
	else if (sa->sa_family == AF_INET6)
	{
		struct sockaddr_in6	*sin = (struct sockaddr_in6 *)sa;

		if (inet_ntop(AF_INET6, (void *)&sin->sin6_addr, str, sizeof(str)) == NULL)
			return (NULL);
		return (str);
	}
	return (NULL);
}

void	sig_alarm(int signal)
{
	(void)signal;
	pr.fsend();
	alarm(1);
}

void 	tv_sub(struct timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) < 0)
	{
		--out->tv_sec;
		out->tv_sec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}

uint16_t	checksum(void *b, int len)
{
	uint16_t			*addr	= (uint16_t *)b;
	register uint16_t	sum		= 0;

	while ( len > 1)
	{
		sum += *addr++;
		len -= 2;
	}
	if (len > 0)
		sum += *addr;
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);
	return (~sum);
}