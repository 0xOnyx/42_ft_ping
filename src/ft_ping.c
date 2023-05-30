#include "ft_ping.h"

unsigned char 	send_buff[BUFF_SIZE];
int 			data_len;
char			*host;
pid_t 			pid;
int 			nsent;
int 			nrecv;
int				sockfd;
int 			verbose;
struct proto 	pr;

static int get_nbr(char *str)
{
	long 	val;
	char	*end_ptr;

	errno = 0;
	val = strtol(str, &end_ptr, 10);
	if (errno != 0)
	{
		perror("-s errqor:");
		exit(EXIT_FAILURE);
	}
	if (end_ptr == str || *end_ptr != '\0')
	{
		dprintf(2, "-s require number value\n");
		exit(EXIT_FAILURE);
	}
	return ((int)val);
}

static void	set_arg(int argc, char **argv)
{
	int		c;

	data_len = 56;
	verbose = 0;
	nsent = 0;
	nrecv = 0;
	pid = getpid() & 0xFFFF;
	opterr = 0;
	while((c = getopt(argc, argv, "vs:")) != -1)
	{
		if (c == 'v')
			verbose++;
		else if(c == 's')
			data_len = get_nbr(optarg);
		else if (c == '?')
		{
			if (optopt == 's')
				dprintf(2, "Option -%c requires an argument\n", optopt);
			else if (isprint(optopt))
				dprintf(2, "Unknown option `-%c`\n", optopt);
			else
				dprintf(2, "Unknown option character `\\x%x`\n", optopt);
		}
	}
	if (optind != argc - 1)
	{
		dprintf(1, "Usage: ping [ -vs ] <hostname>\n");
		exit(EXIT_FAILURE);
	}
	host = argv[optind];
}

int	main(int argc, char **argv)
{
	struct addrinfo		*ai;
	const char			*ip_value;

	set_arg(argc, argv);

//	ai = Host_serv(host, NULL, 0, SOCK_RAW);  IPV6 TODO UNCOMMENT THIS
	ai = Host_serv(host, NULL, AF_INET, SOCK_RAW);  //FOR CE IPV4 commente this is only for dev
	ip_value = Sock_ntop_host(ai->ai_addr);
	signal(SIGALRM, &sig_alarm);
	printf("PING %s (%s): %d data bytes\n",
		   host, ip_value, data_len);
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
	pr.sarecv = calloc(0, ai->ai_addrlen);
	pr.salen = ai->ai_addrlen;
	pr.canonname = ai->ai_canonname;
	readloop();
	freeaddrinfo(ai);
	close(sockfd);
	return (EXIT_SUCCESS);
}