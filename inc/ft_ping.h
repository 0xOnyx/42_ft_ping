#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <errno.h>
# include <signal.h>
# include <ctype.h>
# include <sys/time.h>
# include <aio.h>
# include <stddef.h>
# include <math.h>

//IPV4
# include <netdb.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netinet/ip.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>

//IPV6
# include <netinet/ip6.h>
# include <netinet/icmp6.h>

# define HEADER_ICMP 	8
# define BUFF_SIZE 		1280

extern unsigned char 	send_buff[BUFF_SIZE];
extern int 				data_len;
extern char				*host;
extern pid_t 			pid;
extern long int			nsent;
extern long int			nrecv;
extern int				sockfd;
extern int 				verbose;
extern double			interval;
extern int 				flood;
extern int 				ttl;
extern struct proto		pr;
extern volatile int		is_running;
extern struct timeval	start_time;

const char 		*Sock_ntop_host(const struct sockaddr *sa);
void 			tv_sub(struct timeval *out, struct timeval *in);
struct addrinfo	*Host_serv(const char *host, const char *serv, int family, int socktype);
uint16_t 		checksum(void *, int len);
const char 		*reverse_dns_lookup(struct sockaddr *addr, socklen_t len);
int				get_int_nbr(char *str);
double 			get_double_nbr(char *str);
void			print_state(struct timeval *start_time);
void			sig_alarm(int signal);
void 			set_rtt(double rtt);
void 			fill_timeval(struct timeval *tv, double seconds);
void			init_v6(void);

struct rtt	{
	double	min;
	double	avg;
	double	max;
	double	mdev;
};

struct	proto {
	void 	(*fsend)();
	void 	(*fproc)(ssize_t, struct msghdr *, struct timeval *);
	struct	sockaddr	*sasend;
	struct	sockaddr	*sarecv;
	socklen_t 			salen;
	const char 			*canonname;
	int					icmp_proto;
	struct rtt			rtt;
};

void	send_v4(void);
void	proc_v4(ssize_t n, struct msghdr *msg, struct timeval *tval);

void	send_v6(void);
void 	proc_v6(ssize_t n, struct msghdr *msg, struct timeval *tval);

void 	readloop(void);

#endif