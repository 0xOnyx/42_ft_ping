#include "ft_ping.h"

unsigned char 	send_buff[BUFF_SIZE];
int 			data_len;
char			*host;
pid_t 			pid;
long int		nsent;
long int		nsent_max = -1;
long int		nrecv;
int				sockfd;
int 			verbose;
int 			flood = 0;
double 			interval = 1.0f;
int				ttl = -1;
volatile int	is_running = 1;
struct proto 	pr;
struct timeval	start_time;

void	sig_alarm(int signal)
{
	(void)signal;

	if (nsent >= nsent_max && nsent_max != -1)
		is_running = 0;
	else
		pr.fsend();
}

void sig_quit(int signal)
{
	(void)signal;
	is_running = 0;
}

static void	set_arg(int argc, char **argv)
{
	int		c;

	data_len = 56;
	verbose = 0;
	nsent = 0;
	nrecv = 0;
	opterr = 0;
	pid = getpid() & 0xFFFF;
	while((c = getopt(argc, argv, "Vvfs:t:i:c:")) != -1)
	{
		if (c == 'v')
			verbose++;
		else if (c == 'f')
		{
			flood++;
			interval = 0.01;
		}
		else if (c == 'c')
		{
			nsent_max = get_int_nbr(optarg);
			if (nsent_max <= 0 || nsent_max >= 9223372036854775807)
			{
				dprintf(2, "ft_ping: invalid argument: '%ld': out of range: 0 <= value <= 9223372036854775807\n", nsent_max);
				exit(EXIT_SUCCESS);
			}
		}
		else if (c == 'i')
		{
			interval = get_double_nbr(optarg);
			printf("interval => %lf %s\n", interval, optarg);
			if (interval < 0.001)
			{
				dprintf(2, "ft_ping: invalid argument: '%f': out of range: 0 <= value\n", interval);
				exit(EXIT_SUCCESS);
			}
		}
		else if (c == 't')
		{
			ttl = get_int_nbr(optarg);
			if (ttl <= 0 || ttl >= 255)
			{
				dprintf(2, "ft_ping: invalid argument: '%d': out of range: 0 <= value <= 255\n", ttl);
				exit(EXIT_SUCCESS);
			}
		}
		else if(c == 's')
			data_len = get_int_nbr(optarg);
		else if (c == 'V')
		{
			printf("ft_ping from 42 jerdos-s\n");
			exit(EXIT_SUCCESS);
		}
		else if (c == '?')
		{
			if (optopt == 's' || optopt == 't' || optopt == 'i' || optopt == 'c')
				dprintf(2, "Option -%c requires an argument\n", optopt);
			else if (isprint(optopt))
				dprintf(2, "Unknown option `-%c`\n", optopt);
			else
				dprintf(2, "Unknown option character `\\x%x`\n", optopt);
		}
	}
	if (optind != argc - 1)
	{
		dprintf(1, "Usage: ping [ -Vvfstic ] <hostname>\n");
		exit(EXIT_FAILURE);
	}
	host = argv[optind];
}

int	main(int argc, char **argv)
{
    struct sockaddr_storage	sarecv;
	struct addrinfo			*ai;
	const char				*ip_value;

	set_arg(argc, argv);

	ai = Host_serv(host, NULL, 0, SOCK_RAW); /// IPV6
	ip_value = Sock_ntop_host(ai->ai_addr);
	signal(SIGALRM, &sig_alarm);
	signal(SIGINT, &sig_quit);
	printf("PING %s (%s) %d(%d) bytes of data.\n",
		   host, ip_value, data_len, data_len + HEADER_ICMP + (ai->ai_family == AF_INET ? 20 : 40));
	if (ai->ai_family == AF_INET)
	{
		pr.fproc = &proc_v4;
		pr.fsend = &send_v4;
		pr.icmp_proto = IPPROTO_ICMP;
	}
	else if (ai->ai_family == AF_INET6)
	{
		pr.fproc = &proc_v6;
		pr.fsend = &send_v6;
		pr.icmp_proto = IPPROTO_ICMPV6;
		if (IN6_IS_ADDR_V4MAPPED(
				&(((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr) ))
		{
			dprintf(2, "cannot ping IPv4 mapped IPv6 address");
			return (EXIT_FAILURE);
		}
	}
	else
	{
		dprintf(2, "unknown address family %d\n", ai->ai_family);
		return (EXIT_FAILURE);
	}
	pr.sasend = ai->ai_addr;
	pr.sarecv = (struct sockaddr *)&sarecv;
	pr.salen = ai->ai_addrlen;
	pr.canonname = reverse_dns_lookup(pr.sasend, pr.salen);
	if (pr.canonname)
		pr.canonname = strdup(pr.canonname);
	gettimeofday(&start_time, 0);
	readloop();
	print_state(&start_time);
	freeaddrinfo(ai);
	if (pr.canonname)
		free((void *)pr.canonname);
	close(sockfd);
	return (EXIT_SUCCESS);
}