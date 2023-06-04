#include "ft_ping.h"

void	readloop(void)
{
	int 			size;
	struct msghdr	msg;
	struct iovec	iov;
	struct timeval	tval;
	ssize_t			n;
	char 			recv_buff[BUFF_SIZE];
	char 			control_buff[BUFF_SIZE];

	size = 60 * 1024;
	if ((sockfd = socket(pr.sasend->sa_family, SOCK_RAW, pr.icmp_proto)) == -1
	|| setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size))) // set the socket receive buffer size to 61,440 bytes (60 x 1024) for  IPv4 broadcast address or a multicast address)
	{
		dprintf(2, "error with the socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}


	if (ttl != -1 && pr.sasend->sa_family == AF_INET)
		setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
	else if (pr.sasend->sa_family == AF_INET6)
		init_v6();

	setuid(getuid()); //delete all permission and go to real user ID of calling process
	pr.rtt.max = -1;
	pr.rtt.min = -1;
	pr.rtt.mdev = -1;
	pr.rtt.avg = 0;

	struct itimerval	iter;
	iter.it_value.tv_sec = 0;
	iter.it_value.tv_usec = 1; //time befor first alarm
	fill_timeval(&iter.it_interval, interval);
	setitimer(ITIMER_REAL, &iter, NULL);


	iov.iov_base = recv_buff;
	iov.iov_len = sizeof(recv_buff);
	msg.msg_control = control_buff;
	msg.msg_flags = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_name = pr.sarecv;  //error;
	msg.msg_flags = 0;
	while(is_running)
	{
		msg.msg_namelen = pr.salen;
		msg.msg_controllen = sizeof(control_buff);
		memset(recv_buff, 0, sizeof(recv_buff));
	    n = recvmsg(sockfd, &msg, MSG_DONTWAIT);
		if (is_running == 0)
			return ;
		if (n < 0)
		{
			if (errno == EINTR || errno == EAGAIN) //for prevent alarm signal
				continue ;
			else
				dprintf(2, "recvmsg error %d\n", errno);
		}
		gettimeofday(&tval, NULL);
		(*pr.fproc)(n, &msg, &tval);
	}
}