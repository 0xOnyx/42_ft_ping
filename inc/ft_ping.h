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
# define BUFF_SIZE 		1024

int 	data_len;
char	*host;
pid_t 	pid;
int 	nsent;
int 	nrecv;
int		sockfd;
int 	verbose;

void 			sig_alarm(int signal);
const char 		*Sock_ntop_host(const struct sockaddr *sa);
void 			tv_sub(struct timeval *out, struct timeval *in);
struct addrinfo	*Host_serv(const char *host, const char *serv, int family, int socktype);

struct	proto {
	void 	(*fsend)();
	void 	(*fproc)(ssize_t, struct msghdr *, struct timeval *);
	struct	sockaddr	*sasend;
	struct	sockaddr	*sarecv;
	socklen_t 			salen;
	int					icmp_proto;
} pr;

void	send_v4();
void	proc_v4(ssize_t n, struct msghdr *msg, struct timeval *tval);

void	send_v6();
void 	proc_v6(ssize_t n, struct msghdr *msg, struct timeval *tval);

void 	readloop(void);

#endif