#include "ft_ping.h"

int get_int_nbr(char *str)
{
	long 	val;
	char	*end_ptr;

	errno = 0;
	val = strtol(str, &end_ptr, 10);
	if (errno != 0)
	{
		perror("error:");
		exit(EXIT_FAILURE);
	}
	if (end_ptr == str || *end_ptr != '\0')
	{
		dprintf(2, "require number value\n");
		exit(EXIT_FAILURE);
	}
	return ((int)val);
}

double	get_double_nbr(char *str)
{
	double 	val;
	char	*end_ptr;

	errno = 0;
	val = strtod(str, &end_ptr);
	if (errno != 0)
	{
		perror("error:");
		exit(EXIT_FAILURE);
	}
	if (end_ptr == str || *end_ptr != '\0')
	{
		dprintf(2, "require number value\n");
		exit(EXIT_FAILURE);
	}
	return (val);
}

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
	register uint32_t	sum		= 0;

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

const char 	*reverse_dns_lookup(struct sockaddr *addr, socklen_t len)
{
	static char		buff[NI_MAXHOST];

	if (getnameinfo(addr, len, buff, sizeof(buff), NULL, 0, NI_NAMEREQD))
		return (NULL);
	return (buff);
}

void	print_state(struct timeval *start_time)
{
	int 			lose;
	int 			ratio;
	double			totale_time;
	double			time_elapsed;
	struct timeval	end_time;

	lose = nsent - nrecv;
	if (nsent > 0)
		ratio = (lose * 100) / nsent;
	else
		ratio = 0;
	gettimeofday(&end_time, 0);
	pr.rtt.avg /= nrecv;
	pr.rtt.mdev = pr.rtt.mdev / nrecv;
	pr.rtt.mdev = sqrt(pr.rtt.mdev);
	time_elapsed = ((double)(end_time.tv_usec - start_time->tv_usec)) / 1000.0;
	totale_time = (double)((end_time.tv_sec - start_time->tv_sec) * 1000.0) + time_elapsed;
	printf("\n--- %s ping statistics ---\n", host);
	printf("%ld packets transmitted, %ld received, %d%% packet loss, time %.2lfms\n", nsent, nrecv, ratio, totale_time);
	if (pr.rtt.min != -1)
		printf("rtt min/avg/max/mdev = %.3lf/%.3lf/%.3lf/%.3lf ms\n", pr.rtt.min, pr.rtt.avg, pr.rtt.max, pr.rtt.mdev);
}

void	set_rtt(double rtt)
{
	if (pr.rtt.max == -1 || rtt > pr.rtt.max)
		pr.rtt.max = rtt;
	if (pr.rtt.min == -1 || rtt < pr.rtt.min)
		pr.rtt.min = rtt;
	pr.rtt.avg += rtt;
	double diff = rtt - pr.rtt.avg;
	pr.rtt.mdev += diff * diff;
}

void fill_timeval(struct timeval* tv, double seconds) {
	tv->tv_sec = (time_t)seconds;
	tv->tv_usec = (suseconds_t)((seconds - (double)tv->tv_sec) * 1e6);
}

void	init_v6(void)
{
	struct icmp6_filter	filter;
	int on = 1;

	if (verbose == 0)
	{
		ICMP6_FILTER_SETBLOCKALL(&filter);
		ICMP6_FILTER_SETPASS(ICMP6_ECHO_REQUEST, &filter);
		setsockopt(sockfd, IPPROTO_IPV6, ICMP6_FILTER, &filter, sizeof(filter));
	}
	setsockopt(sockfd, IPPROTO_IPV6, IPV6_HOPLIMIT, &ttl, sizeof(ttl));
	setsockopt(sockfd, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &on, sizeof(on));
}